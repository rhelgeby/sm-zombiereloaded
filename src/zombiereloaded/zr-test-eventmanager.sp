/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           zr-test-eventmanager.sp
 *  Description:    Simple test plugin for the Event Manager API.
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
#include <zombie/core/eventmanager>
#include <zombie/core/bootstrap/boot-core>

#include "zombiereloaded/common/version"

/*____________________________________________________________________________*/

#define PLUGIN_NAME         "Zombie:Reloaded Event Manager Tester"
#define PLUGIN_DESCRIPTION  "Simple test plugin for the Event Manager."

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

new ZMEvent:EventManagerReady = INVALID_ZM_EVENT;
new ZMEvent:EventManagerDisable = INVALID_ZM_EVENT;
new ZMEvent:EventsCreate = INVALID_ZM_EVENT;
new ZMEvent:EventsCreated = INVALID_ZM_EVENT;

/*____________________________________________________________________________*/

public APLRes:AskPluginLoad2(Handle:myself, bool:late, String:error[], err_max)
{
    ZM_BootCore_AskPluginLoad2(myself, late, error, err_max);
    return APLRes_Success;
}

/*____________________________________________________________________________*/

public OnAllPluginsLoaded()
{
    ZM_BootCore_OnAllPluginsLoaded();
}

/*____________________________________________________________________________*/

public OnPluginEnd()
{
    ZM_BootCore_OnPluginEnd();
}

/*____________________________________________________________________________*/

public OnLibraryAdded(const String:name[])
{
    ZM_BootCore_OnLibraryAdded(name);
}

/*____________________________________________________________________________*/

public OnLibraryRemoved(const String:name[])
{
    ZM_BootCore_OnLibraryRemoved(name);
}

/*____________________________________________________________________________*/

ZM_OnCoreLoaded()
{
    TestModule = ZM_CreateModule("zr_test_eventmanager");
    LogMessage("Registered module: %x", TestModule);
    
    EventManagerReady = ZM_HookEventManagerReady(OnEventManagerReady);
    EventManagerDisable = ZM_HookEventManagerDisable(OnEventManagerDisable);
}

/*____________________________________________________________________________*/

ZM_OnCoreUnloaded()
{
    ZM_DeleteModule();
    TestModule = INVALID_ZM_MODULE;
    
    LogMessage("Deleted module.");
}

/*____________________________________________________________________________*/

public OnEventManagerReady()
{
    LogMessage("OnEventManagerReady");
    
    EventsCreate = ZM_HookEventsCreate(OnEventsCreate);
    EventsCreated = ZM_HookEventsCreated(OnEventsCreated);
}

/*____________________________________________________________________________*/

public OnEventManagerDisable()
{
    LogMessage("OnEventManagerDisable");
    
    ZM_UnhookEvent(EventsCreate, OnEventsCreate);
    ZM_UnhookEvent(EventsCreated, OnEventsCreated);
    ZM_UnhookEvent(EventManagerReady, OnEventManagerReady);
    ZM_UnhookEvent(EventManagerDisable, OnEventManagerDisable);
}

/*____________________________________________________________________________*/

public OnEventsCreate()
{
    LogMessage("OnEventsCreate");
}

/*____________________________________________________________________________*/

public OnEventsCreated()
{
    LogMessage("OnEventsCreated");
}
