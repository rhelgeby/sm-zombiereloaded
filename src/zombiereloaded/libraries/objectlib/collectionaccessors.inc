/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           collectionaccessors.inc
 *  Type:           Library
 *  Description:    Accessor functions for collection objects.
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
 * Adds a cell value at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddCell(Collection:collection, any:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Cell, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, -1, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Inserts a cell value at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddCellAt(Collection:collection, index, any:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Cell, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets the cell value at the specified element index in the collection.
 *
 * @param collection    Collection to get value from.
 * @param index         Index of element.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value at the specified index.
 *
 * @error               Invalid collection, element index or element type
 *                      mismatch.
 */
stock ObjLib_GetCellElementAt(Collection:collection, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate collection.
    ObjLib_ValidateCollection(collection);
    
    new Handle:elements = ObjLib_CollectionGetElements(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Cell, errorHandler))
    {
        return 0;
    }
    
    // Validate index.
    if (!ObjLib_CollectionIndexCheckEx(collection, elements, index, errorHandler))
    {
        return 0;
    }
    
    // Get value.
    return GetArrayCell(elements, index);
}

/*____________________________________________________________________________*/

/**
 * Adds a boolean value at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddBool(Collection:collection, bool:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Bool, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, -1, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Inserts a boolean value at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddBoolAt(Collection:collection, index, bool:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Bool, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets the boolean value at the specified element index.
 *
 * @param collection    Collection to get value from.
 * @param index         Index of element.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value at the specified index.
 *
 * @error               Invalid collection, element index or element type
 *                      mismatch.
 */
stock bool:ObjLib_GetBoolElementAt(Collection:collection, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate collection.
    ObjLib_ValidateCollection(collection);
    
    new Handle:elements = ObjLib_CollectionGetElements(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Bool, errorHandler))
    {
        return false;
    }
    
    // Validate index.
    if (!ObjLib_CollectionIndexCheckEx(collection, elements, index, errorHandler))
    {
        return false;
    }
    
    // Get value.
    return bool:GetArrayCell(elements, index);
}

/*____________________________________________________________________________*/

/**
 * Adds a float value at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddFloat(Collection:collection, Float:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Float, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, -1, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Inserts a float value at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddFloatAt(Collection:collection, index, Float:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Float, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets the float value at the specified element index.
 *
 * @param collection    Collection to get value from.
 * @param index         Index of element.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value at the specified index.
 *
 * @error               Invalid collection, element index or element type
 *                      mismatch.
 */
stock Float:ObjLib_GetFloatElementAt(Collection:collection, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate collection.
    ObjLib_ValidateCollection(collection);
    
    new Handle:elements = ObjLib_CollectionGetElements(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Float, errorHandler))
    {
        return 0.0;
    }
    
    // Validate index.
    if (!ObjLib_CollectionIndexCheckEx(collection, elements, index, errorHandler))
    {
        return 0.0;
    }
    
    // Get value.
    return Float:GetArrayCell(elements, index);
}

/*____________________________________________________________________________*/

/**
 * Adds a handle value at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddHandle(Collection:collection, Handle:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Handle, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, -1, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Inserts a handle value at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddHandleAt(Collection:collection, index, Handle:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Handle, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets the handle value at the specified element index.
 *
 * @param collection    Collection to get value from.
 * @param index         Index of element.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value at the specified index.
 *
 * @error               Invalid collection, element index or element type
 *                      mismatch.
 */
stock Handle:ObjLib_GetHandleElementAt(Collection:collection, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate collection.
    ObjLib_ValidateCollection(collection);
    
    new Handle:elements = ObjLib_CollectionGetElements(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Handle, errorHandler))
    {
        return INVALID_HANDLE;
    }
    
    // Validate index.
    if (!ObjLib_CollectionIndexCheckEx(collection, elements, index, errorHandler))
    {
        return INVALID_HANDLE;
    }
    
    // Get value.
    return Handle:GetArrayCell(elements, index);
}

/*____________________________________________________________________________*/

