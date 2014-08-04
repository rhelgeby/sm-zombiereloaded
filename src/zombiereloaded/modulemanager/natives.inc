/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           natives.inc
 *  Description:    Implementation of natives.
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

InitAPI()
{
    PrintToServer("Initializing module manager API.");
    
    CreateNative("ZM_CreateModule", API_CreateModule);
    CreateNative("ZM_DeleteModule", API_DeleteModule);
    CreateNative("ZM_GetModuleByPlugin", API_GetModuleByPlugin);
    CreateNative("ZM_GetModuleOwner", API_GetModuleOwner);
    CreateNative("ZM_GetModuleByName", API_GetModuleByName);
    CreateNative("ZM_GetModuleDescription", API_GetModuleDescription);
    CreateNative("ZM_SetModuleDescription", API_SetModuleDescription);
    CreateNative("ZM_CreateFeature", API_CreateFeature);
    CreateNative("ZM_DeleteFeature", API_DeleteFeature);
    CreateNative("ZM_GetFeature", API_GetFeature);
    CreateNative("ZM_GetFeatureOwner", API_GetFeatureOwner);
    CreateNative("ZM_HookFeatureAdded", API_HookFeatureAdded);
    CreateNative("ZM_HookFeatureRemove", API_HookFeatureRemove);
    
    // TODO: Register events in event manager.
}

/*____________________________________________________________________________*/

/**
 * Creates a new module. The new module is bound to the calling plugin.
 *
 * Warning: You must call ZM_DeleteModule when the module is no longer in use,
 *          such as in OnPluginEnd. Otherwise the module manager will never know
 *          when the module is removed.
 *
 * Note: A plugin is only allowed to create one module. The module will remain
 *       registered until the plugin is unloaded.
 *
 * @param name          Unique module name. Used by lookup functions.
 *
 * @return              Module ID.
 * @error               Plugin has already created a module, name is already in
 *                      use or name is empty.
 */
//native ZMModule:ZM_CreateModule(const String:name[]);
public API_CreateModule(Handle:plugin, numParams)
{
    new String:name[MODULE_STRING_LEN];
    GetNativeString(1, name, sizeof(name));
    
    if (!AssertPluginHasNoModule(plugin)
        || !AssertModuleNameNotExists(name))
    {
        return 0;
    }
    
    new ZMModule:module = AddModule(plugin, name);
    return _:module;
}

/*____________________________________________________________________________*/

/**
 * Deletes the module and notifies dependents.
 *
 * Call this function from OnPluginEnd or when the module is no longer in use.
 *
 * @noreturn
 * @error               Caller has not created a module.
 */
//native ZM_DeleteModule();
public API_DeleteModule(Handle:plugin, numParams)
{
    new ZMModule:module = GetModuleByPlugin(plugin);
    LogMessage("API_DeleteModule: module=%x | plugin=%x", module, plugin);
    
    if (!AssertPluginHasZMModule(module))
    {
        return;
    }
    
    RemoveModule(module);
}

/*____________________________________________________________________________*/

/**
 * Looks up a module by the specified plugin handle value.
 *
 * @param plugin        Handle to plugin representing a module.
 *
 * @return              Module ID, or INVALID_ZM_MODULE if not found.
 */
//native ZMModule:ZM_GetModuleByPlugin(Handle:plugin);
public API_GetModuleByPlugin(Handle:plugin, numParams)
{
    new Handle:pluginToLookup = GetNativeCell(1);
    
    return _:GetModuleByPlugin(pluginToLookup);
}

/*____________________________________________________________________________*/

/**
 * Gets the plugin that created the specified module.
 *
 * @param module        Module to look up.
 *
 * @return              Handle to owner plugin, or INVALID_ZM_MODULE if not
 *                      found.
 */
//native Handle:ZM_GetModuleOwner(ZMModule:module);
public API_GetModuleOwner(Handle:plugin, numParams)
{
    new ZMModule:module = GetNativeCell(1);
    
    if (!AssertIsValidZMModule(module))
    {
        return 0;
    }
    
    return _:GetModulePlugin(module);
}

/*____________________________________________________________________________*/

/**
 * Looks up a module by the specified name.
 *
 * @param name          Name to search for.
 *
 * @return              Module ID, or INVALID_ZM_MODULE if not found.
 */
//native ZMModule:ZM_GetModuleByName(const String:name[]);
public API_GetModuleByName(Handle:plugin, numParams)
{
    new String:name[MODULE_STRING_LEN];
    GetNativeString(1, name, sizeof(name));
    
    return _:GetModuleByName(name);
}

/*____________________________________________________________________________*/

/**
 * Gets the module description.
 *
 * @param module        Module to read from.
 * @param buffer        Output buffer.
 * @param maxlen        Size of output buffer.
 *
 * @return              Number of characters written.
 */
