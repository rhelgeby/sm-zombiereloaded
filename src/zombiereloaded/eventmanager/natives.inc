/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           natives.inc
 *  Description:    Implementation of natives.
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

InitAPI()
{
    PrintToServer("Initializing event manager API.");
    
    CreateNative("ZM_CreateEvent", API_CreateEvent);
    CreateNative("ZM_DeleteEvent", API_DeleteEvent);
    CreateNative("ZM_GetEventByName", API_GetEventByName);
    CreateNative("ZM_HookEvent", API_HookEvent);
    CreateNative("ZM_UnhookEvent", API_UnhookEvent);
    CreateNative("ZM_StartEvent", API_StartEvent);
    CreateNative("ZM_StartSingleEvent", API_StartSingleEvent);
    CreateNative("ZM_FireEvent", API_FireEvent);
    CreateNative("ZM_CancelEvent", API_CancelEvent);
    CreateNative("ZM_HookEventManagerReady", API_HookEventManagerReady);
    CreateNative("ZM_HookEventManagerDisable", API_HookEventManagerDisable);
    CreateNative("ZM_HookEventsCreate", API_HookEventsCreate);
    CreateNative("ZM_HookEventsCreated", API_HookEventsCreated);
}

/*____________________________________________________________________________*/

/**
 * Creates a new named event.
 *
 * @param eventName     Name of new event (case sensitive).
 * @param forwardRef    Handle to private forward associated with the new event.
 *                      ZM_DeleteEvent will close this handle by default.
 *
 * @return              Event ID.
 * @error               - Event manager is not ready
 *                      - Caller is not a module
 *                      - Blank event name
 *                      - Event name already in use
 */
//native ZMEvent:ZM_CreateEvent(const String:eventName[], Handle:forwardRef);
public API_CreateEvent(Handle:plugin, numParams)
{
    if (!AssertEventManagerReady())
    {
        return 0;
    }
    
    new ZMModule:module = ZM_GetModuleByPlugin(plugin);
    if (!AssertPluginHasZMModule(module))
    {
        return 0;
    }

    new String:name[EVENT_STRING_LEN];
    GetNativeString(1, name, sizeof(name));
    
    new Handle:forwardRef = GetNativeCell(2);
    
    new ZMEvent:event = AddZMEvent(module, name, forwardRef);
    return _:event;
}

/*____________________________________________________________________________*/

/**
 * Deletes an existing event from the event manager.
 *
 * @param event         Event to delete.
 * @param deleteForward (Optional) Whether the associated forward should be
 *                      deleted (closing the handle). Default is true.
 *
 * @noreturn
 * @error               - Event manager is not ready
 *                      - Caller is not a module
 *                      - Caller is not the event owner
 *                      - Invalid event
 */
//native ZM_DeleteEvent(ZMEvent:event, bool:deleteForward = true);
public API_DeleteEvent(Handle:plugin, numParams)
{
    if (!AssertEventManagerReady())
    {
        return;
    }
    
    new ZMModule:module = ZM_GetModuleByPlugin(plugin);
    if (!AssertPluginHasZMModule(module))
    {
        return;
    }
    
    new ZMEvent:event = GetNativeCell(1);
    new bool:deleteForward = GetNativeCell(2);
    
    if (!AssertIsEventOwner(module, event))
    {
        return;
    }
    
    RemoveZMEvent(event, deleteForward);
}

/*____________________________________________________________________________*/

/**
 * Gets an event by its name.
 *
 * @param eventName     Event name to look up.
 *
 * @return              Event ID if found, otherwise INVALID_ZM_EVENT.
 * @error               - Event manager is not ready
 *                      - Caller is not a module
 */
