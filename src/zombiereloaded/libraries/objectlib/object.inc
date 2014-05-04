/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           object.inc
 *  Type:           Library
 *  Description:    Object functions for objectlib.
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
 * Creates an empty object based on the type template.
 *
 * There are two types of objects, mutable and immutable.
 *
 * Mutable objects store their own copy of the type descriptor when created so
 * that keys can be added or removed. They use a bit more memory because of
 * this, but are flexible.
 *
 * Immutable objects can have its contents modified, but not its type. Keys
 * cannot be added or removed. The type template is used directly and shared
 * between immutable objects. Memory usage is more efficient than mutable
 * objects.
 *
 * Warning:
 * Creating an immutable object will automatically lock the type template
 * because it's now used directly. If you don't want to lock it, clone it with
 * ObjLib_CloneType and use the cloned type template instead.
 *
 * @param typeTemplate      Object type to use.
 * @param mutableObject     (Optional) Whether the new object is mutable.
 *                          Default is false.
 *
 * @return                  Reference to new object. Must be deleted with
 *                          ObjLib_DeleteObject when no longer in use.
 *
 * @error                   Invalid type template.
 */
stock Object:ObjLib_CreateObject(ObjectType:typeTemplate, bool:mutableObject = false)
{
    // Validate.
    ObjLib_ValidateObjectType(typeTemplate);
    
    new Object:object = Object:CreateArray(_, OBJECT_DATA_LEN);
    new blockSize = ObjLib_GetTypeBlockSize(typeTemplate);
    new keyCount = ObjLib_GetNumKeys(typeTemplate);
    
    // Create array for storing raw object data.
    new Handle:data = CreateArray(blockSize);
    new Handle:nullKey = CreateArray(_, keyCount);
    
    // Initialize keys. All keys are null.
    for (new i = 0; i < keyCount; i++)
    {
        PushArrayCell(data, 0);
        PushArrayCell(nullKey, 1);
    }
    
    // Get type descriptor. Mutable objects use their own clone.
    new ObjectType:type;
    if (mutableObject)
    {
        // Clone type descriptor and make it mutable.
        type = ObjLib_CloneType(typeTemplate, false);
        
        // Attach type to cloned object.
        ObjLib_SetTypeParentObject(type, object);
    }
    else
    {
        // Use the type template directly.
        type = typeTemplate;
        
        // Lock type template for further modification.
        ObjLib_SetTypeLocked(type, true);
    }
    
    ObjLib_SetObjectData(object,        data);
    ObjLib_SetObjectNullKey(object,     nullKey);
    ObjLib_SetTypeDescriptor(object,    type);
    
    ObjectCount++;
    return object;
}

/*____________________________________________________________________________*/

/**
 * Create an object using either the first or second type as type template. This
 * is useful if you need a second type to fall back on (for instance a mutable
 * object).
 *
 * The primary type will be checked first. If not specified, it will use the
 * secondary type. At least one type must be specified.
 *
 * @param primaryType           First type to use.
 * @param secondaryType         Second type to use (fallback type).
 * @param mutableIfSecondary    (Optional) Make the new object mutable if
 *                              secondary type was used. Default is false.
 *
 * @return                      Reference to new object. Must be deleted with
 *                              ObjLib_DeleteObject when no longer in use.
 *
 * @error                       No type specified.
 */
stock Object:ObjLib_CreateObjectEx(ObjectType:primaryType = INVALID_OBJECT_TYPE, ObjectType:secondaryType = INVALID_OBJECT_TYPE, bool:mutableIfSecondary = false)
{
    ObjectType:type = INVALID_OBJECT_TYPE;
    new bool:mutableObject = false;
    
    // Use first type available.
    if (primaryType != INVALID_OBJECT_TYPE)
    {
        type = firstType;
    }
    else if (secondaryType != INVALID_OBJECT_TYPE)
    {
        type = secondaryType;
        mutableObject = mutableIfSecondary;
    }
    else
    {
        ThrowError("At least one type must be specified.");
    }
    
    return ObjLib_CreateObject(type, mutableObject);
}

/*____________________________________________________________________________*/

