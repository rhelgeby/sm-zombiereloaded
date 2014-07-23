/*
 * ============================================================================
 *
 *  Zombie Mod API
 *
 *  File:           eventmanager.inc
 *  Description:    Create and manage abstract events. Works as a wrapper for
 *                  private forwards so that the core can do interceptions.
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

#if defined _zm_eventmanager_included
 #endinput
#endif
#define _zm_eventmanager_included

/*____________________________________________________________________________*/

/*

GOALS

Technical user stories:

* As a module I need to create a named event to easily notify many modules at
  once.

* As a module I need to look an event up by name to avoid tight coupling between
  modules.

* As a module I need to hook an event to be able to listen to events.

* As a module I need to unhook an event if I no longer want to listen to the
  event.

* As a module I need to know when events are being defined so I can define my
  own events before other modules tries to hook them.

* As a module I need to know when all events are defined so I can hook events.

* As a module I need to trigger an event so a message can be broadcasted to
  listening modules.

* As a module I don't want to receive events if I'm disabled in the module
  manager.

* As the event manager I need to fire a specific event to a single module to
  be able to replay events when a module is enabled late.

* As a module I need to mark an event as replayable for late loaded modules.

* As the event manager I need to store prioritized list of events to replay for
  late loaded modules.


IMPLEMENTATION GUIDELINES

Forwards:
Implement events using private forwards. Then most of the work can be delegated
to SourceMod, including type safety.

Direct events:
To be able to send fire an event for a specific module, the event manager still
need to store a list of callbacks. SourceMod also stores a callback list for
every forward, but we also need to know which module that callback belongs to.

Event owner:
The module that creates an event is considered the event owner. Only that module
is allowed to fire the event.

Event hooks:
Modules must be looked up by their plugin handle. Use ZM_GetModuleByPlugin in
modulemanager.inc. That way the plugins doesn't have to pass their own module
ID each time, and we avoid that they pass an invalid ID.

In addition modules must be authorized before an event hook is allowed. Use
ZM_SecurityCheck in auth.inc. Cache the result and update within
OnAuthSettingsChanged event. When security settings have changed, event hooks
that modules previously had access to, but no longer, should be removed (maybe
we also need an event to notify the module).

New and unloaded plugins:
The event manager must update its list of event callbacks when a plugin is
unloaded, so that any event it may own is unregistered and properly removed.
Event hooks that other mouldes made to that event should be unhooked.

Bootstrapping:
Due to plugins being loaded in an undefined order, there's no guarantee that the
event manager will be loaded before other plugins that depends on its API. They
need to consider using LibraryExists and OnLibraryAdded. We might need a
bootstrap library that plugins can include to avoid custom bootstrap logic in
each plugin.

*/

/*____________________________________________________________________________*/

/**
 * Event ID.
 */
enum ZMEvent
{
    INVALID_ZM_EVENT
}

/*____________________________________________________________________________*/

/**
 * @section Predefined bootstrap events created by the event manager.
 */

/**
 * Called when events are being created. Modules that create their own events
 * should create them in this event.
 */
#define ZM_ON_EVENTS_CREATE "OnEventsCreate"
functag public ZM_OnEventsCreate();

/**
 * All events are created. Modules should register event hooks in this event.
 */
#define ZM_ON_EVENTS_CREATED "OnEventsCreated"
functag public ZM_OnEventsCreated();

/** 
 * @endsection
 */

/*____________________________________________________________________________*/

/**
 * Creates a new named event.
 *
 * @param eventName     Name of new event (case sensitive).
 * @param forwardRef    Handle to private forward associated with the new event.
 *                      ZM_DeleteEvent will close this handle by default.
 *
 * @return              Event ID.
 * @error               Caller is not a module, blank event name, or event name
 *                      already in use.
 */
native ZMEvent:ZM_CreateEvent(const String:eventName[], Handle:forwardRef);

/*____________________________________________________________________________*/

/**
 * Deletes an existing event from the event manager.
 *
 * @param event         Event to delete.
 * @param deleteForward (Optional) Whether the associated forward should be
 *                      deleted (closing the handle). Default is true.
 *
 * @noreturn
 * @error               Caller is not a module, not the event owner, or event
 *                      ID is invalid.
 */
native ZM_DeleteEvent(ZMEvent:event, bool:deleteForward = true);

/*____________________________________________________________________________*/

