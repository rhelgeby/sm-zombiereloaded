/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           inspector.inc
 *  Type:           Library
 *  Description:    Object inspector utility for objectlib.
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

static stock ObjLib_InspectorBuilt = false;
static stock String:ObjLib_CommandPrefix[16];

/*____________________________________________________________________________*/

/**
 * Builds the following object inspector commands as admin commands:
 *
 * object_help                  - Prints an overview of inspector commands.
 * object_inspect               - Inspect object keys and values.
 * object_inspect_ex            - Inspect object raw data.
 * object_inspect_type_ex       - Inspect type descriptor raw data.
 * object_inspect_array_cell    - Inspect array elements as cell values.
 * object_inspect_array_float   - Inspect array elements as float values.
 * object_inspect_array_string  - Inspect array elements as string values.
 * object_inspect_array_hex     - Inspect array elements as hex values.
 *
 * Note: Once the inspector is built, it cannot be changed or removed until
 *       the plugin is reloaded.
 *
 * Note: The command prefix is also applied so that the command will start with:
 *       <prefix>_object...
 *
 *       If the prefix string is empty commands will start with: object_...
 *
 * @param commandPrefix     (Optional) Short prefix to use at the beginning of
 *                          admin command names. It's recommended to specify
 *                          this to avoid conflicts with other plugins also
 *                          using this inspector.
 * @param adminFlags        (Optional) Administrative flags (bitstring) to use
 *                          for permissions. Default is ADMFLAG_ROOT.
 * @param group             (Optional) String containing the command group to
 *                          use. If empty, the plugin's filename will be used
 *                          instead. Default is empty.
 */
stock ObjLib_BuildInspector(const String:commandPrefix[] = "", adminFlags = ADMFLAG_ROOT, const String:group[] = "")
{
    if (ObjLib_InspectorBuilt)
    {
        // Only build once.
        return;
    }
    
    // Store prefix.
    strcopy(ObjLib_CommandPrefix, sizeof(ObjLib_CommandPrefix), commandPrefix);
    
    new String:command[64];
    
    ObjLib_GetCommandName(commandPrefix, "object_help", command, sizeof(command));
    RegAdminCmd(command, ObjLibCommand_Help, adminFlags, "Prints an overview of inspector commands.", group);
    
    ObjLib_GetCommandName(commandPrefix, "object_inspect", command, sizeof(command));
    RegAdminCmd(command, ObjLibCommand_Inspect, adminFlags, "Inspects an object's keys and values. Usage: object_inspect <object address>", group);
    
    ObjLib_GetCommandName(commandPrefix, "object_inspect_ex", command, sizeof(command));
    RegAdminCmd(command, ObjLibCommand_InspectEx, adminFlags, "Inspect an object's raw data. Usage: object_inspect_ex <object address>", group);
    
    ObjLib_GetCommandName(commandPrefix, "object_inspect_type_ex", command, sizeof(command));
    RegAdminCmd(command, ObjLibCommand_InspectTypeEx, adminFlags, "Inspects a type descriptor's raw data. Usage: object_inspect_type_ex <type descriptor address>", group);
    
    ObjLib_GetCommandName(commandPrefix, "object_inspect_array_cell", command, sizeof(command));
    RegAdminCmd(command, ObjLibCommand_InspectArrayCell, adminFlags, "Inspects cell values in an array. Usage: object_inspect_array_cell <array handle>", group);
    
    ObjLib_GetCommandName(commandPrefix, "object_inspect_array_float", command, sizeof(command));
    RegAdminCmd(command, ObjLibCommand_InspectArrayFloat, adminFlags, "Inspects float values in an array. Usage: object_inspect_array_float <array handle>", group);
    
    ObjLib_GetCommandName(commandPrefix, "object_inspect_array_string", command, sizeof(command));
    RegAdminCmd(command, ObjLibCommand_InspectArrayString, adminFlags, "Inspects string values in an array. Usage: object_inspect_array_string <array handle>", group);
    
    ObjLib_GetCommandName(commandPrefix, "object_inspect_array_hex", command, sizeof(command));
    RegAdminCmd(command, ObjLibCommand_InspectArrayHex, adminFlags, "Inspects hex values in an array. Usage: object_inspect_array_hex <array handle>", group);
    
    ObjLib_InspectorBuilt = true;
}

