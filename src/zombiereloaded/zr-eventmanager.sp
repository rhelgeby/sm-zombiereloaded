/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           zr-eventmanager.sp
 *  Description:    Implements the Event Manager API.
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
#include <zombie/core/eventmanager>

#include "zombiereloaded/libraries/objectlib"
#include "zombiereloaded/eventmanager/event"
#include "zombiereloaded/eventmanager/natives"

/*____________________________________________________________________________*/

#define PLUGIN_NAME         "Zombie:Reloaded Event Manager"
#define PLUGIN_AUTHOR       "Richard Helgeby"
#define PLUGIN_DESCRIPTION  "Implements the Event Manager API."
#define PLUGIN_VERSION      "1.0.0"
#define PLUGIN_URL          "https://github.com/rhelgeby/sm-zombiereloaded"

public Plugin:myinfo =
{
    name = PLUGIN_NAME,
    author = PLUGIN_AUTHOR,
    description = PLUGIN_DESCRIPTION,
    version = PLUGIN_VERSION,
    url = PLUGIN_URL
};

/*____________________________________________________________________________*/
