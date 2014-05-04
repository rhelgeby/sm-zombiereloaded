/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           accessors.inc
 *  Type:           Library
 *  Description:    Accessor functions for objectlib.
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

/*****************************
 *   Object data accessors   *
 *****************************/

/**
 * Gets a value of type "any" from the specified key.
 *
 * @param object        Object to get value from.
 * @param keyName       Name of key. Case sensitive.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock any:ObjLib_GetAny(Object:object, const String:keyName[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return 0;
    }
    
    return ObjLib_GetAnyAt(object, keyIndex, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "any" in the specified key.
 *
 * @param object        Object to update.
 * @param keyName       Name of key. Case sensitive.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetAny(Object:object, const String:keyName[], any:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return;
    }
    
    ObjLib_SetAnyAt(object, keyIndex, value, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "cell" from the specified key.
 *
 * @param object        Object to get value from.
 * @param keyName       Name of key. Case sensitive.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_GetCell(Object:object, const String:keyName[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return 0;
    }
    
    return ObjLib_GetCellAt(object, keyIndex, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "cell" in the specified key.
 *
 * @param object        Object to update.
 * @param keyName       Name of key. Case sensitive.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetCell(Object:object, const String:keyName[], any:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return;
    }
    
    ObjLib_SetCellAt(object, keyIndex, value, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "bool" from the specified key.
 *
 * @param object        Object to get value from.
 * @param keyName       Name of key. Case sensitive.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock bool:ObjLib_GetBool(Object:object, const String:keyName[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return false;
    }
    
    return ObjLib_GetBoolAt(object, keyIndex, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "bool" in the specified key.
 *
 * @param object        Object to update.
 * @param keyName       Name of key. Case sensitive.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetBool(Object:object, const String:keyName[], bool:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return;
    }
    
    ObjLib_SetBoolAt(object, keyIndex, value, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "float" from the specified key.
 *
 * @param object        Object to get value from.
 * @param keyName       Name of key. Case sensitive.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock Float:ObjLib_GetFloat(Object:object, const String:keyName[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return 0.0;
    }
    
    return ObjLib_GetFloatAt(object, keyIndex, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "float" in the specified key.
 *
 * @param object        Object to update.
 * @param keyName       Name of key. Case sensitive.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetFloat(Object:object, const String:keyName[], Float:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return;
    }
    
    ObjLib_SetFloatAt(object, keyIndex, value, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "handle" from the specified key.
 *
 * @param object        Object to get value from.
 * @param keyName       Name of key. Case sensitive.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock Handle:ObjLib_GetHandle(Object:object, const String:keyName[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return INVALID_HANDLE;
    }
    
    return ObjLib_GetHandleAt(object, keyIndex, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "handle" in the specified key.
 *
 * @param object        Object to update.
 * @param keyName       Name of key. Case sensitive.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetHandle(Object:object, const String:keyName[], Handle:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return;
    }
    
    ObjLib_SetHandleAt(object, keyIndex, value, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "function" from the specified key.
 *
 * @param object        Object to get value from.
 * @param keyName       Name of key. Case sensitive.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock Function:ObjLib_GetFunction(Object:object, const String:keyName[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return INVALID_FUNCTION;
    }
    
    return ObjLib_GetFunctionAt(object, keyIndex, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "function" in the specified key.
 *
 * @param object        Object to update.
 * @param keyName       Name of key. Case sensitive.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetFunction(Object:object, const String:keyName[], Function:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return;
    }
    
    ObjLib_SetFunctionAt(object, keyIndex, value, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "array" in the specified key.
 *
 * @param object        Object to get value from.
 * @param keyName       Name of key. Case sensitive.
 * @param buffer        Destination buffer.
 * @param maxlen        Size of buffer.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Number of cells copied.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_GetArray(Object:object, const String:keyName[], any:buffer[], maxlen, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return 0;
    }
    
    return ObjLib_GetArrayAt(object, keyIndex, buffer, maxlen, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "array" in the specified key.
 *
 * @param object        Object to update.
 * @param keyName       Name of key. Case sensitive.
 * @param values        Array to set.
 * @param maxlen        Size of array.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Number of cells copied.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetArray(Object:object, const String:keyName[], const any:values[], maxlen, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return 0;
    }
    
    return ObjLib_SetArrayAt(object, keyIndex, values, maxlen, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "string" in the specified key.
 *
 * @param object        Object to get value from.
 * @param keyName       Name of key. Case sensitive.
 * @param buffer        Destination buffer.
 * @param maxlen        Size of buffer.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Number of characters copied.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_GetString(Object:object, const String:keyName[], String:buffer[], maxlen, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return 0;
    }
    
    return ObjLib_GetStringAt(object, keyIndex, buffer, maxlen, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "string" in the specified key.
 *
 * @param object        Object to update.
 * @param keyName       Name of key. Case sensitive.
 * @param value         String to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Number of characters copied.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetString(Object:object, const String:keyName[], const String:value[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return 0;
    }
    
    return ObjLib_SetStringAt(object, keyIndex, value, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "object" from the specified key.
 *
 * @param object        Object to get value from.
 * @param keyName       Name of key. Case sensitive.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock Object:ObjLib_GetObject(Object:object, const String:keyName[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return INVALID_OBJECT;
    }
    
    return ObjLib_GetObjectAt(object, keyIndex, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "object" in the specified key.
 *
 * @param object        Object to update.
 * @param keyName       Name of key. Case sensitive.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetObject(Object:object, const String:keyName[], Object:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return;
    }
    
    ObjLib_SetObjectAt(object, keyIndex, value, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "object type" from the specified key.
 *
 * @param object        Object to get value from.
 * @param keyName       Name of key. Case sensitive.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjectType:ObjLib_GetObjectType(Object:object, const String:keyName[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return INVALID_OBJECT_TYPE;
    }
    
    return ObjLib_GetObjectTypeAt(object, keyIndex, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "object type" in the specified key.
 *
 * @param object        Object to update.
 * @param keyName       Name of key. Case sensitive.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetObjectType(Object:object, const String:keyName[], ObjectType:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName, errorHandler);
    if (keyIndex < 0)
    {
        // Error was handled.
        return;
    }
    
    ObjLib_SetObjectTypeAt(object, keyIndex, value, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "any" at the specified key index.
 *
 * @param object        Object to get value from.
 * @param index         Index of key.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock any:ObjLib_GetAnyAt(Object:object, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Any, errorHandler))
    {
        return 0;
    }
    
    // Null check.
    if (!ObjLib_NullCheck(object, index, errorHandler))
    {
        return 0;
    }
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return any:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "any" at the specified key index.
 *
 * @param object        Object to update.
 * @param index         Index of key.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_SetAnyAt(Object:object, index, any:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Any, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    ObjLib_ApplyConstraints(object, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "cell" at the specified key index.
 *
 * @param object        Object to get value from.
 * @param index         Index of key.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_GetCellAt(Object:object, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Cell, errorHandler))
    {
        return 0;
    }
    
    // Null check.
    if (!ObjLib_NullCheck(object, index, errorHandler))
    {
        return 0;
    }
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "cell" at the specified key index.
 *
 * @param object        Object to update.
 * @param index         Index of key.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_SetCellAt(Object:object, index, value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Cell, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    ObjLib_ApplyConstraints(object, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "bool" at the specified key index.
 *
 * @param object        Object to get value from.
 * @param index         Index of key.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock bool:ObjLib_GetBoolAt(Object:object, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Bool, errorHandler))
    {
        return false;
    }
    
    // Null check.
    if (!ObjLib_NullCheck(object, index, errorHandler))
    {
        return false;
    }
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return bool:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "bool" at the specified key index.
 *
 * @param object        Object to update.
 * @param index         Index of key.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_SetBoolAt(Object:object, index, bool:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Bool, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    ObjLib_ApplyConstraints(object, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "float" at the specified key index.
 *
 * @param object        Object to get value from.
 * @param index         Index of key.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock Float:ObjLib_GetFloatAt(Object:object, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Float, errorHandler))
    {
        return 0.0;
    }
    
    // Null check.
    if (!ObjLib_NullCheck(object, index, errorHandler))
    {
        return 0.0;
    }
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return Float:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "float" at the specified key index.
 *
 * @param object        Object to update.
 * @param index         Index of key.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_SetFloatAt(Object:object, index, Float:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Float, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    ObjLib_ApplyConstraints(object, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "handle" at the specified key index.
 *
 * @param object        Object to get value from.
 * @param index         Index of key.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock Handle:ObjLib_GetHandleAt(Object:object, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Handle, errorHandler))
    {
        return INVALID_HANDLE;
    }
    
    // Null check.
    if (!ObjLib_NullCheck(object, index, errorHandler))
    {
        return INVALID_HANDLE;
    }
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return Handle:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "handle" at the specified key index.
 *
 * @param object        Object to update.
 * @param index         Index of key.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_SetHandleAt(Object:object, index, Handle:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Handle, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    ObjLib_ApplyConstraints(object, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "function" at the specified key index.
 *
 * @param object        Object to get value from.
 * @param index         Index of key.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock Function:ObjLib_GetFunctionAt(Object:object, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Function, errorHandler))
    {
        return INVALID_FUNCTION;
    }
    
    // Null check.
    if (!ObjLib_NullCheck(object, index, errorHandler))
    {
        return INVALID_FUNCTION;
    }
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return Function:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "function" at the specified key index.
 *
 * @param object        Object to update.
 * @param index         Index of key.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_SetFunctionAt(Object:object, index, Function:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Function, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    ObjLib_ApplyConstraints(object, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "array" at the specified key index.
 *
 * @param object        Object to get value from.
 * @param index         Index of key.
 * @param buffer        Destination buffer.
 * @param maxlen        Size of buffer.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Number of cells copied.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_GetArrayAt(Object:object, index, any:buffer[], maxlen, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Array, errorHandler))
    {
        return 0;
    }
    
    // Null check.
    if (!ObjLib_NullCheck(object, index, errorHandler))
    {
        return 0;
    }
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return GetArrayArray(data, index, buffer, maxlen);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "array" at the specified key index.
 *
 * @param object        Object to update.
 * @param index         Index of key.
 * @param values        Array to set.
 * @param size          Size of array.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Number of cells copied, 0 on constraint violation.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_SetArrayAt(Object:object, index, const any:values[], size, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate size.
    if (size <= 0)
    {
        ThrowError("Cannot insert an empty array.");
    }
    
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Array, errorHandler))
    {
        return 0;
    }
    
    // Handle constraints, sets value if valid and not overridden.
    return ObjLib_ApplyConstraints(object, typeDescriptor, index, values, size, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "string" at the specified key index.
 *
 * @param object        Object to get value from.
 * @param index         Index of key.
 * @param buffer        Destination buffer.
 * @param maxlen        Size of buffer.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Number of characters copied.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_GetStringAt(Object:object, index, String:buffer[], maxlen, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_String, errorHandler))
    {
        return 0;
    }
    
    // Null check.
    if (!ObjLib_NullCheck(object, index, errorHandler))
    {
        return 0;
    }
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return GetArrayString(data, index, buffer, maxlen);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "string" at the specified key index.
 *
 * @param object        Object to update.
 * @param index         Index of key.
 * @param value         String to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Number of characters copied, 0 on constraint violation,
 *                      or -1 if value was overridden.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_SetStringAt(Object:object, index, const String:value[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    new bool:isLookup = false;
    new bool:isString = true;
    
    // Special check for lookup constraints. Get constraints, if any.
    new Object:constraints = ObjLib_GetTypeConstraintsAt(typeDescriptor, index);
    if (constraints != INVALID_OBJECT   // Has constraints.
        && !ObjLib_IsInLookupHandler()) // Not already in lookup handler.
    {
        new ObjectType:constraintsType = ObjLib_GetTypeDescriptor(constraints);
        if (constraintsType == ObjLib_LookupConstraints)
        {
            // Set lookup flag for constraint handler. Skip type check.
            isLookup = true;
        }
    }
    else
    {
        // Not using lookup constraints or already in lookup handler. Validate
        // type.
        if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_String, errorHandler))
        {
            return 0;
        }
    }
    
    // Handle constraints, sets value if valid and not overridden.
    return ObjLib_ApplyConstraints(object, typeDescriptor, index, value, strlen(value), errorHandler, isLookup, isString);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "object" at the specified key index.
 *
 * @param object        Object to get value from.
 * @param index         Index of key.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock Object:ObjLib_GetObjectAt(Object:object, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Object, errorHandler))
    {
        return INVALID_OBJECT;
    }
    
    // Null check.
    if (!ObjLib_NullCheck(object, index, errorHandler))
    {
        return INVALID_OBJECT;
    }
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return Object:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "object" at the specified key index.
 *
 * @param object        Object to update.
 * @param index         Index of key.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_SetObjectAt(Object:object, index, Object:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_Object, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    ObjLib_ApplyConstraints(object, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "object type" at the specified key index.
 *
 * @param object        Object to get value from.
 * @param index         Index of key.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @return              Value in the specified key.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjectType:ObjLib_GetObjectTypeAt(Object:object, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_ObjectType, errorHandler))
    {
        return INVALID_OBJECT_TYPE;
    }
    
    // Null check.
    if (!ObjLib_NullCheck(object, index, errorHandler))
    {
        return INVALID_OBJECT_TYPE;
    }
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return ObjectType:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "object type" at the specified key index.
 *
 * @param object        Object to update.
 * @param index         Index of key.
 * @param value         Value to set.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid object, key index or key type mismatch.
 */