/**
 * Deletes an object and its data.
 *
 * @param object            Object to delete.
 * @param resetReference    (Optional) Reset object to INVALID_OBJECT when
 *                          deleted. Default is true.
 * @param closeHandles      (Optional) Close handles stored in keys of handle
 *                          data type. Default is false.
 *
 * @error                   Invalid object.
 */
stock ObjLib_DeleteObject(&Object:object, bool:resetReference = true, bool:closeHandles = false)
{
    // TODO: Add support for recursive deletion (nested objects/types).
    //       Use a traversal stack. Don't delete objects that's already in the
    //       stack. This should prevent objects from being deleted twice.
    
    // Validate.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    new Handle:data = ObjLib_GetObjectData(object);
    new Handle:nullKey = ObjLib_GetObjectNullKey(object);
    
    // Close handles in keys if enabled.
    if (closeHandles)
    {
        new numKeys = GetArraySize(data);
        new Handle:keyTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
        
        // Loop through keys.
        for (new key = 0; key < numKeys; key++)
        {
            // Check if key is a handle.
            if (ObjectDataType:GetArrayCell(keyTypes, key) == ObjDataType_Handle)
            {
                // Close handle if nonzero.
                new Handle:value = Handle:GetArrayCell(data, key);
                if (value != INVALID_HANDLE)
                {
                    CloseHandle(value);
                }
            }
        }
    }
    
    new Object:parent = ObjLib_GetTypeParentObject(typeDescriptor);
    if (parent != INVALID_OBJECT)
    {
        // Check if the type descriptor actually belong to this object. (Potential bug).
        if (parent != object)
        {
            ThrowError("[BUG] This mutable object (%x) has a type descriptor (%x) that isn't owned by this object. This is a bug!", object, typeDescriptor);
        }
        
        // Unattatch type descriptor from object so it can be deleted.
        ObjLib_SetTypeParentObject(typeDescriptor, INVALID_OBJECT);
        
        // Delete type descriptor.
        ObjLib_DeleteType(typeDescriptor);
    }
    
    // Delete data arrays.
    CloseHandle(data);
    CloseHandle(nullKey);
    
    // Delete object.
    CloseHandle(Handle:object);
    
    // Reset reference if enabled.
    if (resetReference)
    {
        object = INVALID_OBJECT;
    }
    
    ObjectCount--;
}

/*____________________________________________________________________________*/

/**
 * Creates a clone of an object and its data.
 *
 * Note: If the object data contain reference values, only the references
 *       themself will be cloned, not the data they refer to. See the
 *       cloneHandles parameter for more info.
 *
 * Warning: Creating an immutable object from a mutable object will also create
 *          a clone of the source type descriptor. Delete this type descriptor 
 *          when it's no longer in use. It can be retrieved from the cloned
 *          object with ObjLib_GetTypeDescriptor.
 *
 * @param objectTemplate    Source object.
 * @param mutableObject     (Optional) Make new object mutable. Default is true.
 * @param cloneHandles      (Optional) Use CloneHandle on value entries with the
 *                          handle data type. The handles will still refer to
 *                          the same data, but through a new cloned handle.
 *                          Default is false.
 *
 * @return                  Reference to new cloned object.
 *
 * @error                   Invalid object template.
 */
