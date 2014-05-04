/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           kverrors.inc
 *  Type:           Library
 *  Description:    Error handlers for KeyVaue parser.
 *
 *  Copyright (C) 2012-2013  Richard Helgeby, Greyscale
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
 * Triggers an unexpected key error.
 *
 * @param object            Current object.
 * @param typeDescriptor    Type descriptor for object.
 * @param errorHandler      Error handler to use on errors.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 * @param keyName           Name of the key.
 *
 * @return                  True if error was handled. If not handled this
 *                          function does not return at all.
 */
stock bool:ObjLib_KvUnexpectedKeyError(Object:object, ObjectType:typeDescriptor, ObjLib_ErrorHandler:errorHandler, Object:parseContext, const String:keyName[])
{
    // Get path to current location in keyvalue tree.
    decl String:path[PLATFORM_MAX_PATH];
    ObjLib_KvBuildPath(parseContext, path, sizeof(path));
    
    new Action:result = Plugin_Handled;
    if (ObjLib_HandleError(typeDescriptor,                      // typeDescriptor
                           object,                              // object
                           ObjLibError_KvUnexpectedKey,         // errorType
                           errorHandler,                        // customHandler
                           result,                              // result
                           parseContext,                        // data
                           "Unexpected key \"%s/%s\". It does not exist in the type descriptor. Key names must be in lower case in the type descriptor.",   // format
                           path,
                           keyName))
    {
        // Error was handled in customHandler. Update parser state, skip this
        // key if continuing.
        ObjLib_KvUpdateParserState(result, parseContext);
        
        // Return to parse loop. The parse state will tell if parser should
        // abort or continue.
        return true;
    }
    
    // Not reachable due to call to ThrowError.
    return false;
}

/*____________________________________________________________________________*/

/**
 * Triggers an unexpected section error.
 *
 * @param object            Current object.
 * @param typeDescriptor    Type descriptor for object.
 * @param errorHandler      Error handler to use on errors.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 * @param sectionName       Name of the section.
 *
 * @return                  True if error was handled. If not handled this
 *                          function does not return at all.
 */
stock bool:ObjLib_KvUnexpectedSectionError(Object:object, ObjectType:typeDescriptor, ObjLib_ErrorHandler:errorHandler, Object:parseContext, const String:sectionName[])
{
    // Get path to current location in keyvalue tree.
    decl String:path[PLATFORM_MAX_PATH];
    ObjLib_KvBuildPath(parseContext, path, sizeof(path));
    
    new Action:result = Plugin_Handled;
    if (ObjLib_HandleError(typeDescriptor,                      // typeDescriptor
                           object,                              // object
                           ObjLibError_KvUnexpectedSection,     // errorType
                           errorHandler,                        // customHandler
                           result,                              // result
                           parseContext,                        // data
                           "Unexpected section \"%s/%s\". It does not exist in the type descriptor. Section names must be in lower case in the type descriptor.",   // format
                           path,
                           sectionName))
    {
        // Error was handled in customHandler. Update parser state, skip section
        // if continuing.
        ObjLib_KvUpdateParserState(result, parseContext, ObjLibState_SkipSection);
        
        // Return to parse loop. The parse state will tell if parser should
        // abort or continue.
        return true;
    }
    
    // Not reachable due to call to ThrowError.
    return false;
}

/*____________________________________________________________________________*/

/**
 * Triggers an invalid section error.
 *
 * @param object            Current object.
 * @param typeDescriptor    Type descriptor for object.
 * @param errorHandler      Error handler to use on errors.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 * @param sectionName       Name of the section.
 *
 * @return                  True if error was handled. If not handled this
 *                          function does not return at all.
 */