stock ObjLib_SetObjectTypeAt(Object:object, index, ObjectType:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    if (!ObjLib_KeyTypeCheck(typeDescriptor, object, index, ObjDataType_ObjectType, errorHandler))
    {
        return;
    }
    
    // Copy the value into an array so it can be passed directly to the general
    // constraint handler.
    new any:values[1];
    values[0] = value;
    
    // Handle constraints, sets value if valid and not overridden.
    ObjLib_ApplyConstraints(object, typeDescriptor, index, values, 1, errorHandler);
}

/*____________________________________________________________________________*/

/***************************
 *   Meta data accessors   *
 ***************************/

/**
 * Returns whether the specified object is mutable.
 *
 * @return      True if mutable, false otherwise.
 */
stock bool:ObjLib_IsMutable(Object:object)
{
    return ObjLib_IsTypeMutable(ObjLib_GetTypeDescriptor(object));
}

/*____________________________________________________________________________*/

/**
 * Gets an object's type descriptor.
 *
 * @param object    Object to get type from.
 *
 * @return          Object type descriptor reference.
 */
stock ObjectType:ObjLib_GetTypeDescriptor(Object:object)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    return ObjectType:GetArrayCell(Handle:object, _:Object_MetaData);
}

/*____________________________________________________________________________*/