/**
 * Adds a function value at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddFunction(Collection:collection, Function:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Function, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, -1, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Inserts a function value at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddFunctionAt(Collection:collection, index, Function:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Function, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets the function value at the specified element index.
 *
 * @param collection    Collection to get value from.
 * @param index         Index of element.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value at the specified index.
 *
 * @error               Invalid collection, element index or element type
 *                      mismatch.
 */
stock Function:ObjLib_GetFunctionElementAt(Collection:collection, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate collection.
    ObjLib_ValidateCollection(collection);
    
    new Handle:elements = ObjLib_CollectionGetElements(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Function, errorHandler))
    {
        return INVALID_FUNCTION;
    }
    
    // Validate index.
    if (!ObjLib_CollectionIndexCheckEx(collection, elements, index, errorHandler))
    {
        return INVALID_FUNCTION;
    }
    
    // Get value.
    return Function:GetArrayCell(elements, index);
}

/*____________________________________________________________________________*/

/**
 * Adds an array at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param values        Values to insert.
 * @param size          Size of array.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddArray(Collection:collection, const any:values[], size, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Array, errorHandler))
    {
        return;
    }
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, -1, values, size, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Inserts an array at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param values        Values to insert.
 * @param size          Size of array.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddArrayAt(Collection:collection, index, const any:values[], size, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Array, errorHandler))
    {
        return;
    }
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, index, values, size, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets the array at the specified element index.
 *
 * @param collection    Collection to get value from.
 * @param index         Index of element.
 * @param buffer        Destination buffer.
 * @param maxlen        Size of buffer.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Number of cells copied.
 *
 * @error               Invalid collection, element index or element type
 *                      mismatch.
 */
stock ObjLib_GetArrayElementAt(Collection:collection, index, any:buffer[], maxlen, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate collection.
    ObjLib_ValidateCollection(collection);
    
    new Handle:elements = ObjLib_CollectionGetElements(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Array, errorHandler))
    {
        return 0;
    }
    
    // Validate index.
    if (!ObjLib_CollectionIndexCheckEx(collection, elements, index, errorHandler))
    {
        return 0;
    }
    
    // Get array.
    return GetArrayArray(elements, index, buffer, maxlen);
}

/*____________________________________________________________________________*/

/**
 * Adds a string value at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddString(Collection:collection, const String:value[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    new bool:isString = true;
    new bool:isLookup = false;
    
    // Validate type and lookup case.
    if (!ObjLib_CollectionLookupTypeCheck(collection, ObjDataType_String, isLookup, errorHandler))
    {
        return;
    }
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, -1, value, 1, errorHandler, isLookup, isString);
}

/*____________________________________________________________________________*/

/**
 * Inserts a string value at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddStringAt(Collection:collection, index, const String:value[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    new bool:isString = true;
    new bool:isLookup = false;
    
    // Validate type and lookup case.
    if (!ObjLib_CollectionLookupTypeCheck(collection, ObjDataType_String, isLookup, errorHandler))
    {
        return;
    }
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, index, value, strlen(value), errorHandler, isLookup, isString);
}

/*____________________________________________________________________________*/

/**
 * Gets the string at the specified element index.
 *
 * @param collection    Collection to get value from.
 * @param index         Index of element.
 * @param buffer        Destination buffer.
 * @param maxlen        Size of buffer.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Number of characters copied.
 *
 * @error               Invalid collection, element index or element type
 *                      mismatch.
 */
stock ObjLib_GetStringElementAt(Collection:collection, index, String:buffer[], maxlen, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate collection.
    ObjLib_ValidateCollection(collection);
    
    new Handle:elements = ObjLib_CollectionGetElements(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_String, errorHandler))
    {
        return 0;
    }
    
    // Validate index.
    if (!ObjLib_CollectionIndexCheckEx(collection, elements, index, errorHandler))
    {
        return 0;
    }
    
    // Get array.
    return GetArrayString(elements, index, buffer, maxlen);
}

/*____________________________________________________________________________*/

/**
 * Adds an object reference at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddObject(Collection:collection, Object:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Object, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, -1, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Inserts an object reference at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddObjectAt(Collection:collection, index, Object:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Object, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets the object reference at the specified element index.
 *
 * @param collection    Collection to get value from.
 * @param index         Index of element.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value at the specified index.
 *
 * @error               Invalid collection, element index or element type
 *                      mismatch.
 */
