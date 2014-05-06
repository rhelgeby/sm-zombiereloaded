/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           modulemanager.sp
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

#include "zombiereloaded/libraries/objectlib"
#include "zombiereloaded/modulemanager/module"
#include "zombiereloaded/modulemanager/natives"

/*____________________________________________________________________________*/

#define PLUGIN_NAME         "Zombie:Reloaded Module Manager"
#define PLUGIN_AUTHOR       "Richard Helgeby"
#define PLUGIN_DESCRIPTION  "Implements the Module Manager API."
#define PLUGIN_VERSION      "1.0.0"
#define PLUGIN_URL          "https://github.com/rhelgeby/sm-zombiereloaded"

public Plugin:myinfo =
{
    name = PLUGIN_NAME,
    author = PLUGIN_AUTHOR,
    description = PLUGIN_DESCRIPTION,
    version = PLUGIN_VERSION,
    url = PLUGIN_URL
};

/*____________________________________________________________________________*/

#define MODULE_NAME_LENGTH          64
#define MODULE_NAME_DESCRIPTION     255

/*____________________________________________________________________________*/

/**
 * Defines the internal module data structure. Should not be exposed.
 */
enum ModuleData
{
    /** Handle to plugin that created the module. */
    Handle:Module_Plugin,
    
    String:Module_Name[MODULE_NAME_LENGTH],
    String:Module_Description[MODULE_NAME_DESCRIPTION]
}

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
 * ADT Trie with mappings of feature names to module IDs.
 */
new Handle:ModuleFeatureIndex = INVALID_HANDLE;

/*____________________________________________________________________________*/

public APLRes:AskPluginLoad2(Handle:myself, bool:late, String:error[], err_max)
{
    PrintToServer("Loading module manager.");
    
    InitAPI();
    return APLRes_Success;
}

/*____________________________________________________________________________*/

public OnPluginStart()
{
    InitializeDataStorage();
    PrintToServer("Module manager loaded.");
}

/*____________________________________________________________________________*/

public OnPluginEnd()
{
    PrintToServer("Module manager unloaded.");
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

    if (ModuleFeatureIndex == INVALID_HANDLE)
    {
        ModuleFeatureIndex = CreateTrie();
    }
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
