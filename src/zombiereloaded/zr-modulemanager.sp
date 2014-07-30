/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           zr-modulemanager.sp
 *  Description:    Implements the Module Manager API for registering and
 *                  managing modules.
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

#include <sourcemod>
#include <zombie/core/modulemanager>

#include "zombiereloaded/common/version"

#include "zombiereloaded/libraries/objectlib"
#include "zombiereloaded/modulemanager/module"
#include "zombiereloaded/modulemanager/feature"
#include "zombiereloaded/modulemanager/natives"

/*____________________________________________________________________________*/

#define PLUGIN_NAME         "Zombie:Reloaded Module Manager"
#define PLUGIN_DESCRIPTION  "Implements the Module Manager API."

public Plugin:myinfo =
{
    name = PLUGIN_NAME,
    author = ZOMBIERELOADED_AUTHORS,
    description = PLUGIN_DESCRIPTION,
    version = ZOMBIERELOADED_VERSION,
    url = ZOMBIERELOADED_URL
};

/*____________________________________________________________________________*/

/**
 * ADT Array with references to all modules.
 */
new Handle:ModuleList = INVALID_HANDLE;

/**
 * ADT Trie with mappings of module names to module IDs.
 */
new Handle:ModuleNameIndex = INVALID_HANDLE;

/**
 * ADT Trie with mappings of plugin handles to module IDs.
 */
new Handle:ModulePluginIndex = INVALID_HANDLE;

/**
 * ADT Array with references to all features.
 */
new Handle:FeatureList = INVALID_HANDLE;

/**
 * ADT Trie with mappings of feature names to feature IDs.
 */
new Handle:FeatureNameIndex = INVALID_HANDLE;

/*____________________________________________________________________________*/

public APLRes:AskPluginLoad2(Handle:myself, bool:late, String:error[], err_max)
{
    LogMessage("Loading module manager.");
    
    if (LibraryExists(LIBRARY_ZM_MODULE_MANAGER))
    {
        Format(error, err_max, "Another ZM module manager is already loaded.");
        return APLRes_Failure;
    }
    
    InitAPI();
    RegPluginLibrary(LIBRARY_ZM_MODULE_MANAGER);
    
    return APLRes_Success;
}

/*____________________________________________________________________________*/

public OnPluginStart()
{
    InitializeDataStorage();
    LogMessage("Module manager loaded.");
}

/*____________________________________________________________________________*/

public OnPluginEnd()
{
    LogMessage("Module manager unloaded.");
}

/*____________________________________________________________________________*/

GetHexString(any:value, String:buffer[], maxlen)
{
    Format(buffer, maxlen, "%x", value);
}

/*____________________________________________________________________________*/

ZMModule:GetModuleByPlugin(Handle:plugin)
{
    new String:key[16];
    GetHexString(plugin, key, sizeof(key));

    new ZMModule:module = INVALID_ZM_MODULE;
    if (GetTrieValue(ModulePluginIndex, key, module))
    {
        return module;
    }
    
    return INVALID_ZM_MODULE;
}

/*____________________________________________________________________________*/

ZMModule:GetModuleByName(const String:name[])
{
    new ZMModule:module = INVALID_ZM_MODULE;
    if (GetTrieValue(ModuleNameIndex, name, module))
    {
        return module;
    }
    
    return INVALID_ZM_MODULE;
}

/*____________________________________________________________________________*/

bool:PluginHasModule(Handle:plugin)
{
    return GetModuleByPlugin(plugin) != INVALID_ZM_MODULE;
}

/*____________________________________________________________________________*/

ZMFeature:GetFeatureByName(const String:name[])
{
    new ZMFeature:feature = INVALID_ZM_FEATURE;
    if (GetTrieValue(FeatureNameIndex, name, feature))
    {
        return feature;
    }
    
    return INVALID_ZM_FEATURE;
}

/*____________________________________________________________________________*/

bool:FeatureExists(const String:name[])
{
    new ZMFeature:feature = GetFeatureByName(name);
    return IsValidFeature(feature);
}

/*____________________________________________________________________________*/

bool:IsFeatureOwner(Handle:plugin, ZMFeature:feature)
{
    new ZMModule:module = GetModuleByPlugin(plugin);
    new ZMModule:featureOwner = GetFeatureOwner(feature);
    
    return module == featureOwner;
}

/*____________________________________________________________________________*/

InitializeDataStorage()
{
    if (ModuleList == INVALID_HANDLE)
    {
        ModuleList = CreateArray();
    }

    if (ModuleNameIndex == INVALID_HANDLE)
    {
        ModuleNameIndex = CreateTrie();
    }

    if (ModulePluginIndex == INVALID_HANDLE)
    {
        ModulePluginIndex = CreateTrie();
    }

    if (FeatureList == INVALID_HANDLE)
    {
        FeatureList = CreateArray();
    }
    
    if (FeatureNameIndex == INVALID_HANDLE)
    {
        FeatureNameIndex = CreateTrie();
    }
}

/*____________________________________________________________________________*/

ZMModule:AddModule(Handle:ownerPlugin, const String:moduleName[])
{
    new ZMModule:module = CreateModule(ownerPlugin, moduleName);
    
    AddModuleToList(module);
    AddModuleToIndex(module);
    
    return module;
}

/*____________________________________________________________________________*/

RemoveModule(ZMModule:module)
{
    RemoveModuleFeatures(module);
    RemoveModuleFromList(module);
    RemoveModuleFromIndex(module);
    
    DeleteModule(module);
}

/*____________________________________________________________________________*/

AddModuleToList(ZMModule:module)
{
    PushArrayCell(ModuleList, module);
}

/*____________________________________________________________________________*/