stock Object:ObjLib_CloneObject(Object:objectTemplate, bool:mutableObject = true, bool:cloneHandles = false)
{
    // Validate object.
    ObjLib_ValidateObject(objectTemplate);
    
    // Create new object array.
    new Object:object = Object:CreateArray(_, OBJECT_DATA_LEN);
    
    // Clone data array.
    new Handle:data = CloneArray(ObjLib_GetObjectData(objectTemplate));
    ObjLib_SetObjectData(object, data);
    
    // Clone null key array.
    new Handle:nullKey = CloneArray(ObjLib_GetObjectNullKey(Object:object));
    ObjLib_SetObjectNullKey(object, nullKey);
    
    // Clone type.
    new ObjectType:typeTemplate = ObjLib_GetTypeDescriptor(objectTemplate);
    new bool:mutableSource = ObjLib_IsMutable(objectTemplate);
    
    /*
    The if-statements below decides how the type should be cloned and attached.
    
    m = mutable
    i = immutable
    c = clone type
    cl = clone and lock
    s = use same type
    X = invalid
    
                source-i    dest-i  source-m    dest-m
    source-i    X           s       X           c
    dest-i      s           X       cl          X
    source-m    X           cl      X           c
    dest-m      c           X       c           X
    */
    if (!mutableSource && !mutableObject)
    {
        // Template and destination object is immutable. Use the same type
        // descriptor.
        ObjLib_SetTypeDescriptor(object, typeTemplate);
    }
    else
    {
        // Either source or destination is mutable. Clone type, as unlocked.
        new ObjectType:type = ObjLib_CloneType(typeTemplate, false);
        
        if (mutableObject)
        {
            // Destination is mutable. Attatch cloned type.
            ObjLib_SetTypeParentObject(type, object);
            ObjLib_SetTypeDescriptor(object, type);
        }
        else
        {
            // Destination is immutable. Link cloned type.
            ObjLib_SetTypeDescriptor(object, type);

            // Lock type.
            ObjLib_SetTypeLocked(type, true);
        }
    }
    
    // Clone handles if enabled.
    if (cloneHandles)
    {
        new len = GetArraySize(data);
        new Handle:dataTypes = ObjLib_GetTypeDataTypes(ObjLib_GetTypeDescriptor(object));
        
        // Loop through data values.
        for (new i = 0; i < len; i++)
        {
            new ObjectDataType:dataType = ObjectDataType:GetArrayCell(dataTypes, i);
            if (dataType == ObjDataType_Handle)
            {
                // Get original handle.
                new Handle:value = Handle:GetArrayCell(data, i);
                
                // Replace with cloned handle.
                value = CloneHandle(value);
                SetArrayCell(data, i, value);
            }
        }
    }
    
    ObjectCount++;
    return object;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the object reference is valid.
 *
 * @param object    Object reference to validate.
 *
 * @return          True if valid, false otherwise.
 */
stock bool:ObjLib_IsValidObject(Object:object)
{
    return object != INVALID_OBJECT;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the object is the specitifed type.
 *
 * Note: This is simple type checking. It compares the object's type descriptor
 *       reference against the specified reference.
 *
 * @param object            Object reference to check.
 * @param typeDescriptor    Type to match against.
 *
 * @return                  True if types match, false otherwise.
 */
stock bool:ObjLib_TypeOf(Object:object, ObjectType:typeDescriptor)
{
    return typeDescriptor == ObjLib_GetTypeDescriptor(object);
}

/*____________________________________________________________________________*/

/**
 * Adds a new key to an object, if not locked.
 *
 * @param object        Object to remove key from.
 * @param keyName       Name of key to remove. Case sensitive.
 * @param dataType      Data type of key.
 * @param constraints   Key constraints. Constraint type must match data type.
 *                      Use INVALID_OBJECT to disable constraints.
 *
 * @error               Invalid object, immutable object, invalid constraint
 *                      object, or key name already exist.
 */
stock bool:ObjLib_AddObjectKey(Object:object, const String:keyName[], ObjectDataType:dataType = ObjDataType_Any, Object:constraints = INVALID_OBJECT)
{
    // Validate.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    ObjLib_AddKey(typeDescriptor, keyName, dataType, constraints);
}

/*____________________________________________________________________________*/

/**
 * Removes a key from an object, if not locked.
 *
 * Warning: Data associated with this key will also be deleted.
 *
 * @param object            Object to remove key from.
 * @param keyName           Name of key to remove. Case sensitive.
 * @param deleteConstraint  (Optional) Delete constratint object, if any.
 *                          Default is true.
 *
 * @error                   Invalid object, immutable object, or key name
 *                          doesn't exist.
 */
stock ObjLib_RemoveObjectKey(Object:object, const String:keyName[], bool:deleteConstraint = true)
{
    // Validate.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    ObjLib_RemoveKey(typeDescriptor, keyName, deleteConstraint);
}

/*____________________________________________________________________________*/

/**
 * Resets the value in the specified key (marked as null).
 *
 * @param object    Object to update.
 * @param keyName   Key name.
 *
 * @error           Invalid object.
 */
stock ObjLib_ClearKey(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    ObjLib_ClearKeyAt(object, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Resets the value in the specified key (marked as null).
 *
 * @param object    Object to update.
 * @param keyIndex  Key index.
 *
 * @error           Invalid object.
 */
stock ObjLib_ClearKeyAt(Object:object, keyIndex)
{
    // Validate.
    ObjLib_ValidateObject(object);
    
    ObjLib_SetKeyNull(object, keyIndex, true);
}


/******************************************************************************
 *                             INTERNAL USE ONLY                              *
 ******************************************************************************/

/** Internal use only! */
stock ObjLib_ValidateObject(Object:object)
{
    if (!ObjLib_IsValidObject(object))
    {
        ThrowError("Invalid object (%x).", object);
    }
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_NullCheck(Object:object, keyIndex, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    if (ObjLib_IsKeyNullAt(object, keyIndex))
    {
        // Key is null. Throw error.
        
        new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
        
        // Get Key name.
        new String:keyName[OBJECT_KEY_NAME_LEN];
        new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
        GetArrayString(keys, keyIndex, keyName, sizeof(keyName));
        
        ObjLib_HandleError(typeDescriptor, object, ObjLibError_NullDataKey, errorHandler, _, _, "Key at index %d is null: %s", keyIndex, keyName);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_GetKeyIndexOrFail(Object:object, const String:keyName[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate.
    ObjLib_ValidateObject(object);
    
    new keyIndex = ObjLib_GetKeyIndex(object, keyName);
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    if (keyIndex < 0
        && ObjLib_HandleError(typeDescriptor, object, ObjLibError_InvalidKey, errorHandler, _, _, "Invalid key name (%s).", keyName))
    {
        return -1;
    }
    
    return keyIndex;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Creates a new initialized value entry at the end of the object data array.
 *
 * Note: Don't make new value entries without updating the type descriptor.
 *       These must match.
 *
 * @param object    Object where new value entry is added.
 * @param dataType  Data type of initialized value.
 */
stock ObjLib_AddNewValue(Object:object, ObjectDataType:dataType)
{
    new Handle:data = ObjLib_GetObjectData(object);
    new Handle:nullKey = ObjLib_GetObjectNullKey(object);
    
    switch (dataType)
    {
        case ObjDataType_Any:
        {
            PushArrayCell(data, 0);
        }
        case ObjDataType_Cell:
        {
            PushArrayCell(data, 0);
        }
        case ObjDataType_Bool:
        {
            PushArrayCell(data, 0);
        }
        case ObjDataType_Float:
        {
            PushArrayCell(data, 0.0);
        }
        case ObjDataType_Handle:
        {
            PushArrayCell(data, INVALID_HANDLE);
        }
        case ObjDataType_Function:
        {
            PushArrayCell(data, INVALID_FUNCTION);
        }
        case ObjDataType_Array:
        {
            new dummyArray[1];
            PushArrayArray(data, dummyArray, sizeof(dummyArray));
        }
        case ObjDataType_String:
        {
            PushArrayString(data, "");
        }
        case ObjDataType_Object:
        {
            PushArrayCell(data, INVALID_OBJECT);
        }
        case ObjDataType_ObjectType:
        {
            PushArrayCell(data, INVALID_OBJECT_TYPE);
        }
        default:
        {
            ThrowError("[BUG] Unexpected data type. This is a bug in objectlib.");
        }
    }
    
    // Add a corresponding null key flag. Mark value as null.
    PushArrayCell(nullKey, true);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Removes a value entry from an object.
 *
 * Note: Don't remove value entries without updating the type descriptor.
 *
 * @param object        Object to remove value from.
 * @param entryIndex    Data index of value entry.
 */
stock ObjLib_RemoveEntry(Object:object, entryIndex)
{
    new Handle:data = ObjLib_GetObjectData(object);
    RemoveFromArray(data, entryIndex);
}