stock bool:ObjLib_KvInvalidSectionError(Object:object, ObjectType:typeDescriptor, ObjLib_ErrorHandler:errorHandler, Object:parseContext, const String:sectionName[])
{
    // Get path to current location in keyvalue tree.
    decl String:path[PLATFORM_MAX_PATH];
    ObjLib_KvBuildPath(parseContext, path, sizeof(path));
    
    new Action:result = Plugin_Handled;
    if (ObjLib_HandleError(typeDescriptor,                      // typeDescriptor
                           object,                              // object
                           ObjLibError_KvInvalidSection,        // errorType
                           errorHandler,                        // customHandler
                           result,                              // result
                           parseContext,                        // data
                           "Invalid section at \"%s/%s\". It's supposed to be a key value pair.",   // format
                           path,
                           sectionName))
    {
        // Error was handled in customHandler. Update parser state, skip section
        // if continuing.
        ObjLib_KvUpdateParserState(result, parseContext, ObjLibState_SkipSection);
        
        // Return to parse loop. The parse state will tell if parser
        // should abort or continue.
        return true;
    }
    
    // Not reachable due to call to ThrowError.
    return false;
}

/*____________________________________________________________________________*/

/**
 * Triggers an constraint type mismatch error.
 *
 * @param object            Current object.
 * @param typeDescriptor    Type descriptor for object.
 * @param errorHandler      Error handler to use on errors.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 * @param keyName           Name of the related key.
 * @param constraintType    Type descriptor for constraint object.
 *
 * @return                  True if error was handled. If not handled this
 *                          function does not return at all.
 */
stock bool:ObjLib_KvConstraintTypeMismatch(Object:object, ObjectType:typeDescriptor, ObjLib_ErrorHandler:errorHandler, Object:parseContext, const String:keyName[], ObjectType:constraintType)
{
    // Get path to current location in keyvalue tree.
    decl String:path[PLATFORM_MAX_PATH];
    ObjLib_KvBuildPath(parseContext, path, sizeof(path));
    
    new Action:result = Plugin_Handled;
    if (ObjLib_HandleError(typeDescriptor,                      // typeDescriptor
                           object,                              // object
                           ObjLibError_KvInvalidConstraint,     // errorType
                           errorHandler,                        // customHandler
                           result,                              // result
                           parseContext,                        // data
                           "Invalid constraint type for key \"%s/%s\": %x. It's unknown or does not match the key data type.",   // format
                           path,
                           keyName,
                           constraintType))
    {
        // Error was handled in customHandler. Update parser state.
        ObjLib_KvUpdateParserState(result, parseContext);
        
        // Return to parse loop. The parse state will tell if parser
        // should abort or continue.
        return true;
    }
    
    // Not reachable due to call to ThrowError.
    return false;
}

/*____________________________________________________________________________*/

/**
 * Triggers a general type mismatch error.
 *
 * @param object            Current object.
 * @param typeDescriptor    Type descriptor for object.
 * @param errorHandler      Error handler to use on errors.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 * @param keyName           Name of the related key.
 * @param expectedType      Expected data type.
 * @param actualType        Actual data type.
 *
 * @return                  True if error was handled. If not handled this
 *                          function does not return at all.
 */
stock bool:ObjLib_KvTypeMismatchError(Object:object, ObjectType:typeDescriptor, ObjLib_ErrorHandler:errorHandler, Object:parseContext, const String:keyName[], ObjectDataType:expectedType, ObjectDataType:actualType)
{
    // Get path to current location in keyvalue tree.
    decl String:path[PLATFORM_MAX_PATH];
    ObjLib_KvBuildPath(parseContext, path, sizeof(path));
    
    // Convert data types to strings.
    new String:expectedTypeString[32];
    new String:actualTypeString[32];
    ObjLib_DataTypeToString(expectedType, expectedTypeString, sizeof(expectedTypeString));
    ObjLib_DataTypeToString(actualType, actualTypeString, sizeof(actualTypeString));
    
    new Action:result = Plugin_Handled;
    if (ObjLib_HandleError(typeDescriptor,                      // typeDescriptor
                           object,                              // object
                           ObjLibError_KvTypeMismatch,          // errorType
                           errorHandler,                        // customHandler
                           result,                              // result
                           parseContext,                        // data
                           "Key type mismatch: \"%s/%s\". Expected %s, but %s was used.",   // format
                           path,
                           keyName,
                           expectedTypeString,
                           actualTypeString))
    {
        // Error was handled in customHandler. Update parser state.
        ObjLib_KvUpdateParserState(result, parseContext);
        
        // Return to parse loop. The parse state will tell if parser
        // should abort or continue.
        return true;
    }
    
    // Not reachable due to call to ThrowError.
    return false;
}