stock Object:ObjLib_GetObjectElementAt(Collection:collection, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate collection.
    ObjLib_ValidateCollection(collection);
    
    new Handle:elements = ObjLib_CollectionGetElements(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Object, errorHandler))
    {
        return INVALID_OBJECT;
    }
    
    // Validate index.
    if (!ObjLib_CollectionIndexCheckEx(collection, elements, index, errorHandler))
    {
        return INVALID_OBJECT;
    }
    
    // Get value.
    return Object:GetArrayCell(elements, index);
}

/*____________________________________________________________________________*/

/**
 * Adds a type descriptor reference at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddObjectType(Collection:collection, ObjectType:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_ObjectType, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, -1, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Inserts a type descriptor reference at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddObjectTypeAt(Collection:collection, index, ObjectType:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_ObjectType, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
    ObjLib_ApplyConstraints(Object:collection, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets the type descriptor reference at the specified element index.
 *
 * @param collection    Collection to get value from.
 * @param index         Index of element.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value at the specified index.
 *
 * @error               Invalid collection, element index or element type
 *                      mismatch.
 */
stock ObjectType:ObjLib_GetObjectTypeElementAt(Collection:collection, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate collection.
    ObjLib_ValidateCollection(collection);
    
    new Handle:elements = ObjLib_CollectionGetElements(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_ObjectType, errorHandler))
    {
        return INVALID_OBJECT_TYPE;
    }
    
    // Validate index.
    if (!ObjLib_CollectionIndexCheckEx(collection, elements, index, errorHandler))
    {
        return INVALID_OBJECT_TYPE;
    }
    
    // Get value.
    return ObjectType:GetArrayCell(elements, index);
}

/*____________________________________________________________________________*/

/**
 * Removes a cell value at the specified index in the collection.
 *
 * @param collection    Collection object to remove from.
 * @param index         Index of value to remove.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_RemoveAt(Collection:collection, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    ObjLib_CollectionRemoveAt(Collection:collection, index);
}

/*____________________________________________________________________________*/

/**
 * Gets the number of elements in the specified collection.
 *
 * @param collection    Collection object.
 *
 * @return              Number of elements.
 *
 * @error               Invalid collection.
 */
