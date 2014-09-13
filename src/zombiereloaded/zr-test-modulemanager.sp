/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           zr-test-modulemanager.sp
 *  Description:    Simple test plugin for the Module Manager API.
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
#include <zombie/core/bootstrap/boot-modulemanager>

#include "zombiereloaded/common/version"

/*____________________________________________________________________________*/

#define PLUGIN_NAME         "Zombie:Reloaded Module Manager Tester"
#define PLUGIN_DESCRIPTION  "Simple test plugin for the Module Manager."

public Plugin:myinfo =
{
    name = PLUGIN_NAME,
    author = ZOMBIERELOADED_AUTHORS,
    description = PLUGIN_DESCRIPTION,
    version = ZOMBIERELOADED_VERSION,
    url = ZOMBIERELOADED_URL
};

/*____________________________________________________________________________*/

new ZMModule:TestModule = INVALID_ZM_MODULE;

/*____________________________________________________________________________*/

public APLRes:AskPluginLoad2(Handle:myself, bool:late, String:error[], err_max)
{
    ZM_ModuleMgr_AskPluginLoad2(myself, late, error, err_max);
    return APLRes_Success;
}

/*____________________________________________________________________________*/

public OnAllPluginsLoaded()
{
    ZM_ModuleMgr_OnAllPluginsLoaded();
}

/*____________________________________________________________________________*/

public OnPluginEnd()
{
    ZM_ModuleMgr_OnPluginEnd();
}

/*____________________________________________________________________________*/

public OnLibraryAdded(const String:name[])
{
    ZM_ModuleMgr_OnLibraryAdded(name);
}

/*____________________________________________________________________________*/

public OnLibraryRemoved(const String:name[])
{
    ZM_ModuleMgr_OnLibraryRemoved(name);
}

/*____________________________________________________________________________*/

ZM_OnModuleManagerAdded()
{
    TestModule = ZM_CreateModule("zr_test_modulemanager");
    LogMessage("Registered module: %x", TestModule);
}

/*____________________________________________________________________________*/

ZM_OnModuleManagerRemoved()
{
    ZM_DeleteModule();
    TestModule = INVALID_ZM_MODULE;
    
    LogMessage("Deleted module.");
}