/*____________________________________________________________________________*/

/**
 * Triggers a collection element type mismatch error.
 *
 * @param object            Current object.
 * @param typeDescriptor    Type descriptor for object.
 * @param errorHandler      Error handler to use on errors.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 * @param expectedType      Expected data type.
 * @param actualType        Actual data type.
 *
 * @return                  True if error was handled. If not handled this
 *                          function does not return at all.
 */
stock bool:ObjLib_KvCollectionTypeMismatchError(Collection:object, ObjectType:typeDescriptor, ObjLib_ErrorHandler:errorHandler, Object:parseContext, const String:elementName[], ObjectDataType:expectedType, ObjectDataType:actualType)
{
    // Get path to current location in keyvalue tree.
    decl String:path[PLATFORM_MAX_PATH];
    ObjLib_KvBuildPath(parseContext, path, sizeof(path));
    
    // Convert data types to strings.
    new String:expectedTypeString[32];
    new String:actualTypeString[32];
    ObjLib_DataTypeToString(expectedType, expectedTypeString, sizeof(expectedTypeString));
    ObjLib_DataTypeToString(actualType, actualTypeString, sizeof(actualTypeString));
    
    new Action:result = Plugin_Handled;
    if (ObjLib_HandleError(typeDescriptor,                      // typeDescriptor
                           Object:object,                       // object
                           ObjLibError_KvCollectionTypeMismatch,// errorType
                           errorHandler,                        // customHandler
                           result,                              // result
                           parseContext,                        // data
                           "Collection element type mismatch at \"%s/%s\". Expected %s, but %s was used.",   // format
                           path,
                           elementName,
                           expectedTypeString,
                           actualTypeString))
    {
        // Error was handled in customHandler. Update parser state, skip section
        // if continuing.
        ObjLib_KvUpdateParserState(result, parseContext, ObjLibState_SkipSection);
        
        // Return to parse loop. The parse state will tell if parser
        // should abort or continue.
        return true;
    }
    
    // Not reachable due to call to ThrowError.
    return false;
}

/*____________________________________________________________________________*/

/**
 * Fallback error handler in case no error handler is provided to parser.
 *
 * @param typeDescriptor    Related type descriptor.
 * @param errorType         Type of error.
 * @param message           Error message.
 * @param object            Related object, if available.
 * @param data              Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 *
 * @return                  What to do next:
 *                          Plugin_Handled - Stop parsing.
 *                          Plugin_Continue - Skip current element, continue
 *                          parsing remaining keys.
 */
public Action:ObjLib_KvErrorHandler(ObjectType:typeDescriptor, ObjLibError:errorType, const String:message[], Object:object, Object:data)
{
    new Object:parseContext = ObjLib_KvGetStoredParseContext(data);
    new bool:continueOnError = ObjLib_GetBool(parseContext, "continueOnError");
    
    // Get path to current location in keyvalue tree.
    decl String:path[PLATFORM_MAX_PATH];
    ObjLib_KvBuildPath(parseContext, path, sizeof(path));
    
    // Get parser name if available.
    decl String:name[OBJLIB_KV_MAX_KEY_LEN];
    name[0] = 0;
    if (!ObjLib_IsKeyNull(parseContext, "name"))
    {
        ObjLib_GetString(parseContext, "name", name, sizeof(name));
    }
    else
    {
        strcopy(name, sizeof(name), "Unnamed parser");
    }
    
    // Log general parse error.
    LogError("[%s] Error parsing element at \"%s\": %s", name, path, message);
    
    // Remove parse context from temp storage.
    TempParseContext = INVALID_OBJECT;
    
    // Check if parser should continue.
    if (continueOnError)
    {
        return Plugin_Continue;
    }
    else
    {
        return Plugin_Handled;
    }
}

