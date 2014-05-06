/*
 * ============================================================================
 *
 *  Zombie API
 *
 *  File:           modulemanager.inc
 *  Description:    API for registering and managing modules.
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

#if defined _zm_modulemanager_included
 #endinput
#endif
#define _zm_modulemanager_included

/*____________________________________________________________________________*/

/*

GOALS

Technical user stories:

* As a plugin I need to register myself as a module in the module manager so I
  can become an authenticated module in the zombie framework.

* As a module I need to have a unique ID so I can be identified by the zombie
  framework.

* As a module I need to have a name so I can be identified by server admins.

* As a module I need to have a description so server admins know what I do.

* As a module I need to declare unique named features that other modules can
  declare dependency on, to avoid explicit dependency on the module itself.

* As a module I need to declare dependency on a named feature.

* As a module I need to be notified when a dependent feature is unavailable so I
  can do cleanup before disabling myself.

* As a module I need to be notified when a dependent feature is available so I
  can re-initialize myself.


Context awareness:
Some natives are context aware, so that only the owner plugin can do actions on
its own module. We don't need to pass the module ID, because registered modules
can be looked up by the plugin handle of the caller. None of the module
manager's natives should be used in performance critical areas.

Dependencies:
The module manager doesn't support module dependencies. Instead, modules declare
dependencies on specific features, regardless which module that implements it.
The implementation may use SourceMod's own feature status natives as long as the
module manager API specification is followed.

Unique module ID:
If a module is reloaded, it must re-register itself as a module and will get
another module ID. The old module ID will not be used again, until the module
manager is reloaded.

When the module manager is unloaded, all module IDs are considered invalid.
All modules should to listen for availability status of the module manager by
using OnLibraryAdded/OnLibraryRemoved.

Bootstrap event:
The module manager should make use of RegPluginLibrary so plugins can listen for
availability of the module manager. The ideal solution is that the module
manager can be reloaded in the middle of the game, and that modules will
dynamically react to this event. It may cause some interruptions in the game,
but modules should be flexible enough to start over in the next round.

*/

/*____________________________________________________________________________*/

// TODO: Events for initializing module on late load.

// TODO: Add natives for enabling and disabling modules?

/*____________________________________________________________________________*/

/**
 * Predefined module manager library name to be used with OnLibraryAdded,
 * OnLibraryRemoved, and LibraryExists.
 */
#define LIBRARY_ZM_MODULE_MANAGER "zm_module_manager"

/*____________________________________________________________________________*/

/**
 * @section Events
 */

/**
 * A feature was just added.
 *
 * @param feature       ID of added feature.
 *
 * @noreturn
 */
#define ZM_ON_FEATURE_ADDED "OnFeatureAdded"
functag public OnFeatureAdded(ZMFeature:feature);

/**
 * A feature is being removed.
 *
 * @param feature       ID of feature being removed.
 */
#define ZM_ON_FEATURE_REMOVE "OnFeatureRemove"
functag public OnFeatureRemove(ZMFeature:feature);

/**
 * @endsection
 */

/*____________________________________________________________________________*/

/**
 * Module ID.
 */
enum ZMModule
{
    INVALID_ZM_MODULE = -1
}

/*____________________________________________________________________________*/

/**
 * Feature ID.
 */
enum ZMFeature
{
    INVALID_ZM_FEATURE = -1
}

/*____________________________________________________________________________*/

/**
 * Creates a new module. The new module is bound to the calling plugin.
 *
 * Warning: You must call ZM_DeleteModule when the module is no longer in use,
 *          such as in OnPluginEnd. Otherwise the module manager will never know
 *          when the module is removed.
 *
 * Note: A plugin is only allowed to create one module. The module will remain
 *       registered until the plugin is unloaded.
 *
 * @param name          Unique module name. Used by lookup functions.
 *
 * @return              Module ID.
 * @error               Plugin has already created a module, name is already in
 *                      use or name is empty.
 */
native ZMModule:ZM_CreateModule(const String:name[]);

/*____________________________________________________________________________*/

/**
 * Deletes the module and notifies dependents.
 *
 * Call this function from OnPluginEnd or when the module is no longer in use.
 *
 * @noreturn
 * @error               Caller has not created a module.
 */
native ZM_DeleteModule();

/*____________________________________________________________________________*/

/**
 * Returns whether the specified module is valid.
 *
 * @param module        Module ID to validate.
 *
 * @return              True if valid, false otherwise.
 */