AddModuleToIndex(ZMModule:module)
{
    new String:name[MODULE_STRING_LEN];
    GetModuleName(module, name, sizeof(name));
    
    new Handle:plugin = GetModulePlugin(module);
    new String:pluginID[16];
    GetHexString(plugin, pluginID, sizeof(pluginID));
    
    PushArrayCell(ModuleList, module);
    
    SetTrieValue(ModuleNameIndex, name, module);
    SetTrieValue(ModulePluginIndex, pluginID, module);
    LogMessage("Adding module to index. pluginID=%s | module=%x", pluginID, module);
}

/*____________________________________________________________________________*/

RemoveModuleFromList(ZMModule:module)
{
    new index = FindValueInArray(ModuleList, module);
    if (index < 0)
    {
        ThrowError("Module is not in list.");
    }
    
    RemoveFromArray(ModuleList, index);
}

/*____________________________________________________________________________*/

RemoveModuleFromIndex(ZMModule:module)
{
    new String:name[MODULE_STRING_LEN];
    GetModuleName(module, name, sizeof(name));
    
    new Handle:plugin = GetModulePlugin(module);    
    new String:pluginHex[16];
    GetHexString(plugin, pluginHex, sizeof(pluginHex));
    
    new moduleIndex = FindValueInArray(ModuleList, module);
    if (moduleIndex >= 0)
    {
        RemoveFromArray(ModuleList, moduleIndex);
    }
    
    RemoveFromTrie(ModuleNameIndex, name);
    RemoveFromTrie(ModulePluginIndex, pluginHex);
}

/*____________________________________________________________________________*/

ZMFeature:AddFeature(ZMModule:module, const String:name[])
{
    new ZMFeature:feature = CreateFeature(module, name);
    
    AddFeatureToList(feature);
    AddFeatureToIndex(feature);
    
    return feature;
}

/*____________________________________________________________________________*/

RemoveFeature(ZMFeature:feature)
{
    RemoveFeatureFromList(feature);
    RemoveFeatureFromIndex(feature);
    
    DeleteFeature(feature);
}

/*____________________________________________________________________________*/

AddFeatureToList(ZMFeature:feature)
{
    PushArrayCell(FeatureList, feature);
}

/*____________________________________________________________________________*/

RemoveFeatureFromList(ZMFeature:feature)
{
    new index = FindValueInArray(FeatureList, feature);
    if (index < 0)
    {
        ThrowError("Feature is not in list.");
    }
    
    RemoveFromArray(FeatureList, index);
}

/*____________________________________________________________________________*/

AddFeatureToIndex(ZMFeature:feature)
{
    decl String:name[FEATURE_STRING_LEN];
    name[0] = 0;
    GetFeatureName(feature, name, sizeof(name));
    
    SetTrieValue(FeatureNameIndex, name, feature);
}

/*____________________________________________________________________________*/

RemoveFeatureFromIndex(ZMFeature:feature)
{
    decl String:name[FEATURE_STRING_LEN];
    name[0] = 0;
    GetFeatureName(feature, name, sizeof(name));
    
    RemoveFromTrie(FeatureNameIndex, name);
}

/*____________________________________________________________________________*/

RemoveModuleFeatures(ZMModule:module)
{
    new Handle:featureList = GetModuleFeatures(module);
    
    new size = GetArraySize(featureList);
    for (new i = 0; i < size; i++)
    {
        new ZMFeature:feature = ZMFeature:GetArrayCell(featureList, i);
        RemoveFeature(feature);
    }
}

/*____________________________________________________________________________*/

Handle:GetModuleFeatures(ZMModule:module)
{
    new Handle:featureList = CreateArray();
    
    new numFeatures = GetArraySize(FeatureList);
    for (new i = 0; i < numFeatures; i++)
    {
        new ZMFeature:feature = GetArrayCell(FeatureList, i);
        new ZMModule:featureOwner = GetFeatureOwner(feature);
        
        if (featureOwner == module)
        {
            PushArrayCell(featureList, feature);
        }
    }
    
    return featureList;
}

/*____________________________________________________________________________*/

/**
 * Throws a native error if the plugin already has a module registered to it.
 */
bool:AssertPluginHasNoModule(Handle:plugin)
{
    new ZMModule:module = GetModuleByPlugin(plugin);
    if (IsValidModule(module))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "A module is already registered to this plugin.");
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

/**
 * Throws a native error if the plugin already has a module registered to it.
 */
bool:AssertPluginHasModule(ZMModule:module)
{
    if (!IsValidModule(module))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "No module is registered to this plugin.");
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertModuleNameNotExists(const String:moduleName[])
{
    new ZMModule:module = GetModuleByName(moduleName);
    if (IsValidModule(module))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Module name is already in use: %s", moduleName);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

ZMModule:GetModuleByPluginOrFail(Handle:plugin)
{
    new ZMModule:module = GetModuleByPlugin(plugin);
    if (!ZM_IsValidModule(module))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "No module is registered to this plugin.");
        return INVALID_ZM_MODULE;
    }
    
    return module;
}

/*____________________________________________________________________________*/

bool:AssertFeatureNameNotExists(const String:name[])
{
    new ZMFeature:feature = GetFeatureByName(name);
    if (IsValidFeature(feature))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Feature name is already in use: %s", name);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertFeatureExists(ZMFeature:feature)
{
    if (!IsValidFeature(feature))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Invalid feature: %x", feature);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertIsFeatureOwner(Handle:plugin, ZMFeature:feature)
{
    if (!IsFeatureOwner(plugin, feature))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "This plugin does not own the specified feature: %x", feature);
        return false;
    }
    
    return true;
}
