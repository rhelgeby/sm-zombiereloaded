/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           module.inc
 *  Description:    Defines module data structure and accessors.
 *
 *  Copyright (C) 2014  Richard Helgeby
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

new ObjectType:ModuleType = INVALID_OBJECT_TYPE;

#define MODULE_STRING_LEN   255
#define MODULE_KEY_LEN      16

/*____________________________________________________________________________*/

InitModuleType()
{
    if (ModuleType != INVALID_OBJECT_TYPE)
    {
        // Already defined.
        return;
    }
    
    new blockSize = ByteCountToCells(MODULE_STRING_LEN);
    new keySize = ByteCountToCells(MODULE_KEY_LEN);
    
    ModuleType = ObjLib_CreateType(blockSize, keySize);
    
    ObjLib_AddKey(ModuleType, "plugin", ObjDataType_Handle);
    ObjLib_AddKey(ModuleType, "name", ObjDataType_String);
    ObjLib_AddKey(ModuleType, "description", ObjDataType_String);
}

/*____________________________________________________________________________*/

ZMModule:CreateModule(Handle:plugin, const String:name[])
{
    if (plugin == INVALID_HANDLE)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Plugin not specified.");
    }
    
    if (PluginHasModule(plugin))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "A module for this plugin already exists. Only one module per plugin is allowed.");
    }
    
    if (strlen(name) == 0)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Name is empty.");
    }
    
    InitModuleType();
    
    new Object:module = ObjLib_CreateObject(ModuleType);
    ObjLib_SetHandle(module, "plugin", plugin);
    ObjLib_SetString(module, "name", name);
    
    LogMessage("Created module \"%s\": %x", name, module);
    
    return ZMModule:module;
}

/*____________________________________________________________________________*/

DeleteModule(ZMModule:module)
{   
    if (module == INVALID_ZM_MODULE)
    {
        ThrowError("Invalid module.");
    }
    
    ObjLib_DeleteObject(Object:module, false);
    
    LogMessage("Deleted module: %x", module);
}

/*____________________________________________________________________________*/

Handle:GetModulePlugin(ZMModule:module)
{
    return ObjLib_GetHandle(Object:module, "plugin");
}

/*____________________________________________________________________________*/

GetModuleName(ZMModule:module, String:buffer[], maxlen)
{
    return ObjLib_GetString(Object:module, "name", buffer, maxlen);
}

/*____________________________________________________________________________*/

GetModuleDescription(ZMModule:module, String:buffer[], maxlen)
{
    return ObjLib_GetString(Object:module, "description", buffer, maxlen);
}

/*____________________________________________________________________________*/

SetModuleDescription(ZMModule:module, const String:description[])
{
    return ObjLib_SetString(Object:module, "description", description);
}

/*____________________________________________________________________________*/

bool:IsValidModule(ZMModule:module)
{
    return module != INVALID_ZM_MODULE;
}
