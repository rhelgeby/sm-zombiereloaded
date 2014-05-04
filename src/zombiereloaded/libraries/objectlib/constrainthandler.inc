/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           constrainthandler.inc
 *  Type:           Library
 *  Description:    Handles validation constraints for objectlib.
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
 * @section Flags for storing whether a special constraint handler is working.
 *          This is used to prevent re-entry in these constraint handlers if the
 *          handler itself override the value stored in objects.
 *
 *          This only applies to handlers that may override values or have user
 *          callbacks.
 *
 *          Use ObjLib_IsInCustomHandler and similar to read these values.
 */
static bool:ObjLib_InCustomHandler = false;
static bool:ObjLib_InLookupHandler = false;
/**
 * @endsection
 */

/**
 * Switch for overriding whether constraints are checked. If disabled, values
 * are stored directly in the object.
 */
static bool:ObjLib_ConstraintsEnabled = true;

/**
 * Switch for overriding how values are stored in container objects (like
 * collections).
 *
 * When disabled, values are stored directly in the container object instead of
 * using the container's storage handler. This is necessary to be able to set
 * and modify attributes of the container object.
 */
static bool:ObjLib_ContainerStorageEnabled = true;

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Entry point for constraint handler. Accessor functions will use this instead
 * of storing values themself.
 *
 * This function will call appropriate constraint handlers and set values if
 * valid and not overridden.
 *
 * @param object            The object or collection being validated.
 * @param objectType        The object's type descriptor.
 * @param index             Key or element index. Use a negative index to append
 *                          a value to a collection object.
 * @param values            Values to store. Store single values at index 0.
 * @param size              Number of values.
 * @param errorHandler      Error handler to use if a value is invalid.
 * @param isString          (Optional) Whether the value should be treated as a
 *                          string. Default is false.
 *
 * @return                  Number of cells copied if isString is false.
 *                          Number of characters copied if isString is true.
 *                          -1 on constraint violation.
 *                          -2 if value vas overridden.
 */
