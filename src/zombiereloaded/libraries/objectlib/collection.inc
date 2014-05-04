/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           collection.inc
 *  Type:           Library
 *  Description:    Collection objects. Stores a collection of elements of a
 *                  certain type.
 *
 *  Copyright (C) 2013  Richard Helgeby, Greyscale
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
 * Type descriptor for collection objects. Do not delete this type. You
 * can compare object types against this type to see if it's a collection
 * object by using ObjLib_IsCollectionType).
 */
static ObjectType:ObjLibCollectionType = INVALID_OBJECT_TYPE;
static bool:ObjLibCollectionTypeBuilt = false;

/*____________________________________________________________________________*/

/**
 * Creates a collection object that stores entries of the specified data type.
 *
 * @param dataType          (Optinal) Data type of entries in collection.
 *                          Default is ObjDataType_Object.
 * @param blockSize         Number of cells each element can hold. Only useful
 *                          for string and array types. Default is 1 cell.
 * @param constraints       (Optional) NOT IMPLEMENTED! Element constraints.
 *                          Must be a constraint object. Default is no
 *                          constraints.
 *
 * @return                  Reference to new collection object. Must be deleted
 *                          with ObjLib_DeleteCollection when no longer in use.
 *
 * @error                   
 */
stock Collection:ObjLib_CreateCollection(ObjectDataType:dataType = ObjDataType_Object, blockSize = 1, Object:constraints = INVALID_OBJECT)
{
    // Make sure collection type descriptor is built.
    ObjLib_BuildCollectionType();
    
    // Validate constraints.
    ObjLib_ValidateConstraintOrFail(constraints, dataType, true);
    
    // Create collection object.
    new Object:collection = ObjLib_CreateObject(ObjLibCollectionType, false);
    
    // Create element array.
    new Handle:elements = CreateArray(blockSize);
    
    // Disable container storage handlers and constraints, otherwise the
    // constraint handler thinks we're adding elements to the collection. We're
    // initializing collection attributes now.
    ObjLib_DisableContainerStorage();
    ObjLib_DisableConstraints();
    
    // Initialize collection.
    ObjLib_SetCell(collection, "dataType", dataType);
    ObjLib_SetCell(collection, "blockSize", blockSize);
    ObjLib_SetHandle(collection, "elements", elements);
    ObjLib_SetObject(collection, "constraints", constraints);
    
    // Enable container storage handlers and constraints again. Any attempt to
    // modify this object will now be treated as modifying the element collection.
    ObjLib_EnableContainerStorage();
    ObjLib_EnableConstraints();
    
    return Collection:collection;
}

/*____________________________________________________________________________*/

/**
 * Deletes a collection object and its elements.
 *
 * @param collection        Collection object to delete.
 * @param resetReference    (Optional) Reset object to INVALID_COLLECTION when
 *                          deleted. Default is true.
 *
 * @error                   Invalid collection object.
 */
