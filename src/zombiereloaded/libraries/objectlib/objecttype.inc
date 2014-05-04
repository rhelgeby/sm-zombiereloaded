/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           objecttype.inc
 *  Type:           Library
 *  Description:    ObjectType functions for objectlib.
 *
 *  Copyright (C) 2012  Richard Helgeby, Greyscale
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

/**
 * Creates an empty object type descriptor.
 *
 * Note: You may use ByteCountToCells to calculate number of cells required to
 *       fit a certain string length for blockSize and keySize.
 *
 * @param blockSize     (Optional) Maximum number of cells reserved for each
 *                      value entry. Default is 1 cell.
 * @param keySize       (Optional) Maximum number of cells reserved for each
 *                      key name. Default is 8 cells (32 bytes/characters).
 *                      Make sure OBJECT_KEY_NAME_LEN also is big enough (global
 *                      upper limit).
 * @param errorHandler  (Optional) Callback for general error handler. Called
 *                      when a recoverable error occured.
 *
 * @return              Reference to type descriptor. Must be deleted with
 *                      ObjLib_DeleteType when no longer in use.
 *
 *                      Note: Don't delete type descriptors if there are objects
 *                      using it. Delete objects first.
 */
stock ObjectType:ObjLib_CreateType(blockSize = 1, keySzie = 8, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Create array to store raw type descriptor data. Reserve space required.
    new ObjectType:typeDescriptor = ObjectType:CreateArray(_, OBJECT_TYPE_DATA_LEN);
    
    new Handle:keys = CreateArray(keySzie);         // List of key names (for enumeration or reflection).
    new Handle:nameIndex = CreateTrie();            // ADT Trie with key names mapped to indexes.
    new Handle:dataTypes = CreateArray();           // List of data types of each value entry.
    new Handle:constraintList = CreateArray();      // List of constraint objects for each value entry.
    
    ObjLib_SetTypeLocked(typeDescriptor,        false);
    ObjLib_SetTypeParentObject(typeDescriptor,  INVALID_OBJECT);
    ObjLib_SetTypeKeySize(typeDescriptor,       keySzie);
    ObjLib_SetTypeBlockSize(typeDescriptor,     blockSize);
    ObjLib_SetTypeKeys(typeDescriptor,          keys);
    ObjLib_SetTypeNameIndex(typeDescriptor,     nameIndex);
    ObjLib_SetTypeDataTypes(typeDescriptor,     dataTypes);
    ObjLib_SetTypeConstraints(typeDescriptor,   constraintList);
    ObjLib_SetTypeErrorHandler(typeDescriptor,  errorHandler);
    
    ObjectTypeCount++;
    return typeDescriptor;
}

/*____________________________________________________________________________*/

/**
 * Deletes an object type descriptor, and constraint objects attached to it.
 *
 * Warning: If there are objects refering to this type descriptor they may
 *          trigger an error when trying to use it. Delete objects first.
 *
 * @param typeDescriptor    Type descriptor to delete.
 * @param resetReference    (Optional) Reset typeDescriptor to
 *                          INVALID_OBJECT_TYPE when deleted. Default is true.
 * @param deleteConstraints (Optional) Delete constraint objects attached to
 *                          keys. Default is true.
 *
 * @error                   Invalid object type.
 */
stock ObjLib_DeleteType(&ObjectType:typeDescriptor, bool:resetReference = true, bool:deleteConstraints = true)
{
    // Validate.
    ObjLib_ValidateObjectType(typeDescriptor);
    
    // Check if this type descriptor is attatched to a mutable object.
    if (ObjLib_GetTypeParentObject(typeDescriptor) != INVALID_OBJECT)
    {
        ThrowError("Can't directly delete type descriptor in mutable object. Use ObjLib_DeleteObject.");
    }
    
    // Delete data structures stored in the type descriptor.
    CloseHandle(ObjLib_GetTypeKeys(typeDescriptor));
    CloseHandle(ObjLib_GetTypeNameIndex(typeDescriptor));
    CloseHandle(ObjLib_GetTypeDataTypes(typeDescriptor));
    
    new Handle:constraintList = ObjLib_GetTypeConstraints(typeDescriptor);
    new len = GetArraySize(constraintList);
    
    // Delete constraint objects, if enabled and they exist.
    if (deleteConstraints)
    {
        for (new i = 0; i < len; i++)
        {
            new Object:constraintObject = Object:GetArrayCell(constraintList, i);
            if (constraintObject != INVALID_OBJECT)
            {
                // Delete constraint. Also close handles stored in keys. Some
                // constraints store handles to stuff no longer in use.
                ObjLib_DeleteObject(constraintObject, true, true);
            }
        }
    }
    
    // Delete constraints array.
    CloseHandle(constraintList);
    
    // Delete type descriptor container.
    CloseHandle(Handle:typeDescriptor);
    
    // Reset reference if enabled.
    if (resetReference)
    {
        typeDescriptor = INVALID_OBJECT_TYPE;
    }
    
    ObjectTypeCount--;
}