//native ZM_GetModuleDescription(ZMModule:module, String:buffer[], maxlen);
public API_GetModuleDescription(Handle:plugin, numParams)
{
    new ZMModule:module = GetNativeCell(1);
    
    if (!AssertIsValidZMModule(module))
    {
        return 0;
    }
    
    new String:buffer[MODULE_STRING_LEN];
    new maxlen = GetNativeCell(3);
    
    GetModuleDescription(module, buffer, sizeof(buffer));
    
    new bytes = 0;
    SetNativeString(2, buffer, maxlen, _, bytes);
    
    return bytes;
}

/*____________________________________________________________________________*/

/**
 * Sets the module brief description. This may be disabled by administration
 * commands.
 *
 * @param description   Description to set.
 *
 * @noreturn
 * @error               Caller has not created a module.
 */
//native ZM_SetModuleDescription(const String:description[]);
public API_SetModuleDescription(Handle:plugin, numParams)
{
    new ZMModule:module = GetModuleByPlugin(plugin);
    if (!AssertPluginHasZMModule(module))
    {
        return;
    }
    
    new String:description[MODULE_STRING_LEN];
    GetNativeString(1, description, sizeof(description));
    
    SetModuleDescription(module, description);
}

/*____________________________________________________________________________*/

/**
 * Declares a feature identifier and assigns it to the caller module.
 *
 * Note: This native will also trigger the OnFeatureAdded event, if the event
 *       manager is initialized. Modules should consider using ZM_FeatureExists
 *       if the event manager is unavailable (as in the bootstrap phase).
 *
 * @param identifier    Global and unique name of the feature. Other modules
 *                      may declare dependency on this feature.
 *
 * @return              Feature ID.
 * @error               Name is blank or already in use. Caller has not created
 *                      a module.
 */
//native ZMFeature:ZM_CreateFeature(const String:identifier[]);
public API_CreateFeature(Handle:plugin, numParams)
{
    new String:identifier[MODULE_STRING_LEN];
    GetNativeString(1, identifier, sizeof(identifier));
    
    if (!AssertFeatureNameNotExists(identifier))
    {
        return 0;
    }
    
    new ZMModule:module = GetModuleByPluginOrFail(plugin);
    new ZMFeature:feature = AddFeature(module, identifier);
    
    return _:feature;
}

/*____________________________________________________________________________*/

/**
 * Deletes and unregisters a feature in the caller module.
 *
 * Note: This native will also trigger the OnFeatureRemove event, if the event
 *       manager is initialized.
 *
 * @param feature       Feature to delete.
 *
 * @error               Invalid feature, or caller is not the feature owner.
 */
//native ZM_DeleteFeature(ZMFeature:feature);
public API_DeleteFeature(Handle:plugin, numParams)
{
    new ZMFeature:feature = GetNativeCell(1);
    
    if (!AssertFeatureExists(feature)
        || !AssertIsFeatureOwner(plugin, feature))
    {
        return;
    }
    
    RemoveFeature(feature);
}

/*____________________________________________________________________________*/

/**
 * Gets a feature.
 *
 * @param identifier    Feature identifier string.
 *
 * @return              Feature ID if found, otherwise INVALID_ZM_FEATURE.
 */
//native ZMFeature:ZM_GetFeature(const String:identifier[]);
public API_GetFeature(Handle:plugin, numParams)
{
    new String:identifier[MODULE_STRING_LEN];
    GetNativeString(1, identifier, sizeof(identifier));
    
    new ZMFeature:feature = GetFeatureByName(identifier);
    return _:feature;
}

/*____________________________________________________________________________*/

/**
 * Gets the owner module of the specified feature.
 *
 * @parma feature       Feature to look up.
 *
 * @return              Module ID.
 * @error               Invalid feature.
 */
//native ZMModule:ZM_GetFeatureOwner(ZMFeature:feature);
public API_GetFeatureOwner(Handle:plugin, numParams)
{
    new ZMFeature:feature = ZMFeature:GetNativeCell(1);
    if (!AssertFeatureExists(feature))
    {
        return 0;
    }
    
    new ZMModule:featureOwner = GetFeatureOwner(feature);
    return _:featureOwner;
}

/*____________________________________________________________________________*/

/**
 * Registers an event hook for the OnFeatureAdded event.
 *
 * @param callback      Function to call when event is fired.
 *
 * @return              Event ID.
 * @error               Event is already hooked by the module, or event manager
 *                      is not initialized.
 */
//native ZMEvent:ZM_HookFeatureAdded(ZM_OnFeatureAdded:callback);
public API_HookFeatureAdded(Handle:plugin, numParams)
{
    ThrowNativeError(SP_ERROR_ABORTED, "Not implemented.");
}

/*____________________________________________________________________________*/

/**
 * Registers an event hook for the OnFeatureRemove event.
 *
 * @param callback      Function to call when event is fired.
 *
 * @return              Event ID.
 * @error               Event is already hooked by the module, or event manager
 *                      is not initialized.
 */
//native ZMEvent:ZM_HookFeatureRemove(ZM_OnFeatureRemove:callback);
public API_HookFeatureRemove(Handle:plugin, numParams)
{
    ThrowNativeError(SP_ERROR_ABORTED, "Not implemented.");
}