/*____________________________________________________________________________*/

/**
 * Gets a command name with prefix applied, if any.
 */
stock ObjLib_GetCommandName(const String:prefix[], const String:commandName[], String:buffer[], maxlen)
{
    if (strlen(prefix) == 0)
    {
        strcopy(buffer, maxlen, commandName);
    }
    else
    {
        Format(buffer, maxlen, "%s_%s", prefix, commandName);
    }
}

/*____________________________________________________________________________*/

/**
 * Command handler for object_inspect. Inspects keys in the specified object.
 *
 * @param client        Command sender.
 * @param argc          Number of arguments.
 */
public Action:ObjLibCommand_Help(client, argc)
{
    new String:prefix[16];
    
    if (strlen(ObjLib_CommandPrefix) > 0)
    {
        Format(prefix, sizeof(prefix), "%s_", ObjLib_CommandPrefix);
    }
    
    ReplyToCommand(client, "Object inspector commands:");
    ReplyToCommand(client, "%sobject_help                                         - Prints this overview.", prefix);
    ReplyToCommand(client, "%sobject_inspect <object address>                     - Inspect object keys and values.", prefix);
    ReplyToCommand(client, "%sobject_inspect_ex <object address>                  - Inspect object raw data.", prefix);
    ReplyToCommand(client, "%sobject_inspect_type_ex <type descriptor address>    - Inspect type descriptor raw data.", prefix);
    ReplyToCommand(client, "%sobject_inspect_array_cell <array handle>            - Inspect array elements as cell values.", prefix);
    ReplyToCommand(client, "%sobject_inspect_array_float <array handle>           - Inspect array elements as float values.", prefix);
    ReplyToCommand(client, "%sobject_inspect_array_string <array handle>          - Inspect array elements as string values.", prefix);
    ReplyToCommand(client, "%sobject_inspect_array_hex <array handle>             - Inspect array elements as hex values.", prefix);
    
    ReplyToCommand(client, "\nNote: All addresses and handles must be written as hex values (without the 0x prefix).");
    
    return Plugin_Handled;
}

/*____________________________________________________________________________*/

/**
 * Command handler for object_inspect. Inspects keys in the specified object.
 *
 * @param client        Command sender.
 * @param argc          Number of arguments.
 */
public Action:ObjLibCommand_Inspect(client, argc)
{
    new String:argBuffer[16];
    GetCmdArg(1, argBuffer, sizeof(argBuffer));
    
    new Object:object;
    
    // Parse hex string.
    StringToIntEx(argBuffer, _:object, 16);
    
    ReplyToCommand(client, "Inspecting object 0x%X.", object);
    
    ObjLib_DumpObjectKeys(client, object);
    
    return Plugin_Handled;
}

/*____________________________________________________________________________*/

/**
 * Command handler for object_inspect_ex. Inspects raw data in the specified
 * object.
 *
 * @param client        Command sender.
 * @param argc          Number of arguments.
 */
public Action:ObjLibCommand_InspectEx(client, argc)
{
    new String:argBuffer[16];
    GetCmdArg(1, argBuffer, sizeof(argBuffer));
    
    new Object:object;
    
    // Parse hex string.
    StringToIntEx(argBuffer, _:object, 16);
    
    ReplyToCommand(client, "Inspecting raw data in object 0x%X.", object);
    
    ObjLib_DumpRawObject(client, object);
    
    return Plugin_Handled;
}

/*____________________________________________________________________________*/

