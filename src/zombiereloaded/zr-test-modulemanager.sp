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

new ModuleManagerLoaded = false;
new ZMModule:TestModule = INVALID_ZM_MODULE;

/*____________________________________________________________________________*/

public APLRes:AskPluginLoad2(Handle:myself, bool:late, String:error[], err_max)
{
    ZM_SetModuleManagerOptional();
    return APLRes_Success;
}

/*____________________________________________________________________________*/

public OnAllPluginsLoaded()
{
    if (LibraryExists(LIBRARY_ZM_MODULE_MANAGER))
    {
        OnModuleManagerAdded();
    }

/*____________________________________________________________________________*/

public OnPluginEnd()
{
    OnModuleManagerRemoved();
}

/*____________________________________________________________________________*/

public OnLibraryAdded(const String:name[])
{
    if (StrEqual(name, LIBRARY_ZM_MODULE_MANAGER))
    {
        OnModuleManagerAdded();
    }
}

/*____________________________________________________________________________*/

public OnLibraryRemoved(const String:name[])
{
    if (StrEqual(name, LIBRARY_ZM_MODULE_MANAGER))
    {
        OnModuleManagerRemoved();
    }
}

/*____________________________________________________________________________*/

OnModuleManagerAdded()
{
    if (ModuleManagerLoaded)
    {
        return;
    }
    
    ModuleManagerLoaded = true;
    LogMessage("Module manager available.");
    
    InitializeModule();
}

/*____________________________________________________________________________*/

OnModuleManagerRemoved()
{
    if (!ModuleManagerLoaded)
    {
        return;
    }
    
    ModuleManagerLoaded = false;
    LogMessage("Module manager removed.");
    
    DeleteModule();
}

/*____________________________________________________________________________*/

InitializeModule()
{
    TestModule = ZM_CreateModule("zr_test_modulemanager");
    LogMessage("Registered module: %x", TestModule);
}

/*____________________________________________________________________________*/

DeleteModule()
{
    ZM_DeleteModule();
    TestModule = INVALID_ZM_MODULE;
    
    LogMessage("Deleted module.");
}
