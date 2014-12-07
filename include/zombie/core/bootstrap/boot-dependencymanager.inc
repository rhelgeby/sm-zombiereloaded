/*
 * ============================================================================
 *
 *  Zombie API
 *
 *  File:           boot-dependencymanager.inc
 *  Description:    Dependency manager bootstrapper.
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

/*

USAGE

This bootstrapper will help you handle dependencies so your module can properly
load and unload dynamically.

You need to create this function in your plugin:

    ZM_OnDependencyManagerReady()

Inside ZM_OnDependencyManagerReady, do this:

    - Set up your callbacks using ZM_SetDependencyCallbacks. Use these callbacks
      as enable/disable functions for your plugin.
    - Declare your dependencies using ZM_DependsOn.

Then you also need to call the following functions. The last part of their names
tell where you should call them from:

    ZM_DepMgr_AskPluginLoad2()  - No parameters in this one, we don't need it.
    ZM_DepMgr_OnAllPluginsLoaded()
    ZM_DepMgr_OnPluginEnd()
    ZM_DepMgr_OnLibraryAdded(const String:name[])
    ZM_DepMgr_OnLibraryRemoved(const String:name[])

These functions will bootstrap the dependency manager and listen for changes.
You don't have to do anything else than forwarding the call.

Note: The bootstrapper doesn't use OnPluginStart, but OnAllPluginsLoaded. So you
      can use OnPluginStart to initialize your plugin as usual (at least parts
      that doesn't require the dependencies to be loaded). 


USAGE AS A LIBRARY

Use the bootstrapper as described above, but you don't have to declare
dependencies if you doesn't have any. In addition to that, you need to register
your library.

    - In ZM_OnDependencyManagerReady, add yourself to the dependency manager
      using ZM_AddLibrary.

*/

/*____________________________________________________________________________*/

new ZM_DependencyManagerLoaded = false;

/*____________________________________________________________________________*/

stock APLRes:ZM_DepMgr_AskPluginLoad2()
{
    ZM_SetDependencyManagerOptional();
    return APLRes_Success;
}

/*____________________________________________________________________________*/

stock ZM_DepMgr_OnAllPluginsLoaded()
{
    if (LibraryExists(LIBRARY_ZM_DEPENDENCY_MANAGER))
    {
        ZM_DepMgr_OnDepManagerAdded();
    }
}

/*____________________________________________________________________________*/

stock ZM_DepMgr_OnPluginEnd()
{
    // Clear this plugin's dependencies and libraries to prevent a memory leak
    // in the dependency manager.
    if (ZM_IsDependencyManagerLoaded())
    {
        ZM_RemoveDependencies();
        ZM_RemoveLibraries();
    }
}

/*____________________________________________________________________________*/

stock ZM_DepMgr_OnLibraryAdded(const String:name[])
{
    if (StrEqual(name, LIBRARY_ZM_DEPENDENCY_MANAGER))
    {
        ZM_DepMgr_OnDepManagerAdded();
    }
}

/*____________________________________________________________________________*/

stock ZM_DepMgr_OnLibraryRemoved(const String:name[])
{
    if (StrEqual(name, LIBRARY_ZM_DEPENDENCY_MANAGER))
    {
        ZM_DepMgr_OnDepManagerRemoved();
    }
}

/*____________________________________________________________________________*/

stock ZM_DepMgr_OnDepManagerAdded()
{
    if (ZM_IsDependencyManagerLoaded())
    {
        // Already loaded, ignore this event.
        return;
    }
    
    ZM_DependencyManagerLoaded = true;
    LogMessage("Dependency manager available.");

    // This callback is implemented by the client.
    ZM_OnDependencyManagerReady();
}

/*____________________________________________________________________________*/

stock ZM_DepMgr_OnDepManagerRemoved()
{
    if (!ZM_IsDependencyManagerLoaded())
    {
        // Already unloaded, ignore this event.
        return;
    }
    
    ZM_DependencyManagerLoaded = false;
    LogMessage("Dependency manager removed.");
    
    // Note: No need to cleanup dependencies and libraries here. The dependency
    //       manager is unloading and will clean all the allocated resources
    //       itself.
}

/*____________________________________________________________________________*/

stock ZM_IsDependencyManagerLoaded()
{
    return ZM_DependencyManagerLoaded;
}