/**
 * Command handler for object_inspect_type. Inspects raw data in the specified
 * type descriptor.
 *
 * @param client        Command sender.
 * @param argc          Number of arguments.
 */
public Action:ObjLibCommand_InspectTypeEx(client, argc)
{
    new String:argBuffer[16];
    GetCmdArg(1, argBuffer, sizeof(argBuffer));
    
    new ObjectType:typeDescriptor;
    
    // Parse hex string.
    StringToIntEx(argBuffer, _:typeDescriptor, 16);
    
    ReplyToCommand(client, "Inspecting raw data in type descriptor 0x%X.", typeDescriptor);
    
    ObjLib_DumpRawType(client, typeDescriptor);
    
    return Plugin_Handled;
}

/*____________________________________________________________________________*/

/**
 * Command handler for object_inspect_array_cells. Inspects cells in an array.
 *
 * @param client        Command sender.
 * @param argc          Number of arguments.
 */
public Action:ObjLibCommand_InspectArrayCell(client, argc)
{
    new String:argBuffer[16];
    GetCmdArg(1, argBuffer, sizeof(argBuffer));
    
    new Handle:array;
    
    // Parse hex string.
    StringToIntEx(argBuffer, _:array, 16);
    
    ReplyToCommand(client, "Inspecting array 0x%X.", array);
    
    new String:values[512];
    Array_ADTCellsToString(array, values, sizeof(values), false);
    
    ReplyToCommand(client, values);
    
    return Plugin_Handled;
}

/*____________________________________________________________________________*/

/**
 * Command handler for object_inspect_array_cells. Inspects cells in an array.
 *
 * @param client        Command sender.
 * @param argc          Number of arguments.
 */
public Action:ObjLibCommand_InspectArrayFloat(client, argc)
{
    new String:argBuffer[16];
    GetCmdArg(1, argBuffer, sizeof(argBuffer));
    
    new Handle:array;
    
    // Parse hex string.
    StringToIntEx(argBuffer, _:array, 16);
    
    ReplyToCommand(client, "Inspecting array 0x%X.", array);
    
    new String:values[512];
    Array_ADTFloatToString(array, values, sizeof(values), false);
    
    ReplyToCommand(client, values);
    
    return Plugin_Handled;
}

/*____________________________________________________________________________*/

/**
 * Command handler for object_inspect_array_cells. Inspects cells in an array.
 *
 * @param client        Command sender.
 * @param argc          Number of arguments.
 */
public Action:ObjLibCommand_InspectArrayString(client, argc)
{
    new String:argBuffer[16];
    GetCmdArg(1, argBuffer, sizeof(argBuffer));
    
    new Handle:array;
    
    // Parse hex string.
    StringToIntEx(argBuffer, _:array, 16);
    
    ReplyToCommand(client, "Inspecting array 0x%X.", array);
    
    new String:values[512];
    Array_ADTToString(array, values, sizeof(values), false);
    
    ReplyToCommand(client, values);
    
    return Plugin_Handled;
}

/*____________________________________________________________________________*/

/**
 * Command handler for object_inspect_array_cells. Inspects cells in an array.
 *
 * @param client        Command sender.
 * @param argc          Number of arguments.
 */
public Action:ObjLibCommand_InspectArrayHex(client, argc)
{
    new String:argBuffer[16];
    GetCmdArg(1, argBuffer, sizeof(argBuffer));
    
    new Handle:array;
    
    // Parse hex string.
    StringToIntEx(argBuffer, _:array, 16);
    
    ReplyToCommand(client, "Inspecting array 0x%X.", array);
    
    new String:values[512];
    Array_ADTHexToString(array, values, sizeof(values), false);
    
    ReplyToCommand(client, values);
    
    return Plugin_Handled;
}

/*____________________________________________________________________________*/

