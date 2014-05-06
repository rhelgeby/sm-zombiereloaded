/*
 * ============================================================================
 *
 *  Zombie API
 *
 *  File:           auth.inc
 *  Type:           Include
 *  Description:    Authentication and authorization API.
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

#if defined _zm_auth_included
 #endinput
#endif
#define _zm_auth_included

#include <zombie/core/modulemanager>

/*____________________________________________________________________________*/

/*

Modules that use this manager should cache the security check result and update
their cache when security settings have changed.

*/

/*____________________________________________________________________________*/

/**
 * Called when authorization settings were changed. Modules should update their
 * caches in this event.
 */
functag ZM_OnAuthSettingsChanged();

/*____________________________________________________________________________*/

// TODO: Natives for managing security configuration.

/*____________________________________________________________________________*/

/**
 * Returns whether the plugin has access to the specified feature.
 *
 * @param plugin        Plugin to authorize.
 * @param featureID     Feature ID.
 *
 * @return              True if plugin has access, false otherwise.
 */
native bool:ZM_SecurityCheck(Handle:plugin, ZMFeature:featureID);
