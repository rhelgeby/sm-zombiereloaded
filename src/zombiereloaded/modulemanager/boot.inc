/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           boot.inc
 *  Description:    Boot procedures for the module manager.
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

public APLRes:AskPluginLoad2(Handle:myself, bool:late, String:error[], err_max)
{
    LogMessage("Loading module manager.");
    
    if (LibraryExists(LIBRARY_ZM_MODULE_MANAGER))
    {
        Format(error, err_max, "Another ZM module manager is already loaded.");
        return APLRes_Failure;
    }
    
    ZM_DepMgr_AskPluginLoad2();
    
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

public OnAllPluginsLoaded()
{
    ZM_DepMgr_OnAllPluginsLoaded();
}

/*____________________________________________________________________________*/

public OnPluginEnd()
{
    ZM_DepMgr_OnPluginEnd();
    LogMessage("Module manager unloaded.");
}

/*____________________________________________________________________________*/

public OnLibraryAdded(const String:name[])
{
    ZM_DepMgr_OnLibraryAdded(name);
}

/*____________________________________________________________________________*/

public OnLibraryRemoved(const String:name[])
{
    ZM_DepMgr_OnLibraryRemoved(name);
}

/*____________________________________________________________________________*/

ZM_OnDependencyManagerReady()
{
    ZM_AddLibrary(LIBRARY_ZM_MODULE_MANAGER);
}