/**
 * Prints object contents to the specified client.
 *
 * Output style:
 *
 * Key:                    Data type:  Value:
 * -----------------------------------------------------------------------------
 * exampleKey1             Array       {0, 1, 2, 3, 4, 5}
 * exampleKey2             String      "test string"
 * exampleKey3             Float       2.5
 * exampleKey4             Object      0x12345678
 *
 * 4 key(s) total.
 *
 * @param client        Client or server index.
 * @param object        Object to inspect.
 */
stock ObjLib_DumpObjectKeys(client, Object:object)
{
    static String:HEADER_FORMAT[] = "%23s %11s %s\n-------------------------------------------------------------------------------";
    static String:ROW_FORMAT[] = "%23s %11s %s";
    
    new String:keyName[OBJECT_KEY_NAME_LEN];
    new String:dataTypeString[32];
    new String:valueString[OBJLIB_MAX_STRING_LEN];
    
    // Get type.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Get keys.
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    new Handle:nullKey = ObjLib_GetObjectNullKey(object);
    
    // Get data types.
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    
    // Print table header.
    ReplyToCommand(client, HEADER_FORMAT, "Key:", "Data type:", "Value:");
    
    // Loop through keys.
    new numKeys = ObjLib_GetNumKeys(typeDescriptor);
    for (new key = 0; key < numKeys; key++)
    {
        // Get key name.
        GetArrayString(keys, key, keyName, sizeof(keyName));
        
        // Get data type.
        ObjLib_DataTypeToString(ObjectDataType:GetArrayCell(dataTypes, key), dataTypeString, sizeof(dataTypeString));
        
        // Convert value to a string.
        new bool:isNull = bool:GetArrayCell(nullKey, key);
        if (isNull)
        {
            strcopy(valueString, sizeof(valueString), "(Null)");
        }
        else
        {
            ObjLib_ValueToString(object, key, valueString, sizeof(valueString));
        }
        
        // Print row.
        ReplyToCommand(client, ROW_FORMAT, keyName, dataTypeString, valueString);
    }
    
    ReplyToCommand(client, "\n%d key(s) total.", numKeys);
}

/*____________________________________________________________________________*/

/**
 * Prints object raw data to the specified client.
 *
 * @param client        Client or server index.
 * @param object        Object to inspect.
 */
stock ObjLib_DumpRawObject(client, Object:object)
{
    static String:HEADER_FORMAT[] = "%23s %15s %s\n-------------------------------------------------------------------------------";
    static String:ROW_FORMAT[] = "%23s %15s %s";
    
    new String:valueString[16];
    
    // Get raw data.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    new Handle:nullKey = ObjLib_GetObjectNullKey(object);
    new Handle:data = ObjLib_GetObjectData(object);
    
    // Print table header.
    ReplyToCommand(client, HEADER_FORMAT, "Field:", "Data type:", "Value:");
    
    // data
    Format(valueString, sizeof(valueString), "0x%X", data);
    ReplyToCommand(client, ROW_FORMAT, "Object_Data", "Array", valueString);
    
    // nullKey
    Format(valueString, sizeof(valueString), "0x%X", nullKey);
    ReplyToCommand(client, ROW_FORMAT, "Object_NullKey", "Array", valueString);
    
    // typeDescriptor
    Format(valueString, sizeof(valueString), "0x%X", typeDescriptor);
    ReplyToCommand(client, ROW_FORMAT, "Object_MetaData", "ObjectType", valueString);
}

/*____________________________________________________________________________*/

/**
 * Prints type descriptor raw data to the specified client.
 *
 * @param client        Client or server index.
 * @param object        Object to inspect.
 */
