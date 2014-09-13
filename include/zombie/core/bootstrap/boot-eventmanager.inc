/*
 * ============================================================================
 *
 *  Zombie API
 *
 *  File:           boot-eventmanager.inc
 *  Description:    Event manager bootstrapper.
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

Create the following functions in your plugin. These will be called by this
bootstrapper when the event manager is added or removed:

    ZM_OnEventManagerAdded
    ZM_OnEventManagerRemoved

Call the following functions. The last part of their names tell where you should
call them from:

    ZM_EventMgr_AskPluginLoad2
    ZM_EventMgr_OnAllPluginsLoaded
    ZM_EventMgr_OnPluginEnd
    ZM_EventMgr_OnLibraryAdded
    ZM_EventMgr_OnLibraryRemoved

*/

/*____________________________________________________________________________*/

new ZM_EventManagerLoaded = false;

/*____________________________________________________________________________*/

stock APLRes:ZM_EventMgr_AskPluginLoad2(Handle:myself, bool:late, String:error[], err_max)
{
    ZM_SetEventManagerOptional();
    return APLRes_Success;
}

/*____________________________________________________________________________*/

stock ZM_EventMgr_OnAllPluginsLoaded()
{
    if (LibraryExists(LIBRARY_ZM_EVENT_MANAGER))
    {
        ZM_EventMgr_OnEventManagerAdded();
    }
}

/*____________________________________________________________________________*/

stock ZM_EventMgr_OnPluginEnd()
{
    ZM_EventMgr_OnEventManagerRemoved();
}

/*____________________________________________________________________________*/

stock ZM_EventMgr_OnLibraryAdded(const String:name[])
{
    if (StrEqual(name, LIBRARY_ZM_EVENT_MANAGER))
    {
        ZM_EventMgr_OnEventManagerAdded();
    }
}

/*____________________________________________________________________________*/

stock ZM_EventMgr_OnLibraryRemoved(const String:name[])
{
    if (StrEqual(name, LIBRARY_ZM_EVENT_MANAGER))
    {
        ZM_EventMgr_OnEventManagerRemoved();
    }
}

/*____________________________________________________________________________*/

stock ZM_EventMgr_OnEventManagerAdded()
{
    if (ZM_EventManagerLoaded)
    {
        return;
    }
    
    ZM_EventManagerLoaded = true;
    LogMessage("Event manager available.");
    
    ZM_OnEventManagerAdded();
}

/*____________________________________________________________________________*/

stock ZM_EventMgr_OnEventManagerRemoved()
{
    if (!ZM_EventManagerLoaded)
    {
        return;
    }
    
    ZM_EventManagerLoaded = false;
    LogMessage("Event manager removed.");
    
    ZM_OnEventManagerRemoved();
}

/*____________________________________________________________________________*/

stock ZM_IsEventManagerLoaded()
{
    return ZM_EventManagerLoaded;
}