stock ObjLib_ApplyConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        index,
        const any:values[],
        size,
        ObjLib_ErrorHandler:errorHandler,
        bool:isLookup = false,
        bool:isString = false)
{
    new ObjectConstraintResult:constraintResult;
    
    if (ObjLib_ConstraintsEnabled)
    {
        // Get constraint object.
        new Object:constraints = ObjLib_GetConstraints(object, typeDescriptor, index);
        constraintResult = ObjLib_RunConstraintHandler(constraints, object, typeDescriptor, index, values, size, errorHandler, isLookup);
    }
    else
    {
        // Constraints are disabled. Mark value as valid.
        constraintResult = ObjConstraintResult_Valid;
    }
    
    switch (constraintResult)
    {
        case ObjConstraintResult_Invalid:
        {
            // Error is already handled by a constraint handler. Forward result.
            return -1;
        }
        case ObjConstraintResult_Valid:
        {
            if (size > 1)
            {
                if (isString)
                {
                    // Set string.
                    return ObjLib_ApplyString(object, typeDescriptor, index, values);
                }
                else
                {
                    // Set array.
                    return ObjLib_ApplyArray(object, typeDescriptor, index, values, size);
                }
            }
            else
            {
                // Set value.
                ObjLib_ApplyValue(object, typeDescriptor, index, values[0]);
                return 1;
            }
        }
        case ObjConstraintResult_Overridden:
        {
            return -2;
        }
        default:
        {
            ThrowError("[BUG] Unexpected constraint result. This is a bug in objectlib.");
        }
    }
    
    // Error, it should never reach this code. Return something unspecified to
    // satisfy compiler.
    return -3;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Sets the value in an object or collection (determined by object type). This
 * may be expanded to support future types.
 *
 * @param object            Object or collection to store value in.
 * @param typeDescriptor    Object's type descriptor.
 * @param index             Key or element index. Use a negative index to append
 *                          a value to a collection object.
 * @param value             Value to store.
 *
 * @return                  Number of cells copied: always 1.
 */
stock ObjLib_ApplyValue(
        Object:object,
        ObjectType:typeDescriptor,
        index,
        any:value)
{

    // TODO: Move storage code to its own storage handlers in collection.inc and
    //       object.inc). This function should only be responsible for calling
    //       the appropriate storage handler.
    
    if (ObjLib_IsCollectionType(typeDescriptor) && ObjLib_ContainerStorageEnabled)
    {
        // Object is a collection.
        
        // Add or insert value in collection.
        if (index < 0)
        {
            // Append to collection.
            ObjLib_CollectionAddCell(Collection:object, value);
        }
        else
        {
            // Insert in collection.
            ObjLib_CollectionInsertCellAt(Collection:object, index, value);
        }
    }
    else
    {
        // Object is a general object. Set value.
        new Handle:data = ObjLib_GetObjectData(object);
        SetArrayCell(data, index, value);
        ObjLib_SetKeyNull(object, index, false);
    }
    
    // One cell copied.
    return 1;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Sets the array in an object or collection (determined by object type). This
 * may be expanded to support future types.
 *
 * @param object            Object or collection to store value in.
 * @param typeDescriptor    Object's type descriptor.
 * @param index             Key or element index. Use a negative index to append
 *                          a value to a collection object.
 * @param values            Values to store.
 * @param size              Number of values.
 *
 * @return                  Number of cells copied.
 */
stock ObjLib_ApplyArray(
        Object:object,
        ObjectType:typeDescriptor,
        index,
        const any:values[],
        size)
{
    new count = 0;
    
    if (ObjLib_IsCollectionType(typeDescriptor) && ObjLib_ContainerStorageEnabled)
    {
        // Object is a collection. Get colletion data type.
        
        // Add or insert value in collection.
        if (index < 0)
        {
            // Append to collection.
            count = ObjLib_CollectionAddArray(Collection:object, values, size);
        }
        else
        {
            // Insert in collection.
            count = ObjLib_CollectionInsertArrayAt(Collection:object, index, values, size);
        }
    }
    else
    {
        // Object is a general object. Set value.
        new Handle:data = ObjLib_GetObjectData(object);
        count = SetArrayArray(data, index, values, size);
        ObjLib_SetKeyNull(object, index, false);
    }
    
    return count;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Sets the string in an object or collection (determined by object type). This
 * may be expanded to support future types.
 *
 * @param object            Object or collection to store value in.
 * @param typeDescriptor    Object's type descriptor.
 * @param index             Key or element index. Use a negative index to append
 *                          a value to a collection object.
 * @param value             String to store.
 *
 * @return                  Number of characters copied.
 */
stock ObjLib_ApplyString(
        Object:object,
        ObjectType:typeDescriptor,
        index,
        const String:value[])
{
    new count = 0;
    
    if (ObjLib_IsCollectionType(typeDescriptor) && ObjLib_ContainerStorageEnabled)
    {
        // Object is a collection. Get colletion data type.
        
        // Add or insert value in collection.
        if (index < 0)
        {
            // Append to collection.
            count = ObjLib_CollectionAddString(Collection:object, value);
        }
        else
        {
            // Insert in collection.
            count = ObjLib_CollectionInsertStringAt(Collection:object, index, value);
        }
    }
    else
    {
        // Object is a general object. Set value.
        new Handle:data = ObjLib_GetObjectData(object);
        count = SetArrayString(data, index, value);
        ObjLib_SetKeyNull(object, index, false);
    }
    
    return count;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Calls the constraint handler matching the key's data type.
 *
 * @return              True if validation passed constraints.
 */
stock ObjectConstraintResult:ObjLib_RunConstraintHandler(
        Object:constraints,
        Object:object,
        ObjectType:typeDescriptor,
        keyIndex,
        const any:values[],
        size,
        ObjLib_ErrorHandler:customErrorHandler = INVALID_FUNCTION,
        bool:isLookup = false)
{
    // Check if no constraints.
    if (constraints == INVALID_OBJECT)
    {
        // Value is valid, no constraints.
        return ObjConstraintResult_Valid;
    }
    
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_GetErrorHandler(typeDescriptor, customErrorHandler);
    
    // Delegate work to a handler that matches the data type.
    new ObjectType:constraintsType = ObjLib_GetTypeDescriptor(constraints);
    if (constraintsType == ObjLib_CustomConstraints)
    {
        return ObjLib_HandleCustomConstraints(object, typeDescriptor, constraints, keyIndex, values, size, errorHandler);
    }
    else if (constraintsType == ObjLib_LookupConstraints)
    {
        // Check if this is a special lookup case.
        if (isLookup)
        {
            // Call lookup constraint handler.
            return ObjLib_HandleLookupConstraints(object, typeDescriptor, constraints, keyIndex, values, errorHandler);
        }
        else
        {
            // Check if sub constraints are available.
            new Object:subConstraints = ObjLib_GetObject(constraints, "subConstraints");
            if (subConstraints != INVALID_OBJECT)
            {
                // Recursive re-entry for handling raw data constraints. (Recursion
                // is stopped by preventing lookup contraint objects in
                // subConstraints when building the constraint object.)
                return ObjLib_RunConstraintHandler(subConstraints, object, typeDescriptor, keyIndex, values, size, customErrorHandler);
            }
            else
            {
                // No sub constraints.
                return ObjConstraintResult_Valid;
            }
        }
    }
    else if (constraintsType == ObjLib_CellConstraints)
    {
        return ObjLib_HandleCellConstraints(object, typeDescriptor, constraints, keyIndex, values[0], errorHandler);
    }
    else if (constraintsType == ObjLib_FloatConstraints)
    {
        return ObjLib_HandleFloatConstraints(object, typeDescriptor, constraints, keyIndex, values[0], errorHandler);
    }
    else if (constraintsType == ObjLib_HandleConstraints)
    {
        return ObjLib_HandleHandleConstraints(object, typeDescriptor, constraints, keyIndex, values[0], errorHandler);
    }
    else if (constraintsType == ObjLib_FunctionConstraints)
    {
        return ObjLib_HandleFunctionConstraints(object, typeDescriptor, constraints, keyIndex, values[0], errorHandler);
    }
    else if (constraintsType == ObjLib_ArrayConstraints)
    {
        return ObjLib_HandleArrayConstraints(object, typeDescriptor, constraints, keyIndex, values, size, errorHandler);
    }
    else if (constraintsType == ObjLib_CollectionConstraints)
    {
        return ObjLib_HandleCollectionConstraints(object, typeDescriptor, constraints, keyIndex, values, size, errorHandler);
    }
    else if (constraintsType == ObjLib_StringConstraints)
    {
        return ObjLib_HandleStringConstraints(object, typeDescriptor, constraints, keyIndex, values, errorHandler);
    }
    else if (constraintsType == ObjLib_ObjectConstraints)
    {
        return ObjLib_HandleObjectConstraints(object, typeDescriptor, constraints, keyIndex, values[0], errorHandler);
    }
    else if (constraintsType == ObjLib_ObjectTypeConstraints)
    {
        return ObjLib_HandleObjTypeConstraints(object, typeDescriptor, constraints, keyIndex, values[0], errorHandler);
    }
    else
    {
        // Invalid constraints object type.
        ThrowError("[BUG] Invalid constraints object type. They are validated so this is a bug in objectlib.");
        return ObjConstraintResult_Invalid;
    }
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleCustomConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        const any:value[],
        size,
        ObjLib_ErrorHandler:errorHandler)
{
    // Disable constraints if this handler is already working.
    if (ObjLib_InCustomHandler)
    {
        return ObjConstraintResult_Valid;
    }
    ObjLib_InCustomHandler = true;
    
    // Get callback.
    new ObjLib_KeyValidator:callback = ObjLib_KeyValidator:ObjLib_GetFunction(constraints, "callback");
    
    // Get Key name.
    new String:keyName[OBJECT_KEY_NAME_LEN];
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    GetArrayString(keys, keyIndex, keyName, sizeof(keyName));
    
    // Get data type.
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    new ObjectDataType:dataType = ObjectDataType:GetArrayCell(dataTypes, keyIndex);
    
    // Call custom validator.
    // functag public ObjectConstraintResult:ObjLib_KeyValidator(Object:object, ObjectType:typeDescriptor, const String:keyName[], ObjectDataType:dataType, const any:values[], size);
    new ObjectConstraintResult:result = ObjConstraintResult_Invalid;
    Call_StartFunction(GetMyHandle(), callback);
    Call_PushCell(object);
    Call_PushCell(typeDescriptor);
    Call_PushString(keyName);
    Call_PushCell(dataType);
    Call_PushArray(value, size);
    Call_PushCell(size);
    Call_Finish(result);
    
    ObjLib_InCustomHandler = false;
    return result;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleLookupConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        const String:lookup[],
        ObjLib_ErrorHandler:errorHandler)
{
    // Disable constraints if this handler is already working.
    if (ObjLib_InLookupHandler)
    {
        return ObjConstraintResult_Valid;
    }
    ObjLib_InLookupHandler = true;
    
    // Get raw data type.
    new ObjectDataType:dataType;
    if (ObjLib_IsCollectionType(typeDescriptor))
    {
        // Get data type of collection.
        dataType = ObjectDataType:ObjLib_GetCell(object, "dataType");
    }
    else
    {
        // Get data type of key.
        new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
        dataType = ObjectDataType:GetArrayCell(dataTypes, keyIndex);
    }
    
    // Get constraint parameters.
    new ObjectLookupMethod:method = ObjectLookupMethod:ObjLib_GetCell(constraints, "method");
    new Handle:entries = ObjLib_GetHandle(constraints, "entries");
    new Handle:values = ObjLib_GetHandle(constraints, "values");
    new ObjLibLookupCallback:callback = ObjLib_GetFunction(constraints, "callback");
    
    // Buffer to store replacement value.
    decl any:buffer[OBJLIB_MAX_STRING_LEN];
    buffer[0] = 0;
    new numWritten = -1;
    
    // Delegate lookup to separate helpers according to lookup method.
    switch (method)
    {
        case ObjLookupMethod_Array:
        {
            numWritten = ObjLib_HandleArrayLookup(dataType, entries, values, lookup, buffer, sizeof(buffer));
        }
        case ObjLookupMethod_Trie:
        {
            numWritten = ObjLib_HandleTrieLookup(dataType, entries, lookup, buffer, sizeof(buffer));
        }
        case ObjLookupMethod_Callback:
        {
            numWritten = ObjLib_HandleCallbackLookup(object, dataType, callback, keyIndex, lookup, buffer, sizeof(buffer));
        }
    }
    
    // Check if failed.
    if (numWritten < 0)
    {
        ObjLib_InLookupHandler = false;
        
        // Throw constraint error. Format a nice message matching the constraint
        // error handler. ("Value violated constraint: <buffer>")
        Format(buffer, sizeof(buffer), "Lookup. Invalid lookup entry (\"%s\").", lookup);
        ObjLib_HandleConstraintError(buffer, object, typeDescriptor, keyIndex, errorHandler);
        
        return ObjConstraintResult_Invalid;
    }
    
    // Set value using public functions to recheck sub constraints if available.
    if (ObjLib_IsCollectionType(typeDescriptor))
    {
        // Collection.
        ObjLib_CollectionSetValue(Collection:object, dataType, keyIndex, buffer, sizeof(buffer), errorHandler);
    }
    else
    {
        // General object.
        ObjLib_SetValue(object, dataType, keyIndex, buffer, sizeof(buffer), errorHandler);
    }
    
    // Value is overridden and stored.
    ObjLib_InLookupHandler = false;
    return ObjConstraintResult_Overridden;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Helper for lookup constraint handler. Finds a lookup entry in an array and
 * copies the replacement value according to the data type specified.
 *
 * @param dataType      Data type of replacement values (match the object key's
 *                      data type).
 * @param entries       ADT array with list of string lookup entries.
 * @param values        ADT array with list of replacement values, matching
 *                      the data type.
 * @param lookup        Lookup entry to search for.
 * @param buffer        Buffer to store lookup result. Values are stored at the
 *                      first index. Strings and arrays stored as usual.
 * @param maxlen        Size of the buffer. Strings and array replacements are
 *                      truncated if the buffer is too small.
 *
 * @return              Number of cells or characters copied to buffer
 *                      (depending on data type), or -1 if lookup entry wasn't
 *                      found.
 */
stock ObjLib_HandleArrayLookup(
        ObjectDataType:dataType,
        Handle:entries,
        Handle:values,
        const String:lookup[],
        any:buffer[],
        maxlen)
{
    // Validate buffer size.
    if (maxlen <= 0)
    {
        ThrowError("Buffer size cannot be zero.");
        return -1;
    }
    
    // Search the entry array for the lookup string.
    new entryIndex = FindStringInArray(entries, lookup);
    
    // Check if found.
    if (entryIndex >= 0)
    {
        // Copy replacement value to result buffer.
        if (dataType == ObjDataType_Array)
        {
            return GetArrayArray(values, entryIndex, buffer, maxlen);
        }
        else if (dataType == ObjDataType_String)
        {
            return GetArrayString(values, entryIndex, buffer, maxlen);
        }
        else
        {
            buffer[0] = GetArrayCell(values, entryIndex);
            return 1;
        }
    }
    
    // Not found.
    return -1;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Helper for lookup constraint handler. Looks up an entry in a trie and copies
 * replacement value according to the data type specified.
 *
 * @param dataType      Data type of replacement values (match the object key's
 *                      data type).
 * @param entries       ADT trie with strings mapped to replacement values
 *                      (matching the data type).
 * @param lookup        Lookup entry to search for.
 * @param buffer        Buffer to store lookup result. Values are stored at the
 *                      first index. Strings and arrays stored as usual.
 * @param maxlen        Size of the buffer. Strings and array replacements are
 *                      truncated if the buffer is too small.
 *
 * @return              Number of cells or characters copied to buffer
 *                      (depending on data type), or -1 if lookup entry wasn't
 *                      found.
 */
stock ObjLib_HandleTrieLookup(
        ObjectDataType:dataType,
        Handle:entries,
        const String:lookup[],
        any:buffer[],
        maxlen)
{
    // Validate buffer size.
    if (maxlen <= 0)
    {
        ThrowError("Buffer size cannot be zero.");
        return -1;
    }
    
    new bool:found = false;
    new numWritten = 0;
    
    // Get replacement value and copy to result buffer.
    if (dataType == ObjDataType_Array)
    {
        found = GetTrieArray(entries, lookup, buffer, maxlen, numWritten);
    }
    else if (dataType == ObjDataType_String)
    {
        found = GetTrieString(entries, lookup, buffer, maxlen, numWritten);
    }
    else
    {
        found = GetTrieValue(entries, lookup, buffer[0]);
        numWritten = 1;
    }
    
    return found ? numWritten : -1;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Helper for lookup constraint handler. Looks up an entry through a callback
 * and copies replacement value according to the data type specified.
 *
 * @param object        Object being validated.
 * @param dataType      Data type of replacement values (match the object key's
 *                      data type).
 * @param callback      Lookup callback to use.
 * @param index         Index of key, or index where collection element is
 *                      added.
 * @param lookup        Lookup entry to search for.
 * @param buffer        Buffer to store lookup result. Values are stored at the
 *                      first index. Strings and arrays stored as usual.
 * @param maxlen        Size of the buffer. Strings and array replacements are
 *                      truncated if the buffer is too small.
 *
 * @return              Number of cells or characters copied to buffer
 *                      (depending on data type), or -1 if lookup entry wasn't
 *                      found.
 */
stock ObjLib_HandleCallbackLookup(
        Object:object,
        ObjectDataType:dataType,
        ObjLibLookupCallback:callback,
        index,
        const String:lookup[],
        any:buffer[],
        maxlen)
{
    // Prepare call to the callback.
    Call_StartFunction(GetMyHandle(), callback);
    
    /*
    Callback signatures
    
    Cell:
    bool:public(Object:object, index, const String:lookup[], &any:replacement)
    
    Array:
    public(Object:object, index, const String:lookup[], any:replacement[], maxlen)
    
    String:
    public(Object:object, index, const String:lookup[], String:replacement[], maxlen)
    */

    // Push common parameters.    
    Call_PushCell(object);
    Call_PushCell(index);
    Call_PushString(lookup);
    
    // Push parameters according to the data type (cell, array, string).
    if (dataType == ObjDataType_Array)
    {
        Call_PushArrayEx(buffer, maxlen, SM_PARAM_COPYBACK);
        Call_PushCell(maxlen);
    }
    else if (dataType == ObjDataType_String)
    {
        Call_PushStringEx(buffer, maxlen, SM_PARAM_STRING_UTF8, SM_PARAM_COPYBACK);
        Call_PushCell(maxlen);
    }
    else
    {
        Call_PushCellRef(buffer[0]);
    }
    
    // Call callback.
    new result;
    Call_Finish(result);
    
    // Convert and return result from callback.
    if (dataType == ObjDataType_Array || dataType == ObjDataType_String)
    {
        // Returned number of cells/characters copied.
        return result;
    }
    else
    {
        // Returned true if valid.
        return bool:result ? 1 : -1;
    }
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleCellConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        value,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new min = ObjLib_GetCell(constraints, "min");
    new max = ObjLib_GetCell(constraints, "max");
    new bool:lowerLimit = ObjLib_GetBool(constraints, "lowerLimit");
    new bool:upperLimit = ObjLib_GetBool(constraints, "upperLimit");
    new bool:nonzero = ObjLib_GetBool(constraints, "nonzero");
    
    // Validate lower limit.
    if (lowerLimit && value < min)
    {
        ObjLib_HandleConstraintError("min", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate upper limit.
    if (upperLimit && value > max)
    {
        ObjLib_HandleConstraintError("max", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Check if zero.
    if (nonzero && value == 0)
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleFloatConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        Float:value,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new Float:min = ObjLib_GetFloat(constraints, "min");
    new Float:max = ObjLib_GetFloat(constraints, "max");
    new bool:lowerLimit = ObjLib_GetBool(constraints, "lowerLimit");
    new bool:upperLimit = ObjLib_GetBool(constraints, "upperLimit");
    new bool:nonzero = ObjLib_GetBool(constraints, "nonzero");
    new Float:zeroDelta = ObjLib_GetFloat(constraints, "zeroDelta");
    
    // Validate lower limit.
    if (lowerLimit && value < min)
    {
        ObjLib_HandleConstraintError("min", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate upper limit.
    if (upperLimit && value > max)
    {
        ObjLib_HandleConstraintError("max", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Check if zero.
    if (nonzero && (value == 0 || FloatAbs(value) < zeroDelta))
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleHandleConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        Handle:value,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new bool:nonzero = ObjLib_GetBool(constraints, "nonzero");
    
    // Check if zero.
    if (nonzero && value != INVALID_HANDLE)
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}



/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleFunctionConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        Function:value,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new bool:nonzero = ObjLib_GetBool(constraints, "nonzero");
    
    // Check if nonzero.
    if (nonzero && value != INVALID_FUNCTION)
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleStringConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        const String:value[],
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new bool:nonempty = ObjLib_GetBool(constraints, "nonempty");
    new bool:lowerLimit = ObjLib_GetBool(constraints, "lowerLimit");
    new bool:upperLimit = ObjLib_GetBool(constraints, "upperLimit");
    new minLen = ObjLib_GetCell(constraints, "minLen");
    new maxLen = ObjLib_GetCell(constraints, "maxLen");
    new bool:pathValidation = ObjLib_GetBool(constraints, "pathValidation");
    new bool:fileValidation = ObjLib_GetBool(constraints, "fileValidation");
    new bool:includeValveFS = ObjLib_GetBool(constraints, "includeValveFS");
    //new bool:whitelist = ObjLib_GetBool(constraints, "whitelist");
    //new bool:blacklist = ObjLib_GetBool(constraints, "blacklist");
    
    // TODO
    /*decl String:whitelistChars[OBJECTLIB_WHITELIST_LEN];
    decl String:blacklistChars[OBJECTLIB_WHITELIST_LEN];
    ObjLib_GetString(constraint, "whitelistChars", whitelistChars, sizeof(whitelistChars));
    ObjLib_GetString(constraint, "blacklistChars", blacklistChars, sizeof(blacklistChars));*/
    
    new valueLen = strlen(value);
    
    // Check if empty.
    if (nonempty && valueLen == 0)
    {
        ObjLib_HandleConstraintError("nonempty", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate lower limit.
    if (lowerLimit && valueLen < minLen)
    {
        ObjLib_HandleConstraintError("minLen", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate upper limit.
    if (upperLimit && valueLen > maxLen)
    {
        ObjLib_HandleConstraintError("maxLen", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate path.
    if (pathValidation && !DirExists(value))
    {
        ObjLib_HandleConstraintError("pathValidation", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate file.
    if (fileValidation && !FileExists(value, includeValveFS))
    {
        ObjLib_HandleConstraintError("fileValidation", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // TODO: Check whitelisted and blacklisted characters.
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleArrayConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        const any:value[],
        size,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new bool:lowerLimit = ObjLib_GetBool(constraints, "lowerLimit");
    new bool:upperLimit = ObjLib_GetBool(constraints, "upperLimit");
    new minLen = ObjLib_GetCell(constraints, "minLen");
    new maxLen = ObjLib_GetCell(constraints, "maxLen");
    
    // Validate lower limit.
    if (lowerLimit && size < minLen)
    {
        ObjLib_HandleConstraintError("minLen", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate upper limit.
    if (upperLimit && size > maxLen)
    {
        ObjLib_HandleConstraintError("maxLen", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleCollectionConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        const any:value[],
        size,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new ObjectDataType:dataType = ObjectDataType:ObjLib_GetCell(constraints, "dataType");
    new minBlockSize = ObjLib_GetCell(constraints, "minBlockSize");
    new Object:elementConstraints = Object:ObjLib_GetObject(constraints, "elementConstraints");
    
    // Check if object is a collection.
    if (!ObjLib_IsCollectionType(typeDescriptor))
    {
        ObjLib_HandleConstraintError("objectType", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate data type.
    if (ObjectDataType:ObjLib_GetCell(object, "dataType") != dataType)
    {
        ObjLib_HandleConstraintError("dataType", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate minimum block size.
    if (ObjLib_GetCell(object, "blockSize") < minBlockSize)
    {
        ObjLib_HandleConstraintError("blockSize", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate element constraints.
    if (elementConstraints != INVALID_OBJECT &&
        elementConstraints != ObjLib_GetObject(object, "constraints"))
    {
        ObjLib_HandleConstraintError("elementConstraints", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleObjectConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        Object:value,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new bool:nonzero = ObjLib_GetBool(constraints, "nonzero");
    new ObjectType:type = ObjLib_GetObjectType(constraints, "type");
    
    // Check if zero.
    if (nonzero && value == INVALID_OBJECT)
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Check type.
    if (type != INVALID_OBJECT_TYPE)
    {
        new ObjectType:valueType = ObjLib_GetTypeDescriptor(value);
        if (valueType != type)
        {
            ObjLib_HandleConstraintError("type", object, typeDescriptor, keyIndex, errorHandler);
            return ObjConstraintResult_Invalid;
        }
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}



/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleObjTypeConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        ObjectType:value,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new bool:nonzero = ObjLib_GetBool(constraints, "nonzero");
    new bool:requireKeys = ObjLib_GetBool(constraints, "requireKeys");
    new Handle:keys = ObjLib_GetHandle(constraints, "keys");
    new Handle:dataTypes = ObjLib_GetHandle(constraints, "dataTypes");
    
    decl String:constraintName[16];
    constraintName[0] = 0;
    
    // Check if zero.
    if (nonzero && value == INVALID_OBJECT_TYPE)
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Check keys.
    if (requireKeys)
    {
        // Get data types.
        new Handle:valueDataTypes = ObjLib_GetTypeDataTypes(value);
        
        // Loop through keys.
        new len = GetArraySize(keys);
        for (new i = 0; i < len; i++)
        {
            // Get key name.
            decl String:keyName[OBJECT_KEY_NAME_LEN];
            GetArrayString(keys, i, keyName, sizeof(keyName));
            
            // Check if it exists.
            new key = ObjLib_GetKeyIndexFromType(typeDescriptor, keyName);
            if (key < 0)
            {
                ObjLib_HandleConstraintError("requireKeys", object, typeDescriptor, keyIndex, errorHandler);
                return ObjConstraintResult_Invalid;
            }
            else
            {
                // Check data type.
                if (GetArrayCell(valueDataTypes, key) != GetArrayCell(dataTypes, i))
                {
                    Format(constraintName, sizeof(constraintName), "dataTypes[%d]", i);
                    ObjLib_HandleConstraintError(constraintName, object, typeDescriptor, keyIndex, errorHandler);
                    return ObjConstraintResult_Invalid;
                }
            }
        }
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_HandleConstraintError(const String:constraintName[], Object:object, ObjectType:typeDescriptor, keyIndex, ObjLib_ErrorHandler:errorHandler)
{
    // Get Key name.
    new String:keyName[OBJECT_KEY_NAME_LEN];
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    GetArrayString(keys, keyIndex, keyName, sizeof(keyName));
    
    // TODO: Add support for collections.
    
    return ObjLib_HandleError(typeDescriptor, object, ObjLibError_ValidationError, errorHandler, _, _, "Cannot set value in key (\"%s\"). Value violated constraint: %s", keyName, constraintName);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Gets the constraint object for a object key or a collection.
 */
stock Object:ObjLib_GetConstraints(Object:object, ObjectType:typeDescriptor, keyIndex = 0)
{
    if (ObjLib_IsCollectionType(typeDescriptor))
    {
        // Object is a collection. Get constraint object.
        return ObjLib_GetObject(object, "constraints");
    }
    else
    {
        // Object is a general object. Get constraint object for the specified
        // key.
        return ObjLib_GetTypeConstraintsAt(typeDescriptor, keyIndex);
    }
}

/*____________________________________________________________________________*/

/**
 * Returns whether the constraint handler is currently handling a custom
 * constraint.
 */
stock bool:ObjLib_IsInCustomHandler()
{
    return ObjLib_InCustomHandler;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the constraint handler is currently handling a lookup
 * constraint.
 */
stock bool:ObjLib_IsInLookupHandler()
{
    return ObjLib_InLookupHandler;
}

/*____________________________________________________________________________*/

/**
 * Enables the constraint handler. Values will be validated if constraints are
 * available.
 */
stock ObjLib_EnableConstraints()
{
    ObjLib_ConstraintsEnabled = true;
}

/*____________________________________________________________________________*/

/**
 * Disables the constraint handler. Values will be stored without validation.
 *
 * Note: This setting will stay disabled until manually enabled again.
 *
 * Warning: Do not disable this unless you know what you're doing.
 */
stock ObjLib_DisableConstraints()
{
    ObjLib_ConstraintsEnabled = false;
}

/*____________________________________________________________________________*/

/**
 * Enables container storage handlers. Values can be stored in container objects
 * using their own storage handlers as usual.
 */
stock ObjLib_EnableContainerStorage()
{
    ObjLib_ContainerStorageEnabled = true;
}

/*____________________________________________________________________________*/

/**
 * Disables container storage handlers. Values will be stored directly in the
 * object. This is necessary to modify attributes of the container object
 * itself.
 *
 * Note: This setting will stay disabled until manually enabled again.
 *
 * Warning: Do not disable this unless you know what you're doing.
 */
stock ObjLib_DisableContainerStorage()
{
    ObjLib_ContainerStorageEnabled = false;
}