stock ObjLib_DumpRawType(client, ObjectType:typeDescriptor)
{
    static String:HEADER_FORMAT[] = "%23s %15s %s\n-------------------------------------------------------------------------------";
    static String:ROW_FORMAT[] = "%23s %15s %s";
    
    new String:valueString[16];
    
    // Get raw data.
    new bool:locked = ObjLib_IsTypeMutable(typeDescriptor);
    new Object:parent = ObjLib_GetTypeParentObject(typeDescriptor);
    new keySize = ObjLib_GetTypeKeySize(typeDescriptor);
    new blockSize = ObjLib_GetTypeBlockSize(typeDescriptor);
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    new Handle:nameIndex = ObjLib_GetTypeNameIndex(typeDescriptor);
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    new Handle:constraints = ObjLib_GetTypeConstraints(typeDescriptor);
    new ObjLib_ErrorHandler:errorHandler = ObjLib_GetTypeErrorHandler(typeDescriptor);
    
    // Print table header.
    ReplyToCommand(client, HEADER_FORMAT, "Field:", "Data type:", "Value:");
    
    // locked
    Format(valueString, sizeof(valueString), "%d", locked);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_Locked", "Boolean", valueString);
    
    // parent
    Format(valueString, sizeof(valueString), "0x%X", parent);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_ParentObject", "Object", valueString);
    
    // keySize
    Format(valueString, sizeof(valueString), "%d", keySize);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_KeySize", "Integer", valueString);
    
    // blockSize
    Format(valueString, sizeof(valueString), "%d", blockSize);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_BlockSize", "Integer", valueString);
    
    // keys
    Format(valueString, sizeof(valueString), "0x%X", keys);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_Keys", "Array", valueString);
    
    // nameIndex
    Format(valueString, sizeof(valueString), "0x%X", nameIndex);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_NameIndex", "Trie", valueString);
    
    // dataTypes
    Format(valueString, sizeof(valueString), "0x%X", dataTypes);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_DataTypes", "Array", valueString);
    
    // constraints
    Format(valueString, sizeof(valueString), "0x%x", constraints);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_Constraints", "Array", valueString);
    
    // errorHandler
    Format(valueString, sizeof(valueString), "0x%X", errorHandler);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_ErrorHandler", "Function", valueString);
}

/*____________________________________________________________________________*/

/**
 * Converts the value at the specified key index to a string.
 *
 * @param object    Object to inspect.
 * @param keyIndex  Index of key to convert.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 *
 * @return          Number of cells written.
 */
stock ObjLib_ValueToString(Object:object, keyIndex, String:buffer[], maxlen)
{
    decl String:valueString[OBJLIB_MAX_STRING_LEN];
    valueString[0] = 0;
    
    // Get type.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Get data type.
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    new ObjectDataType:dataType = ObjectDataType:GetArrayCell(dataTypes, keyIndex);
    
    // Get data values.
    new Handle:data = ObjLib_GetObjectData(object);
    
    switch (dataType)
    {
        case ObjDataType_Any, ObjDataType_Cell:
        {
            new value = GetArrayCell(data, keyIndex);
            return IntToString(value, buffer, maxlen);
        }
        case ObjDataType_Bool:
        {
            new bool:value = bool:GetArrayCell(data, keyIndex);
            if (value)
            {
                return strcopy(buffer, maxlen, "true");
            }
            else
            {
                return strcopy(buffer, maxlen, "false");
            }
        }
        case ObjDataType_Float:
        {
            new Float:value = Float:GetArrayCell(data, keyIndex);
            return FloatToString(value, buffer, maxlen);
        }
        case ObjDataType_Handle, ObjDataType_Function, ObjDataType_Object, ObjDataType_ObjectType:
        {
            new value = GetArrayCell(data, keyIndex);
            return Format(buffer, maxlen, "0x%X", value);
        }
        case ObjDataType_Array:
        {
            new blockSize = ObjLib_GetTypeBlockSize(typeDescriptor);
            new values[blockSize];
            GetArrayArray(data, keyIndex, values, blockSize);
            return Array_CellsToString(values, blockSize, buffer, maxlen, false);
        }
        case ObjDataType_String:
        {
            return GetArrayString(data, keyIndex, buffer, maxlen);
        }
        default:
        {
            ThrowError("[BUG] Unexpected type. This is a bug in objectlib.");
        }
    }
    
    return 0;
}