/*____________________________________________________________________________*/


/**
 * Builds a path to the current parser location.
 *
 * @param parseContext      Parser context object.
 * @param buffer            Destination string buffer.
 * @param maxlen            Size of buffer.
 *
 * @return                  Number of bytes written (including null terminator).
 */
stock ObjLib_KvBuildPath(Object:parseContext, String:buffer[], maxlen)
{
    new Handle:nameStack = ObjLib_GetHandle(parseContext, "nameStack");
    new len = GetArraySize(nameStack);
    new count = 0;
    
    decl String:nameElement[OBJECT_KEY_NAME_LEN];
    nameElement[0] = 0;
    
    // Initialize buffer.
    buffer[0] = 0;
    
    // Check if no elements.
    if (len == 0)
    {
        return count + 1;
    }
    
    // Append name elements to path.
    for (new i = 0; i < len; i++)
    {
        GetArrayString(nameStack, i, nameElement, sizeof(nameElement));
        count += StrCat(buffer, maxlen, nameElement);
        
        // Append path separator, except at the end.
        if (i < len - 1)
        {
            count += StrCat(buffer, maxlen, "/");
        }
    }
    
    return count + 1;
}

/*____________________________________________________________________________*/


/**
 * Returns the error handler from the parse context, or a fallback error
 * handler.
 */
stock ObjLib_ErrorHandler:ObjLib_KvGetErrorHandler(Object:parseContext)
{
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_GetFunction(parseContext, "errorHandler");
    
    if (errorHandler == INVALID_FUNCTION)
    {
        // No error handler specified.Store parser context.
        TempParseContext = parseContext;
        
        // Use fallback error handler if enabled.
        #if defined OBJLIB_KV_SOFT_ERROR
            errorHandler = ObjLib_KvErrorHandler;
        #endif
    }
    
    return errorHandler;
}

/*____________________________________________________________________________*/

/**
 * Returns the parse context or parse context in temp storage.
 */
stock Object:ObjLib_KvGetStoredParseContext(Object:parseContext)
{
    if (parseContext != INVALID_OBJECT)
    {
        // Validate type.
        if (ObjLib_GetTypeDescriptor(parseContext) == ObjLib_KvContextType)
        {
            return parseContext;
        }
        else
        {
            ThrowError("[BUG] Invalid object type, expected parse context. This is a bug in objectlib.");
            return INVALID_OBJECT;
        }
    }
    else
    {
        if (TempParseContext != INVALID_OBJECT)
        {
            return TempParseContext;
        }
        else
        {
            ThrowError("[BUG] Missing parse context in temp storage. This is a bug in objectlib.");
            return INVALID_OBJECT;
        }
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!.
 * Updates parser state according to an error handler result.
 *
 * @param result            Result value from error handler.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 * @param continueState     (Optional) Parser state to set if continuing
 *                          parsing. Default is ObjLibSate_Running.
 * @param abortState        (Optional) Parser state to set if aborting parsing.
 *                          Default is ObjLibState_Aborted.
 */
stock ObjLib_KvUpdateParserState(Action:result, Object:parseContext, ObjLibParseState:continueState = ObjLibState_Running, ObjLibParseState:abortState = ObjLibState_Aborted)
{
    // Error was handled in customHandler. Update parser state.
    if (result == Plugin_Continue)
    {
        // Continue parsing.
        ObjLib_SetCell(parseContext, "parseState", continueState);
    }
    else
    {
        // Abort parsing.
        ObjLib_SetCell(parseContext, "parseState", abortState);
    }
}
