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

#include <zombie/core/bootstrap/boot-modulemanager>

#include "zombiereloaded/common/version"

/*____________________________________________________________________________*/

#define PLUGIN_NAME         "Zombie:Reloaded Event Manager"
#define PLUGIN_DESCRIPTION  "Implements the Event Manager API."

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
 * ADT Array with references to all events.
 */
new Handle:EventList = INVALID_HANDLE;

/**
 * ADT Trie with mappings of event names to events.
 */
new Handle:EventNameIndex = INVALID_HANDLE;

/**
 * Reference to event being prepared (not fired yet), if any.
 */
new ZMEvent:EventStarted = INVALID_ZM_EVENT;

/**
 * Stores whether the event manager is ready to handle natives.
 */
new bool:EventManagerReady = false;

/**
 * Module for the event manager.
 */
new ZMModule:EventManagerModule = INVALID_ZM_MODULE;

/*____________________________________________________________________________*/

// Predefined events (and their forwards) by the event manager.

new Handle:ForwardOnEventManagerReady = INVALID_HANDLE;
new ZMEvent:EventOnEventManagerReady = INVALID_ZM_EVENT;

new Handle:ForwardOnEventManagerDisable = INVALID_HANDLE;
new ZMEvent:EventOnEventManagerDisable = INVALID_ZM_EVENT;

new Handle:ForwardOnEventsCreate = INVALID_HANDLE;
new ZMEvent:EventOnEventsCreate = INVALID_ZM_EVENT;

new Handle:ForwardOnEventsCreated = INVALID_HANDLE;
new ZMEvent:EventOnEventsCreated = INVALID_ZM_EVENT;

/*____________________________________________________________________________*/

#include "zombiereloaded/libraries/objectlib"
#include "zombiereloaded/eventmanager/event"
#include "zombiereloaded/eventmanager/events"
#include "zombiereloaded/eventmanager/boot"
#include "zombiereloaded/eventmanager/natives"

/*____________________________________________________________________________*/

ZMEvent:AddZMEvent(ZMModule:owner, const String:name[], Handle:forwardRef)
{
    new ZMEvent:event = CreateZMEvent(owner, name, forwardRef);

    AddEventToList(event);    
    AddEventToIndex(event);
    
    return event;
}

/*____________________________________________________________________________*/

RemoveZMEvent(ZMEvent:event, bool:deleteForward = true)
{
    if (!AssertIsValidZMEvent(event))
    {
        return;
    }
    
    RemoveEventFromList(event);
    RemoveEventFromIndex(event);
    
    DeleteZMEvent(event, deleteForward);
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

GetHexString(any:value, String:buffer[], maxlen)
{
    Format(buffer, maxlen, "%x", value);
}

/*____________________________________________________________________________*/

public Handle:StartEvent(ZMEvent:event)
{
    if (!AssertIsValidZMEvent(event)
        || !AssertEventNotStarted())
    {
        return;
    }
    
    new Handle:forwardRef = GetZMEventForward(event);
    
    Call_StartForward(forwardRef);
    EventStarted = event;
}

/*____________________________________________________________________________*/

StartSingleEvent(ZMEvent:event, ZMModule:module)
{
    if (!AssertIsValidZMEvent(event)
        || !AssertEventNotStarted())
    {
        return;
    }
    
    new Handle:eventOwnerPlugin = ZM_GetModuleOwner(module);
    
    new Function:callback = GetModuleCallback(event, module);
    AssertModuleCallbackValid(callback, event, module);
    
    Call_StartFunction(eventOwnerPlugin, callback);
    EventStarted = event;
}

/*____________________________________________________________________________*/

FireZMEvent(&any:result = 0)
{
    if (!AssertEventStarted())
    {
        return 0;
    }
    
    // Reset before calling, in case of nested events.
    EventStarted = INVALID_ZM_EVENT;
    
    return Call_Finish(result);
}

/*____________________________________________________________________________*/

CancelEvent()
{
    if (!AssertEventStarted())
    {
        return;
    }
    
    Call_Cancel();
    EventStarted = INVALID_ZM_EVENT;
}

/*____________________________________________________________________________*/

HookZMEvent(ZMModule:module, ZMEvent:event, Function:callback)
{
    CreatePredefinedEventsIfNotExist();
    
    if (!AssertIsValidZMModule(module)
        || !AssertIsValidZMEvent(event)
        || !AssertIsValidCallback(callback)
        || !AssertHookNotExists(event, module))
    {
        return;
    }
    new Handle:ownerPlugin = ZM_GetModuleOwner(module);
    new Handle:forwardRef = GetZMEventForward(event);
    
    if (!AddToForward(forwardRef, ownerPlugin, callback))
    {
        ThrowForwardUpdateError();
        return;
    }
    AddCallbackToEvent(event, module, callback);
}

/*____________________________________________________________________________*/

UnhookZMEvent(ZMModule:module, ZMEvent:event, Function:callback)
{
    if (!AssertIsValidZMModule(module)
        || !AssertIsValidZMEvent(event)
        || !AssertIsValidCallback(callback)
        || !AssertHookExists(event, module))
    {
        return;
    }
    
    new Handle:ownerPlugin = ZM_GetModuleOwner(module);
    new Handle:forwardRef = GetZMEventForward(event);
    
    if (!RemoveFromForward(forwardRef, ownerPlugin, callback))
    {
        ThrowForwardUpdateError();
        return;
    }
    RemoveCallbackFromEvent(event, module);
}

/*____________________________________________________________________________*/

bool:AssertEventNameNotExists(const String:name[])
{
    if (EventExists(name))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Event name is already in use: %s", name);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertEventNameNotEmpty(const String:name[])
{
    if (strlen(name) == 0)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Event name is empty.");
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertIsEventOwner(ZMModule:module, ZMEvent:event)
{
    if (GetZMEventOwner(event) != module)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "This module does not own the specified event: %x", event);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertIsValidForward(Handle:forwardRef)
{
    if (forwardRef == INVALID_HANDLE)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Invalid forward: %x", forwardRef);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertEventStarted()
{
    if (EventStarted == INVALID_ZM_EVENT)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "No event is started.");
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertEventNotStarted()
{
    if (EventStarted != INVALID_ZM_EVENT)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "An event is already started, but not fired or canceled.");
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertModuleCallbackValid(Function:callback, ZMEvent:event, ZMModule:module)
{
    if (callback == INVALID_FUNCTION)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "The specified module (%x) has not hooked this event (%x).", module, event);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertIsValidCallback(Function:callback)
{
    if (callback == INVALID_FUNCTION)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Invalid callback: %x", callback);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertHookNotExists(ZMEvent:event, ZMModule:module)
{
    if (HasModuleCallback(event, module))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "The module (%x) has already hooked this event (%x).", module, event);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

bool:AssertHookExists(ZMEvent:event, ZMModule:module)
{
    if (!HasModuleCallback(event, module))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "The module (%x) has not hooked this event (%x).", module, event);
        return false;
    }
    
    return true;
}

/*____________________________________________________________________________*/

ThrowForwardUpdateError()
{
    ThrowNativeError(SP_ERROR_ABORTED, "Failed to update callback list. This can not happen while an event call is started and not fired.");
}

/*____________________________________________________________________________*/

bool:AssertEventManagerReady()
{
    if (!EventManagerReady)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "The event manager is not ready.");
        return false;
    }
    
    return true;
}
