/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           zr-dependencymanager.sp
 *  Description:    Implements the Dependency Manager API.
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
#include <zombie/core/dependencymanager>
#include <zombie/core/bootstrap/boot-dependencymanager>

#include "zombiereloaded/common/version"

/*____________________________________________________________________________*/

#define PLUGIN_NAME         "Zombie:Reloaded Dependency Manager"
#define PLUGIN_DESCRIPTION  "Implements the Dependency Manager API."

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
 * Hash map of library names mapped to their objects.
 */
new Handle:Libraries = INVALID_HANDLE;

/**
 * Hash map of plugin IDs mapped to their objects
 */
new Handle:Dependencies = INVALID_HANDLE;

/*____________________________________________________________________________*/

#include "zombiereloaded/libraries/objectlib"
#include "zombiereloaded/dependencymanager/library"
#include "zombiereloaded/dependencymanager/dependent"

/*____________________________________________________________________________*/