/**
 * Gets the index for the specified key in an object.
 *
 * Warning: When a key is removed, indexes for existing keys below the removed
 *          key will be shifted up. If you cache an index and later remove a key
 *          you need to cache the index again.
 *
 * @param object    Object to get index from.
 * @param keyName   Name of key. Case sensitive.
 *
 * @return          Key index if successful, or -1 if it doesn't exist.
 */
stock ObjLib_GetKeyIndex(Object:object, const String:keyName[])
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    return ObjLib_GetKeyIndexFromType(typeDescriptor, keyName);
}

/*____________________________________________________________________________*/

/**
 * Gets the index for the specified key in an object.
 *
 * Warning: When a key is removed, indexes for existing keys below the removed
 *          key will be shifted up. If you cache an index and later remove a key
 *          you need to cache the index again.
 *
 * @param object    Object to get index from.
 * @param keyName   Name of key. Case sensitive.
 *
 * @return          Key index if successful, or -1 if it doesn't exist.
 */
stock ObjLib_GetKeyIndexFromType(ObjectType:typeDescriptor, const String:keyName[])
{
    // Validate type.
    ObjLib_ValidateObjectType(typeDescriptor);
    
    new Handle:nameIndex = ObjLib_GetTypeNameIndex(typeDescriptor);
    
    new index = -1;
    if (GetTrieValue(nameIndex, keyName, index))
    {
        // Key found.
        return index;
    }
    
    // Key not found.
    return -1;
}

