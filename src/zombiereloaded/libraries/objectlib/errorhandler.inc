/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           errorhandler.inc
 *  Type:           Library
 *  Description:    Error handler for objectlib.
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
 * Internal use only!
 * General error handler. Calls the error custom handler if available, or throws
 * a regular error.
 *
 * @param typeDescriptor    Related type descriptor.
 * @param object            Related object, if available.
 * @param errorType         Type of error.
 * @param customHandler     Custom error handler. Overrides handler in type
 *                          descriptor if specified.
 * @param result            (Output) Return value from custom handler. Only
 *                          valid if this function returns true.
 * @param data              Data bundle with additional data, if available.
 * @param format            Message formatting rules.
 * @param ...               Format parameters.
 *
 * @return      True if error was handled. If it's not handled this function
 *              doesn't return at all.
 */
stock bool:ObjLib_HandleError(ObjectType:typeDescriptor,
                              Object:object = INVALID_OBJECT,
                              ObjLibError:errorType,
                              ObjLib_ErrorHandler:customHandler = INVALID_FUNCTION,
                              &Action:result = Plugin_Handled,
                              Object:data = INVALID_OBJECT,
                              const String:format[],
                              any:...)
{
    // Get error handler callback.
    new ObjLib_ErrorHandler:callback;
    if (customHandler != INVALID_FUNCTION)
    {
        // Use custom error handler.
        callback = customHandler;
    }
    else
    {
        // Use handler in type descriptor.
        callback = ObjLib_GetTypeErrorHandler(typeDescriptor);
    }
    
    // Format message.
    decl String:message[255];
    message[0] = 0;
    VFormat(message, sizeof(message), format, 8);
    
    if (callback == INVALID_FUNCTION)
    {
        // No error handler. Throw error instead.
        ThrowError(message);
        
        return false;   // To satisfy compiler.
    }
    else
    {
        // Forward to callback.
        // functag public Action:ObjLib_ErrorHandler(ObjectType:typeDescriptor, ObjLibError:errorType, const String:message[], Object:object, Object:data);
        Call_StartFunction(GetMyHandle(), callback);
        Call_PushCell(typeDescriptor);
        Call_PushCell(errorType);
        Call_PushString(message);
        Call_PushCell(object);
        Call_PushCell(data);
        Call_Finish(result);
        return true;
    }
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_ErrorHandler:ObjLib_GetErrorHandler(ObjectType:typeDescriptor, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    if (errorHandler != INVALID_FUNCTION)
    {
        return errorHandler;
    }
    
    return ObjLib_GetTypeErrorHandler(typeDescriptor);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Throws a general type mismatch error.
 *
 * @return      True if error was handled. If not handled this function does not
 *              return.
 */
stock bool:ObjLib_TypeMismatchError(ObjectType:typeDescriptor, Object:object, ObjectDataType:actual, ObjectDataType:expected, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    new String:actualName[16];
    new String:expectedName[16];
    
    ObjLib_DataTypeToString(actual, actualName, sizeof(actualName));
    ObjLib_DataTypeToString(expected, expectedName, sizeof(expectedName));
    
    return ObjLib_HandleError(typeDescriptor, object, ObjLibError_TypeMismatch, errorHandler, _, _, "Data type mismatch. Expected \"%s\", but \"%s\" was used.", expectedName, actualName);
}
