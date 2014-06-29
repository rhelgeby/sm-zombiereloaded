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
}

/*____________________________________________________________________________*/

ZMEvent:CreateZMEvent(ZMModule:owner, const String:name[])
{
    AssertIsValidZMModule(owner);
    AssertEventNameNotEmpty(name);
    AssertEventNameNotExists(name);
    
    InitEventType();
    
    new Object:event = ObjLib_CreateObject(EventType);
    ObjLib_SetCell(event, "owner", owner);
    ObjLib_SetString(event, "name", name);
    
    PrintToServer("Created event\"%s\".", name);
    
    return ZMEvent:event;
}

/*____________________________________________________________________________*/

DeleteZMEvent(ZMEvent:event)
{   
    if (!ZM_IsValidEvent(event))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Invalid event: %x", event);
    }
    
    ObjLib_DeleteObject(Object:event);
    
    PrintToServer("Deleted event 0x%X.", event);
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