stock ObjLib_GetCollectionSize(Collection:collection)
{
    ObjLib_ValidateCollection(collection);
    
    // Get element array.
    new Handle:elements = ObjLib_CollectionGetElements(collection);
    
    return GetArraySize(elements);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Inserts or adds a value/array to a collection using the appropriate public
 * functions.
 *
 * @param collection            The collection object.
 * @param dataType              Data type of value to insert.
 * @param index                 Index to insert at. Use a negative index to add
 *                              at the end of the collection.
 * @param values                Values to insert. Only the first value is used
 *                              for non-array types.
 * @param size                  Number of values.
 * @param errorHandler          Error handler to use for general object errors.
 */
stock ObjLib_CollectionSetValue(Collection:collection, ObjectDataType:dataType, index, const any:values[], size, ObjLib_ErrorHandler:errorHandler)
{
    // Whether to insert or add.
    new bool:insert = (index >= 0);
    
    // Use appropriate add function according to the data type and index.
    switch (dataType)
    {
        case ObjDataType_Any:
        {
            ThrowError("Not implemented!");
        }
        case ObjDataType_Cell:
        {
            if (insert)
            {
                ObjLib_AddCellAt(collection, index, values[0], errorHandler);
            }
            else
            {
                ObjLib_AddCell(collection, values[0], errorHandler);
            }
        }
        case ObjDataType_Bool:
        {
            if (insert)
            {
                ObjLib_AddBoolAt(collection, index, values[0], errorHandler);
            }
            else
            {
                ObjLib_AddBool(collection, values[0], errorHandler);
            }
        }
        case ObjDataType_Float:
        {
            if (insert)
            {
                ObjLib_AddFloatAt(collection, index, values[0], errorHandler);
            }
            else
            {
                ObjLib_AddFloat(collection, values[0], errorHandler);
            }
        }
        case ObjDataType_Handle:
        {
            if (insert)
            {
                ObjLib_AddHandleAt(collection, index, values[0], errorHandler);
            }
            else
            {
                ObjLib_AddHandle(collection, values[0], errorHandler);
            }
        }
        case ObjDataType_Function:
        {
            if (insert)
            {
                ObjLib_AddFunctionAt(collection, index, values[0], errorHandler);
            }
            else
            {
                ObjLib_AddFunction(collection, values[0], errorHandler);
            }
        }
        case ObjDataType_Array:
        {
            if (insert)
            {
                ObjLib_AddArrayAt(collection, index, values, size, errorHandler);
            }
            else
            {
                ObjLib_AddArray(collection, values, size, errorHandler);
            }
        }
        case ObjDataType_String:
        {
            if (insert)
            {
                ObjLib_AddStringAt(collection, index, values, errorHandler);
            }
            else
            {
                ObjLib_AddString(collection, values, errorHandler);
            }
        }
        case ObjDataType_Object:
        {
            if (insert)
            {
                ObjLib_AddObjectAt(collection, index, values[0], errorHandler);
            }
            else
            {
                ObjLib_AddObject(collection, values[0], errorHandler);
            }
        }
        case ObjDataType_ObjectType:
        {
            if (insert)
            {
                ObjLib_AddObjectTypeAt(collection, index, values[0], errorHandler);
            }
            else
            {
                ObjLib_AddObjectType(collection, values[0], errorHandler);
            }
        }
        default:
        {
            // Unexpected type.
            ThrowError("[BUG] Unexpected data type. This is a bug in objectlib.");
        }
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Low level helper. Skips constraints.
 */
stock ObjLib_CollectionInsertCellAt(Collection:collection, index, any:value)
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    new elementCount = GetArraySize(elements);
    
    // Validate index.
    ObjLib_CollectionIndexCheck(elementCount, index);
    
    // Move elements to make free space at the specified index.
    ShiftArrayUp(elements, index);
    
    // Insert value.
    SetArrayCell(elements, index, value);
    
    return 1;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionInsertArrayAt(Collection:collection, index, const any:values[], size)
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    new elementCount = GetArraySize(elements);
    
    // Validate index.
    ObjLib_CollectionIndexCheck(elementCount, index);
    
    // Move elements to make free space at the specified index.
    ShiftArrayUp(elements, index);
    
    // Insert array.
    return SetArrayArray(elements, index, values, size);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionInsertStringAt(Collection:collection, index, const String:value[])
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    new elementCount = GetArraySize(elements);
    
    // Validate index.
    ObjLib_CollectionIndexCheck(elementCount, index);
    
    // Move elements to make free space at the specified index.
    ShiftArrayUp(elements, index);
    
    // Insert string.
    return SetArrayString(elements, index, value);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionAddCell(Collection:collection, any:value)
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    
    // Add value.
    PushArrayCell(elements, value);
    return 1;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionAddArray(Collection:collection, const any:values[], size)
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    
    // Add array.
    return PushArrayArray(elements, values, size);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionAddString(Collection:collection, const String:value[])
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    
    // Add string.
    return PushArrayString(elements, value);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionRemoveAt(Collection:collection, index)
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    new elementCount = GetArraySize(elements);
    
    // Validate index.
    ObjLib_CollectionIndexCheck(elementCount, index);
    
    // Remove element.
    RemoveFromArray(elements, index);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionIndexCheck(elementCount, index)
{
    // Validate index.
    if (!Util_IsInBounds(0, elementCount - 1, index))
    {
        ThrowError("Index out of bounds (%d).", index);
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 */
stock bool:ObjLib_CollectionIndexCheckEx(Collection:collection, Handle:elements, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate index.
    new size = GetArraySize(elements);
    if (index < 0 || index >= size)
    {
        new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
        
        ObjLib_HandleError(typeDescriptor, Object:collection, ObjLibError_InvalidKey, errorHandler, _, _, "Element index is out of bounds: %d", index);
        return false;
    }
    
    return true;
}
