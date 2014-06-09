/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           feature.inc
 *  Description:    Defines feature structure and accessors.
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

new ObjectType:FeatureObjectType = INVALID_OBJECT_TYPE;

#define FEATURE_STRING_LEN   64
#define FEATURE_KEY_LEN      16

/*____________________________________________________________________________*/

InitFeatureType()
{
    if (FeatureObjectType != INVALID_OBJECT_TYPE)
    {
        // Already defined.
        return;
    }
    
    new blockSize = ByteCountToCells(FEATURE_STRING_LEN);
    new keySize = ByteCountToCells(FEATURE_KEY_LEN);
    
    FeatureObjectType = ObjLib_CreateType(blockSize, keySize);
    
    ObjLib_AddKey(FeatureObjectType, "ownerModule", ObjDataType_Cell);
    ObjLib_AddKey(FeatureObjectType, "name", ObjDataType_String);
}

/*____________________________________________________________________________*/

ZMFeature:CreateFeature(ZMModule:ownerModule, const String:name[])
{
    if (strlen(name) == 0)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Feature name cannot be empty.");
    }
    if (!IsValidModule(ownerModule))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Invalid module: %x", ownerModule);
    }
    if (FeatureExists(name))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Feature name already exists. Must be unique.");
    }
    
    InitFeatureType();
    
    new Object:feature = ObjLib_CreateObject(FeatureObjectType);
    ObjLib_SetCell(feature, "ownerModule", ownerModule);
    ObjLib_SetString(feature, "name", name);
    
    PrintToServer("Created feature \"%s\".", name);
    
    return ZMFeature:feature;
}

/*____________________________________________________________________________*/

DeleteFeature(ZMFeature:feature)
{   
    if (!IsValidFeature(feature))
    {
        ThrowNativeError(SP_ERROR_ABORTED, "Invalid feature: %x", feature);
    }
    
    ObjLib_DeleteObject(Object:feature);
    
    PrintToServer("Deleted feature 0x%X.", feature);
}

/*____________________________________________________________________________*/

ZMModule:GetFeatureOwner(ZMFeature:feature)
{
    return ZMModule:ObjLib_GetCell(Object:feature, "ownerModule");
}

/*____________________________________________________________________________*/

GetFeatureName(ZMFeature:feature, String:buffer[], maxlen)
{
    return ObjLib_GetString(Object:feature, "name", buffer, maxlen);
}

/*____________________________________________________________________________*/

bool:IsValidFeature(ZMFeature:feature)
{
    return feature != INVALID_ZM_FEATURE;
}