/*____________________________________________________________________________*/

/**
 * Creates a new object type based on the specified type descriptor.
 *
 * Note: blockSize and keySize of the new cloned object type cannot be modified.
 * Note: The new object type will not be attatched to an object.
 *
 * @param typeDescriptor    Source object type template.
 * @param locked            (Optional) Whether the new object type is locked.
 *                          Default is false.
 * @param errorHandler      (Optional) Callback for general error handler. Called
 *                          when a recoverable error occured. Use
 *                          INVALID_FUNCTION to use the error handler in
 *                          typeDescriptor.
 *
 * @return                  New object type descriptor. Must be deleted with
 *                          ObjLib_DeleteType when no longer in use.
 *
 * @error                   Invalid object type or trying to clone a constraint
 *                          type.
 */
stock ObjectType:ObjLib_CloneType(ObjectType:typeDescriptor, bool:locked = false, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate.
    ObjLib_ValidateObjectType(typeDescriptor);
    
    // Stop possible infinite recursion or users trying to clone constraint
    // objects.
    if (ObjLib_IsConstraintType(typeDescriptor))
    {
        // Note: Possible recursive reentry when cloning constraints.
        // Constraints are objects that must be cloned too. They are immutable
        // and shouldn't need to clone their type. If they do, it's a bug and
        // will cause an infinite loop or eventually stack overflow.
        ThrowError("Constraint types cannot be cloned.");
    }
    
    new keySize = ObjLib_GetTypeKeySize(typeDescriptor);
    new blockSize = ObjLib_GetTypeBlockSize(typeDescriptor);
    
    new Handle:keys = CloneArray(ObjLib_GetTypeKeys(typeDescriptor));
    new Handle:nameIndex = ObjLib_CloneTrie(ObjLib_GetTypeNameIndex(typeDescriptor), keys, keySize);
    new Handle:dataTypes = CloneArray(ObjLib_GetTypeDataTypes(typeDescriptor));
    new Handle:sourceConstraints = ObjLib_GetTypeConstraints(typeDescriptor);
    new ObjLib_ErrorHandler:newErrorHandler = ObjLib_GetErrorHandler(typeDescriptor, errorHandler);
    
    new numKeys = GetArraySize(keys);
    new Handle:constraintList = CreateArray(_, numKeys);
    
    // Clone constraint objects, if any.
    for (new i = 0; i < numKeys; i++)
    {
        new Object:constraintObject = Object:GetArrayCell(sourceConstraints, i);
        if (constraintObject == INVALID_OBJECT)
        {
            // Reserve space for constraint.
            PushArrayCell(constraintList, INVALID_OBJECT);
        }
        else
        {
            // Create an immutable clone.
            new Object:cloned = ObjLib_CloneObject(constraintObject, false);
            PushArrayCell(constraintList, cloned);
        }
    }
    
    new ObjectType:newType = ObjectType:CreateArray(_, OBJECT_TYPE_DATA_LEN);
    
    ObjLib_SetTypeLocked(newType,       locked);
    ObjLib_SetTypeParentObject(newType, INVALID_OBJECT);
    ObjLib_SetTypeKeySize(newType,      keySize);
    ObjLib_SetTypeBlockSize(newType,    blockSize);
    ObjLib_SetTypeKeys(newType,         keys);
    ObjLib_SetTypeNameIndex(newType,    nameIndex);
    ObjLib_SetTypeDataTypes(newType,    dataTypes);
    ObjLib_SetTypeConstraints(newType,  constraintList);
    ObjLib_SetTypeErrorHandler(newType, newErrorHandler);
    
    ObjectTypeCount++;
    return newType;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the object type descriptor is valid.
 *
 * @param typeDescriptor    Type descriptor to validate.
 *
 * @return                  True if valid, false otherwise.
 */
stock bool:ObjLib_IsValidObjectType(ObjectType:typeDescriptor)
{
    return typeDescriptor != INVALID_OBJECT_TYPE;
}

/*____________________________________________________________________________*/

/**
 * Adds a new key to an object type descriptor.
 * 
 * @param typeDescriptor    Handle to type descriptor.
 * @param keyName           Name of the new key. Case sensitive.
 * @param dataType          Data type of key.
 * @param constraints       Key constraints. Constraint type must match data
 *                          type. Use INVALID_OBJECT to disable constraints.
 *
 * @error                   Invalid object type, invalid constraint object,
 *                          type is not mutable, or key name already exist.
 */
stock ObjLib_AddKey(ObjectType:typeDescriptor, const String:keyName[], ObjectDataType:dataType = ObjDataType_Any, Object:constraints = INVALID_OBJECT)
{
    // Validate.
    ObjLib_ValidateObjectType(typeDescriptor);
    
    new Object:parent = ObjLib_GetTypeParentObject(typeDescriptor);
    
    // Check if not mutable.
    if (!ObjLib_IsTypeMutable(typeDescriptor)
        && ObjLib_HandleError(typeDescriptor, parent, ObjLibError_Immutable, _, _, _, "This object type descriptor is immutable (%x).", typeDescriptor))
    {
        return;
    }
    
    // Check if key exist.
    if (ObjLib_KeyExist(typeDescriptor, keyName)
        && ObjLib_HandleError(typeDescriptor, parent, ObjLibError_KeyExist, _, _, _, "Key name already exist (\"%s\"). You might want to use an array/collection if this was intended.", keyName))
    {
        return;
    }
    
    // Validate constraints.
    ObjLib_ValidateConstraintOrFail(constraints, dataType);
    
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    new Handle:nameIndex = ObjLib_GetTypeNameIndex(typeDescriptor);
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    new Handle:constraintList = ObjLib_GetTypeConstraints(typeDescriptor);
    
    // Update type descriptor entries.
    new entryIndex = PushArrayString(keys, keyName);    // Add key name.
    SetTrieValue(nameIndex, keyName, entryIndex);       // Add to name index.
    PushArrayCell(dataTypes, dataType);                 // Set data type.
    PushArrayCell(constraintList, constraints);         // Add constraints.
    
    // Update parent object if available.
    if (parent != INVALID_OBJECT)
    {
        // Add new data entry. This will push a new value at the same index as
        // the key. Value is also marked as null.
        ObjLib_AddNewValue(parent, dataType);
    }
}

/*____________________________________________________________________________*/

/**
 * Removes a key from an object type descriptor, if not locked.
 *
 * @param typeDescriptor    Object type descriptor to remove key from.
 * @param keyName           Name of key to remove. Case sensitive.
 * @param deleteConstraint  (Optional) Delete constratint object, if any.
 *                          Default is true.
 *
 * @return                  True if successful, false if type descriptor was
 *                          locked.
 *
 * @error                   Invalid object type, type is not mutable or key name
 *                          doesn't exist.
 */
stock bool:ObjLib_RemoveKey(ObjectType:typeDescriptor, const String:keyName[], bool:deleteConstraint = true)
{
    // Validate.
    ObjLib_ValidateObjectType(typeDescriptor);
    
    new Object:parent = ObjLib_GetTypeParentObject(typeDescriptor);
    
    // Check if not mutable.
    if (!ObjLib_IsTypeMutable(typeDescriptor)
        && ObjLib_HandleError(typeDescriptor, parent, ObjLibError_Immutable, _, _, _, "This object type descriptor is immutable (%x).", typeDescriptor))
    {
        return;
    }
    
    // Check if key doesn't exist.
    if (!ObjLib_KeyExist(typeDescriptor, keyName)
        && ObjLib_HandleError(typeDescriptor, parent, ObjLibError_InvalidKey, _, _, _, "Key doesn't exist (\"%s\").", keyName))
    {
        return;
    }
    
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    new Handle:nameIndex = ObjLib_GetTypeNameIndex(typeDescriptor);
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    new Handle:constraintList = ObjLib_GetTypeConstraints(typeDescriptor);
    
    // Get entry index.
    new entryIndex = -1;
    GetTrieValue(nameIndex, keyName, entryIndex);
    
    // Delete constraint if enabled and available.
    if (deleteConstraint)
    {
        new Object:constraint = Object:GetArrayCell(constraintList, entryIndex);
        if (constraint != INVALID_OBJECT)
        {
            // Delete constraint object.
            ObjLib_DeleteObject(constraint);
        }
    }
    
    // Remove type descriptor entries.
    RemoveFromArray(keys, entryIndex);                  // Remove key.
    RemoveFromArray(dataTypes, entryIndex);             // Remove data type.
    RemoveFromTrie(nameIndex, keyName);                 // Remove from name index.
    RemoveFromArray(constraintList, entryIndex);        // Remove constraint.
    
    // Refresh name index, since keys below this key is shifted up.
    ObjLib_RefreshNameIndex(typeDescriptor);
    
    // Update parent object if available.
    if (parent != INVALID_OBJECT)
    {
        // Remove entry.
        ObjLib_RemoveEntry(parent, entryIndex);
    }
}

/*____________________________________________________________________________*/

/**
 * Returns whether a key exist in an object type descriptor.
 *
 * @param typeDescriptor    Obect type descriptor to inspect.
 * @param keyName           Key to search for. Case sensitive.
 *
 * @return                  True if found, false otherwise.
 */
stock bool:ObjLib_KeyExist(ObjectType:typeDescriptor, const String:keyName[])
{
    new Handle:nameIndex = ObjLib_GetTypeNameIndex(typeDescriptor);
    new dummyBuffer;
    if (GetTrieValue(nameIndex, keyName, dummyBuffer))
    {
        // Key exist.
        return true;
    }
    return false;
}


/******************************************************************************
 *                             INTERNAL USE ONLY                              *
 ******************************************************************************/

/** Internal use only! */
stock ObjLib_ValidateObjectType(ObjectType:typeDescriptor)
{
    if (!ObjLib_IsValidObjectType(typeDescriptor))
    {
        ThrowError("Invalid object type (%x).", typeDescriptor);
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Validates a key type against an object's key type at the specified index.
 *
 * This function will throw an validation error on key type mismatch.
 *
 * @param typeDescriptor    Object type descriptor.
 * @param object            Related object.
 * @param keyIndex          Index of key to validate against.
 * @param keyType           Type of key to validate.
 * @param errorHandler      Custom error handler. Overrides error handler in
 *                          type descriptor if specified.
 *
 * @return                  True if passed, false otherwise.
 */
stock bool:ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, Object:object, keyIndex, ObjectDataType:keyType, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate key index.
    if (keyIndex < 0
        && ObjLib_HandleError(typeDescriptor, object, ObjLibError_InvalidKey, errorHandler, _, _, "Invalid key index (%d).", keyIndex))
    {
        return false;
    }
    
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    new ObjectDataType:destType = ObjectDataType:GetArrayCell(dataTypes, keyIndex);
    
    if (keyType != destType)
    {
        // Key mismatch. Throw error.
        ObjLib_TypeMismatchError(typeDescriptor, object, keyType, destType, errorHandler);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_RefreshNameIndex(ObjectType:typeDescriptor)
{
    decl String:keyName[OBJECT_KEY_NAME_LEN];
    keyName[0] = 0;
    
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    new Handle:nameIndex = ObjLib_GetTypeNameIndex(typeDescriptor);
    new len = GetArraySize(keys);
    
    // Loop through all keys.
    for (new key = 0; key < len; key++)
    {
        // Get key name.
        GetArrayString(keys, key, keyName, sizeof(keyName));
        
        // Update key with new index value.
        SetTrieValue(nameIndex, keyName, key);
    }
}