stock ObjLib_DeleteCollection(Collection:collection, bool:resetReference = true)
{
    // Delete the collection object. Also close handles stored in keys.
    ObjLib_DeleteObject(Object:collection, false, true);
    
    // Reset reference if enabled.
    if (resetReference)
    {
        collection = INVALID_COLLECTION;
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 */
stock ObjLib_BuildCollectionType()
{
    if (!ObjLibCollectionTypeBuilt)
    {
        ObjLibCollectionType = ObjLib_CreateType(
                1,                      // blockSize
                ByteCountToCells(16),   // keySzie
                INVALID_FUNCTION);      // errorHandler
        
        // Data type of elements in collection.
        ObjLib_AddKey(ObjLibCollectionType, "dataType", ObjDataType_Cell);
        
        // Block size. Number of cells reserved for each element.
        ObjLib_AddKey(ObjLibCollectionType, "blockSize", ObjDataType_Cell);
        
        // Handle to array of elements.
        ObjLib_AddKey(ObjLibCollectionType, "elements", ObjDataType_Handle);
        
        // Element constraints (applies to all elements in the collection).
        ObjLib_AddKey(ObjLibCollectionType, "constraints", ObjDataType_Object);
        
        ObjLibCollectionTypeBuilt = true;
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 */
stock ObjLib_ValidateCollection(Collection:collection)
{
    if (collection == INVALID_COLLECTION
        || !ObjLib_TypeOf(Object:collection, ObjLibCollectionType))
    {
        ThrowError("Invalid collection (%x).", collection);
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Validates a data type against a collection's data type.
 *
 * This function will throw an validation error on type mismatch.
 *
 * @param collection        Collection object.
 * @param dataType          Data type to validate.
 * @param errorHandler      Custom error handler. Overrides error handler in
 *                          type descriptor if specified.
 *
 * @return                  True if passed, false otherwise.
 */
stock bool:ObjLib_CollectionTypeCheck(Collection:collection, ObjectDataType:dataType, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Get collection data type.
    new ObjectDataType:collectionType = ObjectDataType:ObjLib_GetCell(Object:collection, "dataType");
    
    // Check for type mismatch.
    if (dataType != collectionType)
    {
        new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
        ObjLib_TypeMismatchError(typeDescriptor, Object:collection, dataType, collectionType, errorHandler);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Validates a data type against a collection's data type at the specified.
 *
 * This function will throw an validation error on type mismatch.
 *
 * @param collection        Collection object.
 * @param dataType          Data type to validate.
 * @param isLookup          (Output) Whether this is a lookup case.
 * @param errorHandler      Custom error handler. Overrides error handler in
 *                          type descriptor if specified.
 *
 * @return                  True if passed type check, false otherwise.
 */
stock bool:ObjLib_CollectionLookupTypeCheck(Collection:collection, ObjectDataType:dataType, &bool:isLookup = false, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Special check for lookup constraints. Get constraints, if any.
    new Object:constraints = ObjLib_GetObject(Object:collection, "constraints");
    if (constraints != INVALID_OBJECT   // Has constraints.
        && !ObjLib_IsInLookupHandler()) // Not already in lookup handler.
    {
        new ObjectType:constraintsType = ObjLib_GetTypeDescriptor(constraints);
        if (constraintsType == ObjLib_LookupConstraints)
        {
            // Set lookup flag for constraint handler. Skip type check.
            isLookup = true;
        }
        return true;
    }
    else
    {
        // Not using lookup constraints or already in lookup handler. Validate
        // type.
        return ObjLib_CollectionTypeCheck(collection, ObjDataType_String, errorHandler);
    }
}

/*____________________________________________________________________________*/

/**
 * Returns whether the specified type is a collection type.
 *
 * @param typeDescriptor        Type descriptor to compare.
 */
stock bool:ObjLib_IsCollectionType(ObjectType:typeDescriptor)
{
    ObjLib_BuildCollectionType();
    return typeDescriptor == ObjLibCollectionType;
}

/*____________________________________________________________________________*/

/**
 * Gets the type descriptor for collection objects.
 *
 * @return                  Type descriptor for collection objects.
 */
stock ObjectType:ObjLib_GetCollectionType()
{
    ObjLib_BuildCollectionType();
    return ObjLibCollectionType;
}

/*____________________________________________________________________________*/

/**
 * Gets the data type of the specified collection.
 *
 * @param collection        Collection object.
 *
 * @return                  Data type.
 */
stock ObjectDataType:ObjLib_CollectionGetDataType(Collection:collection)
{
    return ObjectDataType:ObjLib_GetCell(Object:collection, "dataType");
}

/*____________________________________________________________________________*/

/**
 * Gets the constraints object for the specified collection.
 *
 * @param collection        Collection object.
 *
 * @return                  Constraint object or INVALID_OBJECT if none.
 */
stock Object:ObjLib_CollectionGetConstraints(Collection:collection)
{
    return ObjLib_GetObject(Object:collection, "constraints");
}

/*____________________________________________________________________________*/

/**
 * Gets the element array for the specified collection.
 *
 * @param collection        Collection object.
 *
 * @return                  Element array handle.
 */
stock Handle:ObjLib_CollectionGetElements(Collection:collection)
{
    return ObjLib_GetHandle(Object:collection, "elements");
}
