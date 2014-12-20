/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           library.inc
 *  Description:    Defines library data structure with owner and state.
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

new ObjectType:LibraryType = INVALID_OBJECT_TYPE;

enum Library
{
    INVALID_LIBRARY = INVALID_OBJECT
}

#define LIBRARY_KEY_LEN      16

/*____________________________________________________________________________*/

InitLibraryType()
{
    if (LibraryType != INVALID_OBJECT_TYPE)
    {
        // Already defined.
        return;
    }
    
    new blockSize = 1;
    new keySize = ByteCountToCells(LIBRARY_KEY_LEN);
    
    LibraryType = ObjLib_CreateType(blockSize, keySize);
    
    ObjLib_AddKey(LibraryType, "owner", ObjDataType_Handle);
    ObjLib_AddKey(LibraryType, "available", ObjDataType_Bool);
}

/*____________________________________________________________________________*/

Library:CreateLibrary(Handle:owner = INVALID_HANDLE)
{
    InitLibraryType();
    
    new Object:library = ObjLib_CreateObject(LibraryType);
    
    ObjLib_SetHandle(library, "owner", owner);
    ObjLib_SetBool(library, "available", false);
    
    return Library:dependent;
}

/*____________________________________________________________________________*/

DeleteLibrary(Library:library)
{   
    if (library == INVALID_LIBRARY)
    {
        ThrowError("Invalid library.");
    }
    
    ObjLib_DeleteObject(Object:library, false);
}

/*____________________________________________________________________________*/

Handle:GetLibraryOwner(Library:library)
{
    return ObjLib_GetHandle(Object:library, "owner");
}

/*____________________________________________________________________________*/

bool:IsLibraryAvailable(Library:library)
{
    return ObjLib_GetBool(Object:library, "available");
}

/*____________________________________________________________________________*/

SetLibraryAvailable(Library:library, bool:available)
{
    return ObjLib_SetBool(Object:library, "available", available);
}
