/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           events.inc
 *  Description:    Triggers predefined events in the event manager.
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

/*____________________________________________________________________________*/

FireOnEventManagerReady()
{
    StartEvent(EventOnEventManagerReady);
    FireZMEvent();
}

/*____________________________________________________________________________*/

FireOnEventManagerDisable()
{
    StartEvent(EventOnEventManagerDisable);
    FireZMEvent();
}

/*____________________________________________________________________________*/

FireOnEventsCreate()
{
    StartEvent(EventOnEventsCreate);
    FireZMEvent();
}

/*____________________________________________________________________________*/

FireOnEventsCreated()
{
    StartEvent(EventOnEventsCreated);
    FireZMEvent();
}
