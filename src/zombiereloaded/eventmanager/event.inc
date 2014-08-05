/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           event.inc
 *  Description:    Defines event data structure and accessors.
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

new ObjectType:EventType = INVALID_OBJECT_TYPE;

#define EVENT_STRING_LEN   255
#define EVENT_KEY_LEN      8

/*____________________________________________________________________________*/

InitEventType()
{
    if (EventType != INVALID_OBJECT_TYPE)
    {
        // Already defined.
        return;
    }
    
    new blockSize = ByteCountToCells(EVENT_STRING_LEN);
    new keySize = ByteCountToCells(EVENT_KEY_LEN);
    
    EventType = ObjLib_CreateType(blockSize, keySize);
    
    ObjLib_AddKey(EventType, "owner", ObjDataType_Cell);
    ObjLib_AddKey(EventType, "name", ObjDataType_String);
    
    // Handle to private forward for the event.
    ObjLib_AddKey(EventType, "forward", ObjDataType_Handle);
    
    // ADT Trie with module IDs mapped to callbacks. Normally we let SourceMod
    // store and handle callbacks. This is only used for events that are fired
    // against specific modules only. No need to worry about lookup performance
    // in these cases.
    ObjLib_AddKey(EventType, "callbacks", ObjDataType_Handle);
}

/*____________________________________________________________________________*/

ZMEvent:CreateZMEvent(
        ZMModule:owner,
        const String:name[],
        Handle:forwardRef)
{
    AssertIsValidZMModule(owner);
    AssertEventNameNotEmpty(name);
    AssertEventNameNotExists(name);
    AssertIsValidForward(forwardRef);
    
    InitEventType();
    
    new Object:event = ObjLib_CreateObject(EventType);
    ObjLib_SetCell(event, "owner", owner);
    ObjLib_SetString(event, "name", name);
    ObjLib_SetHandle(event, "forward", forwardRef);
    
    new Handle:callbacks = CreateTrie();
    ObjLib_SetHandle(event, "callbacks", callbacks);
    
    LogMessage("Created event: \"%s\" (%x)", name, event);
    
    return ZMEvent:event;
}

/*____________________________________________________________________________*/

DeleteZMEvent(ZMEvent:event, bool:deleteForward = true)
{   
    if (!ZM_IsValidEvent(event))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Invalid event: %x", event);
    }
    
    new String:eventName[EVENT_STRING_LEN];
    GetZMEventName(event, eventName, sizeof(eventName));
    
    new Handle:forwardRef = ObjLib_GetHandle(Object:event, "forward");
    new Handle:callbacks = ObjLib_GetHandle(Object:event, "callbacks");
    
    if (deleteForward)
    {
        CloseHandle(forwardRef);
    }
    
    CloseHandle(callbacks);
    
    ObjLib_DeleteObject(Object:event, false);
    
    LogMessage("Deleted event: \"%s\" (%x)", eventName, event);
}

/*____________________________________________________________________________*/

ZMModule:GetZMEventOwner(ZMEvent:event)
{
    return ZMModule:ObjLib_GetCell(Object:event, "owner");
}

/*____________________________________________________________________________*/

GetZMEventName(ZMEvent:event, String:buffer[], maxlen)
{
    return ObjLib_GetString(Object:event, "name", buffer, maxlen);
}

/*____________________________________________________________________________*/

Handle:GetZMEventForward(ZMEvent:event)
{
    return ObjLib_GetHandle(Object:event, "forward");
}

/*____________________________________________________________________________*/

Handle:GetZMEventCallbacks(ZMEvent:event)
{
    return ObjLib_GetHandle(Object:event, "callbacks");
}

/*____________________________________________________________________________*/

AddCallbackToEvent(ZMEvent:event, ZMModule:module, Function:callback)
{
    new Handle:callbacks = GetZMEventCallbacks(event);
    
    new String:moduleIdString[16];
    GetHexString(module, moduleIdString, sizeof(moduleIdString));
    
    SetTrieValue(callbacks, moduleIdString, callback);
}

/*____________________________________________________________________________*/

bool:RemoveCallbackFromEvent(ZMEvent:event, ZMModule:module)
{
    new Handle:callbacks = GetZMEventCallbacks(ZMEvent:event);
    
    new String:moduleIdString[16];
    GetHexString(module, moduleIdString, sizeof(moduleIdString));
    
    return bool:RemoveFromTrie(callbacks, moduleIdString);
}

/*____________________________________________________________________________*/

Function:GetModuleCallback(ZMEvent:event, ZMModule:module)
{
    new Function:callback = INVALID_FUNCTION;
    
    decl String:moduleIdString[16];
    GetHexString(module, moduleIdString, sizeof(moduleIdString));
    
    new Handle:callbacks = GetZMEventCallbacks(event);
    if (GetTrieValue(callbacks, moduleIdString, callback))
    {
        return callback;
    }
    
    return INVALID_FUNCTION;
}

/*____________________________________________________________________________*/

bool:HasModuleCallback(ZMEvent:event, ZMModule:module)
{
    return GetModuleCallback(event, module) != INVALID_FUNCTION;
}