//native ZMEvent:ZM_GetEventByName(const String:eventName[]);
public API_GetEventByName(Handle:plugin, numParams)
{
    if (!AssertEventManagerReady())
    {
        return 0;
    }
    
    new String:eventName[EVENT_STRING_LEN];
    GetNativeString(1, eventName, sizeof(eventName));
    
    new ZMEvent:event = GetEventByName(eventName);
    
    return _:event;
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
 * @error               - Event manager is not ready
 *                      - Caller is not a module
 *                      - Invalid callback
 *                      - Invalid event
 *                      - An event call is in progress (started, but not fired)
 */
//native ZM_HookEvent(ZMModule:module, ZMEvent:event, Function:callback);
public API_HookEvent(Handle:plugin, numParams)
{
    if (!AssertEventManagerReady())
    {
        return;
    }
    
    new ZMModule:module = GetNativeCell(1);
    new ZMEvent:event = GetNativeCell(2);
    new Function:callback = GetNativeCell(3);
    
    HookZMEvent(module, event, callback);
}

/*____________________________________________________________________________*/

/**
 * Removes an event hook for the current module.
 *
 * @param event         Event to remove callback from.
 * @param callback      Callback to unhook.
 *
 * @noreturn
 * @error               - Event manager is not ready
 *                      - Caller is not a module
 *                      - Invalid callback
 *                      - Invalid event
 *                      - An event call is in progress (started, but not fired)
 */
//native ZM_UnhookEvent(ZMEvent:event, Function:callback);
public API_UnhookEvent(Handle:plugin, numParams)
{
    if (!AssertEventManagerReady())
    {
        return;
    }
    
    new ZMModule:module = ZM_GetModuleByPlugin(plugin);
    if (!AssertPluginHasZMModule(module))
    {
        return;
    }
    
    new ZMEvent:event = GetNativeCell(1);
    new Function:callback = GetNativeCell(2);
    
    UnhookZMEvent(module, event, callback);
}

/*____________________________________________________________________________*/


/**
 * Starts an event call.
 *
 * Use the Call_Push* functions after this function to push event parameters.
 *
 * @param event         Event to start.
 *
 * @noreturn
 * @error               - Event manager is not ready
 *                      - Caller is not a module
 *                      - Caller is not the event owner
 *                      - Invalid event
 *                      - An event call is in progress (started, but not fired)
 */
//native ZM_StartEvent(ZMEvent:event);
public API_StartEvent(Handle:plugin, numParams)
{
    if (!AssertEventManagerReady())
    {
        return;
    }
    
    new ZMModule:module = ZM_GetModuleByPlugin(plugin);
    if (!AssertPluginHasZMModule(module))
    {
        return;
    }
    
    new ZMEvent:event = GetNativeCell(1);
    if (!AssertIsEventOwner(module, event))
    {
        return;
    }
    
    StartEvent(event);
}

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
 * @error               - Event manager is not ready
 *                      - Caller is not a module
 *                      - Caller is not the event owner
 *                      - Invalid event
 *                      - An event call is in progress (started, but not fired)
 */
//native ZM_StartSingleEvent(ZMEvent:event, ZMModule:module)
public API_StartSingleEvent(Handle:plugin, numParams)
{
    if (!AssertEventManagerReady())
    {
        return;
    }
    
    new ZMModule:module = ZM_GetModuleByPlugin(plugin);
    if (!AssertPluginHasZMModule(module))
    {
        return;
    }
    
    new ZMEvent:event = GetNativeCell(1);
    if (!AssertIsEventOwner(module, event))
    {
        return;
    }
    
    StartSingleEvent(event, module);
}

/*____________________________________________________________________________*/

/**
 * Fires the event call.
 *
 * @param result        (Output) Result of the event.
 *
 * @return              SP_ERROR_NONE on success, any other integer on failure.
 *                      See SP_ERROR_* constants.
 * @error               - Event manager is not ready
 *                      - Caller is not a module
 *                      - Caller is not the event owner
 */
//native ZM_FireEvent(&any:result = 0);
public API_FireEvent(Handle:plugin, numParams)
{
    if (!AssertEventManagerReady())
    {
        return 0;
    }
    
    new ZMModule:module = ZM_GetModuleByPlugin(plugin);
    if (!AssertPluginHasZMModule(module))
    {
        return 0;
    }
    
    new ZMEvent:event = EventStarted;
    
    if (!AssertIsEventOwner(module, event))
    {
        return 0;
    }
    
    new result = 0;
    new status = FireZMEvent(result);
    
    SetNativeCellRef(1, result);
    return status;
}

/*____________________________________________________________________________*/

/**
 * Cancels a call to an event.
 *
 * @noreturn
 * @error               - Event manager is not ready
 *                      - Caller is not a module
 *                      - Caller is not the event owner
 *                      - Called before an event is started
 */
//native ZM_CancelEvent();
public API_CancelEvent(Handle:plugin, numParams)
{
    if (!AssertEventManagerReady())
    {
        return;
    }
    
    new ZMModule:module = ZM_GetModuleByPlugin(plugin);
    if (!AssertPluginHasZMModule(module))
    {
        return;
    }
    
    new ZMEvent:event = EventStarted;
    
    if (!AssertIsEventOwner(module, event))
    {
        return;
    }
    
    CancelEvent();
}

/*____________________________________________________________________________*/

/**
 * Registers an event hook for the OnEventManagerReady event.
 *
 * @param callback      Function to call when event is fired.
 *
 * @return              Event ID.
 * @error               - Caller is not a module
 *                      - Event is already hooked by the module
 */
//native ZMEvent:ZM_HookEventManagerReady(ZM_OnEventManagerReady:callback);
public API_HookEventManagerReady(Handle:plugin, numParams)
{
    new ZMModule:module = ZM_GetModuleByPlugin(plugin);
    if (!AssertPluginHasZMModule(module))
    {
        return 0;
    }
    
    new Function:callback = GetNativeCell(1);
    
    HookZMEvent(module, EventOnEventManagerReady, callback);
    return _:EventOnEventManagerReady;
}

/*____________________________________________________________________________*/

/**
 * Registers an event hook for the OnEventManagerDisable event.
 *
 * @param callback      Function to call when event is fired.
 *
 * @return              Event ID.
 *                      - Caller is not a module
 *                      - Event is already hooked by the module
 */
//native ZMEvent:ZM_HookEventManagerDisable(ZM_OnEventManagerDisable:callback);
public API_HookEventManagerDisable(Handle:plugin, numParams)
{
    new ZMModule:module = ZM_GetModuleByPlugin(plugin);
    if (!AssertPluginHasZMModule(module))
    {
        return 0;
    }
    
    new Function:callback = GetNativeCell(1);
    
    HookZMEvent(module, EventOnEventManagerDisable, callback);
    return _:EventOnEventManagerDisable;
}

/*____________________________________________________________________________*/

/**
 * Registers an event hook for the OnEventsCreate event.
 *
 * @param callback      Function to call when event is fired.
 *
 * @return              Event ID.
 * @error               - Event manager is not ready
 *                      - Caller is not a module
 *                      - Event is already hooked by the module
 */
//native ZMEvent:ZM_HookEventsCreate(ZM_OnEventsCreate:callback);
public API_HookEventsCreate(Handle:plugin, numParams)
{
    new ZMModule:module = ZM_GetModuleByPlugin(plugin);
    if (!AssertPluginHasZMModule(module))
    {
        return 0;
    }
    
    new Function:callback = GetNativeCell(1);
    
    HookZMEvent(module, EventOnEventsCreate, callback);
    return _:EventOnEventsCreate;
}

/*____________________________________________________________________________*/

/**
 * Registers an event hook for the OnEventsCreated event.
 *
 * @param callback      Function to call when event is fired.
 *
 * @return              Event ID.
 * @error               - Event manager is not ready
 *                      - Caller is not a module
 *                      - Event is already hooked by the module
 */
//native ZMEvent:ZM_HookEventsCreated(ZM_OnEventsCreated:callback);
public API_HookEventsCreated(Handle:plugin, numParams)
{
    new ZMModule:module = ZM_GetModuleByPlugin(plugin);
    if (!AssertPluginHasZMModule(module))
    {
        return 0;
    }
    
    new Function:callback = GetNativeCell(1);
    
    HookZMEvent(module, EventOnEventsCreated, callback);
    return _:EventOnEventsCreated;
}
