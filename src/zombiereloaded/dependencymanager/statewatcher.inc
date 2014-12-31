/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           statewatcher.inc
 *  Description:    Inspects libraries for state changes.
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

new bool:AllPluginsLoaded = false;

UpdateLibraryState(
        const String:libraryName[],
        Library:library,
        bool:triggerEvent = true)
{
    new bool:newState = LibraryExists(libraryName);
    SetLibraryState(library, newState, triggerEvent);
}

/*____________________________________________________________________________*/

SetLibraryState(Library:library, bool:available, bool:triggerEvent = true)
{
    new bool:oldState = IsLibraryAvailable(library);
    SetLibraryAvailable(library, available);
    
    if (!AllPluginsLoaded)
    {
        // All plugins aren't loaded yet. Wait to avoid repeated dependency
        // state checks during configuration phase of all plugins. A new check
        // will be done in OnAllPluginsLoaded. After that, it will do a full
        // check for each library state change.
        return;
    }
    
    if (triggerEvent && available != oldState)
    {
        // State change, check dependents.
        // TODO
    }    
}

/*____________________________________________________________________________*/

public OnAllPluginsLoaded()
{
    AllPluginsLoaded = true;
    
    // TODO: Loop through each known library and call UpdateLibraryState.
}

/*____________________________________________________________________________*/

public OnLibraryAdded(const String:name[])
{
    new Library:library = GetLibrary(name);
    if (library != INVALID_LIBRARY)
    {
        SetLibraryState(library, true);
    }
}

/*____________________________________________________________________________*/

public OnLibraryRemoved(const String:name[])
{
    new Library:library = GetLibrary(name);
    if (library != INVALID_LIBRARY)
    {
        SetLibraryState(library, false);
    }
}
