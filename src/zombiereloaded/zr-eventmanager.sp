/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           zr-eventmanager.sp
 *  Description:    Implements the Event Manager API.
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

#include "zombiereloaded/libraries/objectlib"
#include "zombiereloaded/eventmanager/event"
#include "zombiereloaded/eventmanager/natives"

/*____________________________________________________________________________*/

#define PLUGIN_NAME         "Zombie:Reloaded Event Manager"
#define PLUGIN_AUTHOR       "Richard Helgeby"
#define PLUGIN_DESCRIPTION  "Implements the Event Manager API."
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

/**
 * ADT Array with references to all events.
 */
new Handle:EventList = INVALID_HANDLE;

/**
 * ADT Trie with mappings of event names to events.
 */
new Handle:EventNameIndex = INVALID_HANDLE;

/*____________________________________________________________________________*/

public APLRes:AskPluginLoad2(Handle:myself, bool:late, String:error[], err_max)
{
    PrintToServer("Loading event manager.");
    
    InitAPI();
    return APLRes_Success;
}

/*____________________________________________________________________________*/

public OnPluginStart()
{
    InitializeDataStorage();
    PrintToServer("Event manager loaded.");
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
        EventNameIndex = CreateArray();
    }
}

/*____________________________________________________________________________*/

ZMEvent:AddZMEvent(ZMModule:owner, const String:name[])
{
    new ZMEvent:event = CreateZMEvent(owner, name);

    AddEventToList(event);    
    AddEventToIndex(event);
    
    return event;
}

/*____________________________________________________________________________*/

RemoveZMEvent(ZMEvent:event)
{
    AssertIsValidEvent(event);
    
    // TODO: Unhook event callbacks.
    
    RemoveEventFromList(event);
    RemoveEventFromIndex(event);
    
    DeleteZMEvent(event);
}

/*____________________________________________________________________________*/

AddEventToList(ZMEvent:event)
{
    PushArrayCell(EventList, event);
}

/*____________________________________________________________________________*/

AddEventToIndex(ZMEvent:event)
{
    new String:name[EVENT_STRING_LEN];
    GetZMEventName(event, name, sizeof(name));
    
    SetTrieValue(EventNameIndex, name, event);
}

/*____________________________________________________________________________*/

RemoveEventFromList(ZMEvent:event)
{
    new index = FindValueInArray(EventList, event);
    if (index < 0)
    {
        ThrowError("Event is not in list.");
    }
    
    RemoveFromArray(EventList, index);
}

/*____________________________________________________________________________*/

RemoveEventFromIndex(ZMEvent:event)
{
    new String:name[EVENT_STRING_LEN];
    GetZMEventName(event, name, sizeof(name));
    
    RemoveFromTrie(EventNameIndex, name);
}

/*____________________________________________________________________________*/

bool:EventExists(const String:name[])
{
    return GetEventByName(name) != INVALID_ZM_EVENT;
}

/*____________________________________________________________________________*/

ZMEvent:GetEventByName(const String:name[])
{
    new ZMEvent:event = INVALID_ZM_EVENT;
    if (GetTrieValue(EventNameIndex, name, event))
    {
        return event;
    }
    
    return INVALID_ZM_EVENT;
}

/*____________________________________________________________________________*/

AssertEventNameNotExists(const String:name[])
{
    if (EventExists(name))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Event name is already in use: %s", name);
    }
}

/*____________________________________________________________________________*/

AssertEventNameNotEmpty(const String:name[])
{
    if (strlen(name) == 0)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Event name is empty.");
    }
}

/*____________________________________________________________________________*/

AssertIsEventOwner(ZMModule:module, ZMEvent:event)
{
    if (GetZMEventOwner(event) != module)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "This module does not own the specified event: %x", event);
    }
}
