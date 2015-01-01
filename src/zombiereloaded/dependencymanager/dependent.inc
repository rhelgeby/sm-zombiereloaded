/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           dependent.inc
 *  Description:    Defines dependent data structure with callbacks and
 *                  dependency list.
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

new ObjectType:DependentType = INVALID_OBJECT_TYPE;

enum Dependent
{
    INVALID_DEPENDENT = INVALID_OBJECT
}

#define DEPENDENT_KEY_LEN      16

/*____________________________________________________________________________*/

InitDependentType()
{
    if (DependentType != INVALID_OBJECT_TYPE)
    {
        // Already defined.
        return;
    }
    
    new blockSize = 1;
    new keySize = ByteCountToCells(DEPENDENT_KEY_LEN);
    
    DependentType = ObjLib_CreateType(blockSize, keySize);
    
    ObjLib_AddKey(DependentType, "dependencies", ObjDataType_Handle);
    ObjLib_AddKey(DependentType, "readyCallback", ObjDataType_Function);
    ObjLib_AddKey(DependentType, "unavailableCallback", ObjDataType_Function);
    
    // Current state of this dependent.
    ObjLib_AddKey(DependentType, "dependenciesReady", ObjDataType_Bool);
}

/*____________________________________________________________________________*/

Dependent:CreateDependent()
{
    InitDependentType();
    
    new Object:dependent = ObjLib_CreateObject(DependentType);
    
    new Handle:dependencies = CreateArray();
    ObjLib_SetHandle(dependent, "dependencies", dependencies);
    
    ObjLib_SetFunction(dependent, "readyCallback", INVALID_FUNCTION);
    ObjLib_SetFunction(dependent, "unavailableCallback", INVALID_FUNCTION);
    
    ObjLib_SetBool(dependent, "dependenciesReady", false);
    
    return Dependent:dependent;
}

/*____________________________________________________________________________*/

DeleteDependent(Dependent:dependent)
{   
    if (dependent == INVALID_DEPENDENT)
    {
        ThrowError("Invalid dependent.");
    }
    
    new Handle:dependencies = GetDependencies(dependent);
    CloseHandle(dependencies);
    
    ObjLib_DeleteObject(Object:dependent, false);
}

/*____________________________________________________________________________*/

Handle:GetDependencies(Dependent:dependent)
{
    return ObjLib_GetHandle(Object:dependent, "dependencies");
}

/*____________________________________________________________________________*/

AddDependency(Dependent:dependent, Library:library)
{
    new Handle:dependencies = GetDependencies(dependent);
    
    new index = FindValueInArray(dependencies, library);
    if (index >= 0)
    {
        // Already added, ignore it.
        return;
    }
    
    PushArrayCell(dependencies, library);
}

/*____________________________________________________________________________*/

ZM_OnDependenciesReady:GetReadyCallback(Dependent:dependent)
{
    return ZM_OnDependenciesReady:ObjLib_GetFunction(Object:dependent, "readyCallback");
}

/*____________________________________________________________________________*/

ZM_OnDependenciesUnavailable:GetUnavailableCallback(Dependent:dependent)
{
    return ZM_OnDependenciesUnavailable:ObjLib_GetFunction(Object:dependent, "unavailableCallback");
}

/*____________________________________________________________________________*/

SetCallbacks(
        Dependent:dependent,
        ZM_OnDependenciesReady:ready,
        ZM_OnDependenciesUnavailable:unavailable)
{
    ObjLib_SetFunction(Object:dependent, "readyCallback", ready);
    ObjLib_SetFunction(Object:dependent, "unavailableCallback", unavailable);
}

/*____________________________________________________________________________*/

bool:IsDependenciesReady(Dependent:dependent)
{
    return ObjLib_GetBool(Object:dependent, "dependenciesReady");
}

/*____________________________________________________________________________*/

SetDependenciesReady(Dependent:dependent, bool:ready)
{
    ObjLib_SetBool(Object:dependent, "dependenciesReady", ready);
}
