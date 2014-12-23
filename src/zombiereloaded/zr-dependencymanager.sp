/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           zr-dependencymanager.sp
 *  Description:    Implements the Dependency Manager API.
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
#include <zombie/core/dependencymanager>

#include "zombiereloaded/common/version"

/*____________________________________________________________________________*/

#define PLUGIN_NAME         "Zombie:Reloaded Dependency Manager"
#define PLUGIN_DESCRIPTION  "Implements the Dependency Manager API."

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
 * Hash map of library names mapped to library objects.
 */
new Handle:Libraries = INVALID_HANDLE;

/**
 * Hash map of plugin IDs mapped to dependent objects.
 */
new Handle:Dependents = INVALID_HANDLE;

/*____________________________________________________________________________*/

#include "zombiereloaded/libraries/objectlib"
#include "zombiereloaded/dependencymanager/library"
#include "zombiereloaded/dependencymanager/dependent"
#include "zombiereloaded/dependencymanager/natives"

/*____________________________________________________________________________*/

GetHexString(any:value, String:buffer[], maxlen)
{
    Format(buffer, maxlen, "%x", value);
}

/*____________________________________________________________________________*/

public APLRes:AskPluginLoad2(Handle:myself, bool:late, String:error[], err_max)
{
    LogMessage("Loading dependency manager.");
    
    if (LibraryExists(LIBRARY_ZM_DEPENDENCY_MANAGER))
    {
        Format(error, err_max, "Another ZM dependency manager is already loaded.");
        return APLRes_Failure;
    }
    
    InitAPI();
    RegPluginLibrary(LIBRARY_ZM_DEPENDENCY_MANAGER);
    
    return APLRes_Success;
}

/*____________________________________________________________________________*/

public OnPluginStart()
{
    InitializeDataStorage();
}

/*____________________________________________________________________________*/

InitializeDataStorage()
{
    if (Libraries == INVALID_HANDLE)
    {
        Libraries = CreateTrie();
    }
    
    if (Dependents == INVALID_HANDLE)
    {
        Dependents = CreateTrie();
    }
}

/*____________________________________________________________________________*/

Dependent:GetDependent(Handle:plugin)
{
    new String:pluginId[16];
    GetHexString(plugin, pluginId, sizeof(pluginId));
    
    new Dependent:dependent = INVALID_DEPENDENT;
    GetTrieValue(Dependents, pluginId, dependent);
    return dependent;
}

/*____________________________________________________________________________*/

SetDependent(const String:pluginId[], Dependent:dependent)
{
    SetTrieValue(Dependents, pluginId, dependent);
}

/*____________________________________________________________________________*/

Dependent:InitializeDependent(Handle:plugin)
{
    new Dependent:dependent = CreateDependent();
    
    new String:pluginId[16];
    GetHexString(plugin, pluginId, sizeof(pluginId));
    
    SetDependent(pluginId, dependent);
    
    return dependent;
}

/*____________________________________________________________________________*/

Dependent:GetOrCreateDependent(Handle:plugin)
{
    new Dependent:dependent = GetDependent(plugin);
    if (dependent != INVALID_DEPENDENT)
    {
        // Already created, return existing dependent.
        return dependent;
    }
    
    // Create a new dependent.
    return InitializeDependent(plugin);
}

/*____________________________________________________________________________*/

SetDependencyCallbacks(
        Handle:plugin,
        ZM_OnDependenciesReady:ready,
        ZM_OnDependenciesUnavailable:unavailable)
{
    if (!AssertValidReadyCallback(ready)
        || AssertValidUnavailableCallback(unavailable))
    {
        return;
    }
    
    new Dependent:dependent = GetOrCreateDependent(plugin);
    SetCallbacks(dependent, ready, unavailable);
}

/*____________________________________________________________________________*/

bool:AssertValidReadyCallback(ZM_OnDependenciesReady:ready)
{
    if (ready == INVALID_FUNCTION)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Invalid ready-callback: %x", ready);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertValidUnavailableCallback(ZM_OnDependenciesUnavailable:unavailable)
{
    if (unavailable == INVALID_FUNCTION)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Invalid unavailable-callback: %x", unavailable);
        return false;
    }
    
    return true;
}