/**
 * Validates an event ID.
 *
 * @param event         Event to validate.
 *
 * @return              True if valid, otherwise false.
 * @error               Caller is not a module.
 */
stock bool:ZM_IsValidEvent(ZMEvent:event)
{
    return event != INVALID_ZM_EVENT;
}

/*____________________________________________________________________________*/

/**
 * Gets an event by its name.
 *
 * @param eventName     Event name to look up.
 *
 * @return              Event ID if found, otherwise INVALID_ZM_EVENT.
 * @error               Caller is not a module.
 */
native ZMEvent:ZM_GetEventByName(const String:eventName[]);

/*____________________________________________________________________________*/

/**
 * Returns whether the specified event exists.
 *
 * @param eventName     Event name to look up.
 *
 * @return              True if event exists, false otherwise.
 * @error               Caller is not a module.
 */
stock bool:ZM_EventExists(const String:eventName[])
{
    new ZMEvent:event = ZM_GetEventByName(eventName);
    return ZM_IsValidEvent(event);
}

/*____________________________________________________________________________*/

/**
 * Adds a callback to an event.
 *
 * This is a generic native intended to be used by modules that create events.
 * It does not provide type safety (any function can be passed), so it should be
 * wrapped by a hook function for a specific event with a specific callback tag.
 *
 * @param module        Module where callback resides.
 * @param event         Event to add callback to.
 * @param callback      Function callback to add.
 *
 * @noreturn
 * @error               Caller is not a module, not the event owner, callback or
 *                      event ID is invalid.
 */
native ZM_HookEvent(ZMModule:module, ZMEvent:event, Function:callback);

/*____________________________________________________________________________*/

/**
 * Removes an event hook.
 *
 * @param event         Event ID.
 * @param callback      Callback to unhook.
 *
 * @noreturn
 * @error               Caller is not a module, callback or event ID is invalid.
 */
native ZM_UnhookEvent(ZMEvent:event, Function:callback);

/*____________________________________________________________________________*/

/**
 * Starts an event call.
 *
 * Use the Call_Push* functions after this function to push event parameters.
 *
 * @param event         Event to start.
 *
 * @noreturn
 * @error               Caller is not a module, not the event owner, invalid
 *                      event, or started before another event is fired.
 */
native ZM_StartEvent(ZMEvent:event);

/*____________________________________________________________________________*/

/**
 * Starts an event call to single module.
 *
 * Use the Call_Push* functions after this function to push event parameters.
 *
 * @param event         Event to start.
 * @param module        Target module to fire event in.
 *
 * @noreturn
 * @error               Caller is not a module, not the event owner, invalid
 *                      event, or started before another event is fired.
 */
native ZM_StartSingleEvent(ZMEvent:event, ZMModule:module)

/*____________________________________________________________________________*/

/**
 * Fires the event call.
 *
 * @param result        (Output) Result of the event.
 *
 * @return              SP_ERROR_NONE on success, any other integer on failure.
 *                      See SP_ERROR_* constants.
 * @error               Caller is not a module, or not the event owner.
 */
native ZM_FireEvent(&any:result = 0);

/*____________________________________________________________________________*/

/**
 * Cancels a call to an event.
 *
 * @noreturn
 * @error               Caller is not a module, or called before an event is
 *                      started.
 */
native ZM_CancelEvent();

/*____________________________________________________________________________*/

/**
 * Registers an event hook for the OnEventsCreate event.
 *
 * @param callback      Function to call when event is fired.
 *
 * @return              Event ID.
 * @error               Caller is not a module, or event is already hooked by
 *                      the module.
 */
native ZMEvent:ZM_HookEventsCreate(ZM_OnEventsCreate:callback);

/*____________________________________________________________________________*/

/**
 * Registers an event hook for the OnEventsCreated event.
 *
 * @param callback      Function to call when event is fired.
 *
 * @return              Event ID.
 * @error               Caller is not a module, or event is already hooked by
 *                      the module.
 */
native ZMEvent:ZM_HookEventsCreated(ZM_OnEventsCreated:callback);

/*____________________________________________________________________________*/

/**
 * Throws an error if the specified event is invalid.
 *
 * @param event         Event to validate.
 *
 * @noreturn
 * @error               Invalid event.
 */
stock AssertIsValidEvent(ZMEvent:event)
{
    if (!ZM_IsValidEvent(event))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Invalid event: %x", event);
    }
}
