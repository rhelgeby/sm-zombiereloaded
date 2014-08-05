/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           boot.inc
 *  Description:    Boot procedures for the event manager.
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
    LogMessage("Loading event manager.");
    
    ZM_ModuleMgr_AskPluginLoad2(myself, late, error, err_max);
    
    InitAPI();
    RegPluginLibrary(LIBRARY_ZM_EVENT_MANAGER);
    
    return APLRes_Success;
}

/*____________________________________________________________________________*/

public OnPluginStart()
{
    InitializeDataStorage();
    
    LogMessage("Event manager loaded.");
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

LoadModule()
{
    EventManagerModule = ZM_CreateModule(MODULE_ZM_EVENT_MANAGER);
    
    CreatePredefinedEvents();
    EventManagerReady = true;
}

/*____________________________________________________________________________*/

UnloadModule()
{
    ZM_DeleteModule();
    EventManagerModule = INVALID_ZM_MODULE;
    
    DeletePredefinedEvents();
    EventManagerReady = false;
}

/*____________________________________________________________________________*/

InitializeDataStorage()
{
    if (EventList == INVALID_HANDLE)
    {
        EventList = CreateArray();
    }
    
    if (EventNameIndex == INVALID_HANDLE)
    {
        EventNameIndex = CreateTrie();
    }
}

/*____________________________________________________________________________*/

CreatePredefinedEvents()
{
    ForwardOnEventManagerReady = CreateForward(ET_Ignore);
    EventOnEventManagerReady = AddZMEvent(EventManagerModule, ZM_ON_EVENTMANAGER_READY, ForwardOnEventManagerReady);
    
    ForwardOnEventManagerDisable = CreateForward(ET_Ignore);
    EventOnEventManagerDisable = AddZMEvent(EventManagerModule, ZM_ON_EVENTMANAGER_DISABLE, ForwardOnEventManagerDisable);
    
    ForwardOnEventsCreate = CreateForward(ET_Ignore);
    EventOnEventsCreate = AddZMEvent(EventManagerModule, ZM_ON_EVENTS_CREATE, ForwardOnEventsCreate);
    
    ForwardOnEventsCreated = CreateForward(ET_Ignore);
    EventOnEventsCreated = AddZMEvent(EventManagerModule, ZM_ON_EVENTS_CREATED, ForwardOnEventsCreated);
}

/*____________________________________________________________________________*/

DeletePredefinedEvents()
{
    // Remove events and forwards. Forwards are removed by RemoveZMEvent, we
    // just need to reset the handle.
    
    RemoveZMEvent(EventOnEventManagerReady);
    ForwardOnEventManagerReady = INVALID_HANDLE;
    
    RemoveZMEvent(EventOnEventManagerDisable);
    ForwardOnEventManagerDisable = INVALID_HANDLE;
    
    RemoveZMEvent(EventOnEventsCreate);
    ForwardOnEventsCreate = INVALID_HANDLE;
    
    RemoveZMEvent(EventOnEventsCreated);
    ForwardOnEventsCreated = INVALID_HANDLE;
}