/*____________________________________________________________________________*/

/**
 * Returns whether a key is null at the specified key index.
 *
 * @param object    Object to inspect.
 * @param keyIndex  Key index.
 *
 * @return          True if null, false otherwise.
 */
stock bool:ObjLib_IsKeyNullAt(Object:object, keyIndex)
{
    new Handle:nullKey = ObjLib_GetObjectNullKey(object);
    return bool:GetArrayCell(nullKey, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Returns whether a key is null at the specified key index.
 *
 * @param object    Object to inspect.
 * @param keyName   Key name.
 *
 * @return          True if null, false otherwise.
 */
stock bool:ObjLib_IsKeyNull(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_IsKeyNullAt(object, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Gets the list of key names in an object type descriptor.
 *
 * @param typeDescriptor    Object type to inspect.
 *
 * @return      Handle to ADT array with key name strings. Do not close this
 *              handle. Use ObjLib_DeleteType instead.
 */
stock Handle:ObjLib_GetTypeKeys(ObjectType:typeDescriptor)
{
    return Handle:GetArrayCell(Handle:typeDescriptor, _:ObjectType_Keys);
}

/*____________________________________________________________________________*/

/**
 * Gets the list of data types for each key in an object type descriptor.
 *
 * @param typeDescriptor    Type descriptor to inspect.
 *
 * @return      Handle to ADT array of ObjectDataType entries.
 */
stock Handle:ObjLib_GetTypeDataTypes(ObjectType:typeDescriptor)
{
    return Handle:GetArrayCell(Handle:typeDescriptor, _:ObjectType_DataTypes);
}

/*____________________________________________________________________________*/

/**
 * Gets the data type of the specified key.
 *
 * @param typeDescriptor    Type descriptor to inspect.
 * @param keyIndex          Index of key.
 *
 * @return                  Type of key.
 */
stock ObjectDataType:ObjLib_GetKeyDataType(ObjectType:typeDescriptor, keyIndex)
{
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    return ObjectDataType:GetArrayCell(dataTypes, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Gets the constraints object for the specified key.
 *
 * @param typeDescriptor    Type descriptor.
 * @param keyIndex          Index of key to get constraints from.
 *
 * @return                  Constraints object or INVALID_OBJECT if no
 *                          constraints.
 */
stock Object:ObjLib_GetTypeConstraintsAt(ObjectType:typeDescriptor, keyIndex)
{
    // Get constraints object list.
    new Handle:constraintList = ObjLib_GetTypeConstraints(typeDescriptor);
    return Object:GetArrayCell(constraintList, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Sets the constraints object for the specified key.
 *
 * Warning: Do not set a new constraint object before deleting any existing
 *          constraint object at this index first. Otherwise there will be
 *          a memory leak.
 *          
 *          Existing constraint object is automatically deleted if
 *          deleteOldConstraints is true.
 *
 * @param typeDescriptor        Type descriptor.
 * @param keyIndex              Index of key to get constraints from.
 * @param constraints           Constraints object to set.
 * @param deleteOldConstraints  Whether to delete existing constraints object,
 *                              if any. Default is true.
 */
stock Object:ObjLib_SetTypeConstraintsAt(ObjectType:typeDescriptor, keyIndex, Object:constraints, bool:deleteOldConstraints = true)
{
    // Get key data type.
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    new ObjectDataType:dataType = ObjectDataType:GetArrayCell(dataTypes, keyIndex);
    
    // Validate.
    ObjLib_ValidateConstraintOrFail(constraints, dataType);
    
    // Get constraints object list.
    new Handle:constraintList = ObjLib_GetTypeConstraints(typeDescriptor);
    
    // Delete old constraints if enabled.
    if (deleteOldConstraints)
    {
        new Object:oldConstraints = Object:GetArrayCell(constraintList, keyIndex);
        if (oldConstraints != INVALID_OBJECT)
        {
            ObjLib_DeleteObject(oldConstraints);
        }
    }
    
    // Set new constraints.
    SetArrayCell(constraintList, keyIndex, constraints);
}

/*____________________________________________________________________________*/

/**
 * Returns whether the specified object type descriptor is mutable (not locked).
 *
 * @param typeDescriptor    Object type to inspect.
 *
 * @return                  True if mutable, false otherwise.
 */
stock bool:ObjLib_IsTypeMutable(ObjectType:typeDescriptor)
{
    return GetArrayCell(Handle:typeDescriptor, _:ObjectType_Locked) == 0;
}

/*____________________________________________________________________________*/

/**
 * Returns the name of a data type.
 *
 * @param dataType  Data type value.
 * @param buffer    Name buffer.
 * @param maxlen    Size of buffer.
 *
 * @return          Number of cells written.
 */
stock ObjLib_DataTypeToString(ObjectDataType:dataType, String:buffer[], maxlen)
{
    switch (dataType)
    {
        case ObjDataType_Any:           return strcopy(buffer, maxlen, "Any");
        case ObjDataType_Cell:          return strcopy(buffer, maxlen, "Cell");
        case ObjDataType_Bool:          return strcopy(buffer, maxlen, "Bool");
        case ObjDataType_Float:         return strcopy(buffer, maxlen, "Float");
        case ObjDataType_Handle:        return strcopy(buffer, maxlen, "Handle");
        case ObjDataType_Function:      return strcopy(buffer, maxlen, "Function");
        case ObjDataType_Array:         return strcopy(buffer, maxlen, "Array");
        case ObjDataType_String:        return strcopy(buffer, maxlen, "String");
        case ObjDataType_Object:        return strcopy(buffer, maxlen, "Object");
        case ObjDataType_ObjectType:    return strcopy(buffer, maxlen, "ObjectType");
        default:                        return strcopy(buffer, maxlen, "(Unefined)");
    }
    
    return 0;
}

/*____________________________________________________________________________*/

/**
 * Returns the number of keys declared in the specified type descriptor.
 *
 * @param typeDescriptor    Object type to inspect.
 *
 * @return                  Number of keys.
 */
stock ObjLib_GetNumKeys(ObjectType:typeDescriptor)
{
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    return GetArraySize(keys);
}


/******************************************************************************
 *                             INTERNAL USE ONLY                              *
 ******************************************************************************/

/** Internal use only! */
stock ObjLib_SetTypeLocked(ObjectType:typeDescriptor, bool:locked)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_Locked, locked);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Object:ObjLib_GetTypeParentObject(ObjectType:typeDescriptor)
{
    return Object:GetArrayCell(Handle:typeDescriptor, _:ObjectType_ParentObject);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeParentObject(ObjectType:typeDescriptor, Object:parent)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_ParentObject, parent);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_GetTypeKeySize(ObjectType:typeDescriptor)
{
    return GetArrayCell(Handle:typeDescriptor, _:ObjectType_KeySize);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeKeySize(ObjectType:typeDescriptor, keySize)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_KeySize, keySize);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_GetTypeBlockSize(ObjectType:typeDescriptor)
{
    return GetArrayCell(Handle:typeDescriptor, _:ObjectType_BlockSize);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeBlockSize(ObjectType:typeDescriptor, blockSize)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_BlockSize, blockSize);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeKeys(ObjectType:typeDescriptor, Handle:keys)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_Keys, keys);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_GetTypeNameIndex(ObjectType:typeDescriptor)
{
    return Handle:GetArrayCell(Handle:typeDescriptor, _:ObjectType_NameIndex);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeNameIndex(ObjectType:typeDescriptor, Handle:nameIndex)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_NameIndex, nameIndex);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeDataTypes(ObjectType:typeDescriptor, Handle:dataTypes)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_DataTypes, dataTypes);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_GetTypeConstraints(ObjectType:typeDescriptor)
{
    return Handle:GetArrayCell(Handle:typeDescriptor, _:ObjectType_Constraints);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeConstraints(ObjectType:typeDescriptor, Handle:constraints)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_Constraints, constraints);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_ErrorHandler:ObjLib_GetTypeErrorHandler(ObjectType:typeDescriptor)
{
    return ObjLib_ErrorHandler:GetArrayCell(Handle:typeDescriptor, _:ObjectType_ErrorHandler);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeErrorHandler(ObjectType:typeDescriptor, ObjLib_ErrorHandler:errorHandler)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_ErrorHandler, errorHandler);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_SetTypeDescriptor(Object:object, ObjectType:typeDescriptor)
{
    SetArrayCell(Handle:object, _:Object_MetaData, typeDescriptor);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_GetObjectData(Object:object)
{
    return Handle:GetArrayCell(Handle:object, _:Object_Data);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_SetObjectData(Object:object, Handle:data)
{
    SetArrayCell(Handle:object, _:Object_Data, data);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_GetObjectNullKey(Object:object)
{
    return Handle:GetArrayCell(Handle:object, _:Object_NullKey);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_SetObjectNullKey(Object:object, Handle:data)
{
    SetArrayCell(Handle:object, _:Object_NullKey, data);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetKeyNull(Object:object, keyIndex, bool:null)
{
    new Handle:nullKey = ObjLib_GetObjectNullKey(object);
    SetArrayCell(nullKey, keyIndex, null);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Sets a value in an object using the appropriate public functions.
 *
 * @param object                The object.
 * @param dataType              Data type of value to set.
 * @param index                 Key index.
 * @param values                Values to insert. Only the first value is used
 *                              for non-array types.
 * @param size                  Number of values.
 * @param errorHandler          Error handler to use for general object errors.
 */
stock ObjLib_SetValue(Object:object, ObjectDataType:dataType, keyIndex, const any:values[], size, ObjLib_ErrorHandler:errorHandler)
{
    switch (dataType)
    {
        case ObjDataType_Any:
        {
            ObjLib_SetAnyAt(object, keyIndex, values[0], errorHandler);
        }
        case ObjDataType_Cell:
        {
            ObjLib_SetCellAt(object, keyIndex, values[0], errorHandler);
        }
        case ObjDataType_Bool:
        {
            ObjLib_SetBoolAt(object, keyIndex, values[0], errorHandler);
        }
        case ObjDataType_Float:
        {
            ObjLib_SetFloatAt(object, keyIndex, values[0], errorHandler);
        }
        case ObjDataType_Handle:
        {
            ObjLib_SetHandleAt(object, keyIndex, values[0], errorHandler);
        }
        case ObjDataType_Function:
        {
            ObjLib_SetFunctionAt(object, keyIndex, values[0], errorHandler);
        }
        case ObjDataType_Array:
        {
            ObjLib_SetArrayAt(object, keyIndex, values, size, errorHandler);
        }
        case ObjDataType_String:
        {
            ObjLib_SetStringAt(object, keyIndex, values, errorHandler);
        }
        case ObjDataType_Object:
        {
            ObjLib_SetObjectAt(object, keyIndex, values[0], errorHandler);
        }
        case ObjDataType_ObjectType:
        {
            ObjLib_SetObjectTypeAt(object, keyIndex, values[0], errorHandler);
        }
        default:
        {
            // Unexpected type.
            ThrowError("[BUG] Unexpected data type. This is a bug in objectlib.");
        }
    }
}