stock ZM_IsValidModule(ZMModule:module)
{
    return module != INVALID_ZM_MODULE;
}

/*____________________________________________________________________________*/

/**
 * Looks up a module by the specified plugin handle value.
 *
 * @param plugin        Handle to plugin representing a module.
 *
 * @return              Module ID, or INVALID_ZM_MODULE if not found.
 */
native ZMModule:ZM_GetModuleByPlugin(Handle:plugin);

/*____________________________________________________________________________*/

/**
 * Gets the plugin that created the specified module.
 *
 * @param module        Module to look up.
 *
 * @return              Handle to owner plugin, or INVALID_ZM_MODULE if not
 *                      found.
 * @error               Invalid module ID.
 */
native Handle:ZM_GetModuleOwner(ZMModule:module);

/*____________________________________________________________________________*/

/**
 * Looks up a module by the specified name.
 *
 * @param name          Name to search for.
 *
 * @return              Module ID, or INVALID_ZM_MODULE if not found.
 */
native ZMModule:ZM_GetModuleByName(const String:name[]);

/*____________________________________________________________________________*/

/**
 * Gets the module description.
 *
 * @param module        Module to read from.
 * @param buffer        Output buffer.
 * @param maxlen        Size of output buffer.
 *
 * @return              Number of characters written.
 * @error               Invalid module ID.
 */
native ZM_GetModuleDescription(ZMModule:module, String:buffer[], maxlen);

/*____________________________________________________________________________*/

/**
 * Sets the module brief description. This may be disabled by administration
 * commands.
 *
 * @param description   Description to set.
 *
 * @noreturn
 * @error               Caller has not created a module.
 */
native ZM_SetModuleDescription(const String:description[]);

/*____________________________________________________________________________*/

/**
 * Declares a feature identifier and assigns it to the caller module.
 *
 * Note: This native will also trigger the OnFeatureAdded event, if the event
 *       manager is initialized. Modules should consider using ZM_FeatureExists
 *       if the event manager is unavailable (as in the bootstrap phase).
 *
 * @param identifier    Global and unique name of the feature. Other modules
 *                      may declare dependency on this feature.
 *
 * @return              Feature ID.
 * @error               Name is blank or already in use. Caller has not created
 *                      a module.
 */
native ZMFeature:ZM_CreateFeature(const String:identifier[]);

/*____________________________________________________________________________*/

/**
 * Deletes and unregisters a feature in the caller module.
 *
 * Note: This native will also trigger the OnFeatureRemove event, if the event
 *       manager is initialized.
 *
 * @param feature       Feature to delete.
 *
 * @error               Invalid feature, or caller is not the feature owner.
 */
native ZM_DeleteFeature(ZMFeature:feature);

/*____________________________________________________________________________*/

/**
 * Gets a feature.
 *
 * @param identifier    Feature identifier string.
 *
 * @return              Feature ID if found, otherwise INVALID_ZM_FEATURE.
 */
native ZMFeature:ZM_GetFeature(const String:identifier[]);

/*____________________________________________________________________________*/

/**
 * Returns whether the specified feature exists.
 *
 * @param identifier    Feature identifier to search for.
 *
 * @return              True if feature exists, false otherwise.
 */
stock bool:ZM_FeatureExists(const String:identifier[])
{
    return ZM_GetFeature(identifier) != INVALID_ZM_FEATURE;
}

/*____________________________________________________________________________*/

/**
 * Gets the specified feature's owner module.
 *
 * @parma feature       Feature to look up.
 *
 * @return              Module ID if found, otherwise INVALID_ZM_MODULE.
 */
native ZMModule:ZM_GetFeatureOwner(ZMFeature:feature);

/*____________________________________________________________________________*/

/**
 * Registers an event hook for the OnFeatureAdded event.
 *
 * @param callback      Function to call when event is fired.
 *
 * @return              Event ID.
 * @error               Event is already hooked by the module, or event manager
 *                      is not initialized.
 */
native ZMEvent:ZM_HookFeatureAdded(ZM_OnFeatureAdded:callback);

/*____________________________________________________________________________*/

/**
 * Registers an event hook for the OnFeatureRemove event.
 *
 * @param callback      Function to call when event is fired.
 *
 * @return              Event ID.
 * @error               Event is already hooked by the module, or event manager
 *                      is not initialized.
 */
native ZMEvent:ZM_HookFeatureRemove(ZM_OnFeatureRemove:callback);
