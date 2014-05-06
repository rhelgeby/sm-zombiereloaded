/*
 * ============================================================================
 *
 *  Zombie API
 *
 *  File:           constraints.inc
 *  Description:    Resource constraint builders. Include resource manager
 *                  instead of this file.
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

#if defined _zm_resourceconstraints_included
 #endinput
#endif
#define _zm_resourceconstraints_included

/*____________________________________________________________________________*/

/**
 * File validation options.
 */
enum ZMFileValidation
{
    ZMFileValidation_None,          /** No validation. */
    ZMFileValidation_FullPath,      /** File and directory path. */
    ZMFileValidation_DirectoryOnly  /** Just directory path. */
}

/*____________________________________________________________________________*/

/**
 * Deletes a constraint definition.
 *
 * Note:    This function will close handles passed to constraint builder.
 *
 * Warning: Any use of the constraint reference after deletion will trigger an
 *          error. It's recommended to reset the references (enabled by
 *          default).
 *
 * @param constraint        Constraint to delete.
 * @param resetReference    (Optional) Sets constraint to INVALID_ZM_CONSTRAINT
 *                          when constraint is deleted (default).
 *
 * @noreturn
 */
native ZM_DeleteConstraint(
        &ZMConstraint:constraint,
        bool:resetReference = true);

/*____________________________________________________________________________*/

/**
 * Creates a cell constraint definition. All parameters are optional and
 * defaults to no limit.
 *
 * @param nonzero           Cannot be zero.
 * @param lowerLimit        Enable lower limit.
 * @param upperLimit        Enable upper limit.
 * @param min               Lower limit. Cannot be higher than max.
 * @param max               Upper limit. Cannot be lower than min.
 *
 * @return                  Constraint definition. Must be deleted with
 *                          ZM_DeleteConstraint when no longer used.
 * @error                   Overlapping max and min values.
 */
native ZMConstraint:ZM_CreateCellConstraint(
        bool:nonzero = false,
        bool:lowerLimit = false,
        bool:upperLimit = false,
        min = 0,
        max = 0);

/*____________________________________________________________________________*/

/**
 * Creates a float constraint definition. All parameters are optional and
 * defaults to no limit.
 *
 * @param nonzero           Cannot be zero.
 * @param lowerLimit        Enable lower limit.
 * @param upperLimit        Enable upper limit.
 * @param min               Lower limit. Cannot be higher than max.
 * @param max               Upper limit. Cannot be lower than min.
 * @param zeroDelta         Maximum difference allowed when comparing against
 *                          zero.
 *
 * @return                  Constraint definition. Must be deleted with
 *                          ZM_DeleteConstraint when no longer used.
 * @error                   Overlapping max and min values.
 */
native ZMConstraint:ZM_CreateFloatConstraint(
        bool:nonzero = false,
        bool:lowerLimit = false,
        bool:upperLimit = false,
        Float:min = 0.0,
        Float:max = 0.0,
        Float:zeroDelta = 0.0001);

/*____________________________________________________________________________*/

/**
 * Creates a string constraint definition. All parameters are optional and
 * defaults to no limit.
 *
 * @param nonempty          Cannot be empty.
 * @param lowerLimit        Enable minimum length limit.
 * @param upperLimit        Enable maximum lenght limit.
 * @param min               Minimum length. Cannot be higher than max.
 * @param max               Maximum length. Cannot be lower than min.
 * @param fileValidation    File validation mode.
 * @param includeValveFS    Include files in Valve's file system when using
 *                          fileValidation.
 *
 * @return                  Constraint definition. Must be deleted with
 *                          ZM_DeleteConstraint when no longer used.
 * @error                   Overlapping length limits.
 */
native ZMConstraint:ZM_CreateStringConstraint(
        bool:nonempty = false,
        bool:lowerLimit = false,
        bool:upperLimit = false,
        minLen = 0,
        maxLen = 0,
        ZMFileValidation:fileValidation = ZMFileValidation_None,
        bool:includeValveFS = false);

/*____________________________________________________________________________*/

/**
 * Creates an array constraint definition. All parameters are optional and
 * defaults to no limit.
 *
 * @param lowerLimit        Enable minimum length limit.
 * @param upperLimit        Enable maximum lenght limit.
 * @param min               Minimum length. Cannot be higher than max.
 * @param max               Maximum length. Cannot be lower than min.
 *
 * @return                  Constraint definition. Must be deleted with
 *                          ZM_DeleteConstraint when no longer used.
 * @error                   Overlapping length limits.
 */
native ZMConstraint:ZM_CreateArrayConstraint(
        bool:lowerLimit = false,
        bool:upperLimit = false,
        minLen = 0,
        maxLen = 0);

/*____________________________________________________________________________*/

/**
 * Creates an entity constraint definition. Can be used to require entities of a
 * certain type.
 *
 * All parameters are optional and defaults to no limit.
 *
 * @param nonzero           Cannot be zero (INVALID_ZM_ENTITY_TYPE).
 * @param type              Must be a certain type. Use INVALID_ZM_ENTITY_TYPE
 *                          to skip type validation.
 *
 * @return                  Constraint definition. Must be deleted with
 *                          ZM_DeleteConstraint when no longer used.
 */
native ZMConstraint:ZM_CreateEntityConstraint(
        bool:nonzero = false,
        ZMEntityType:type = INVALID_ZM_ENTITY_TYPE);

/*____________________________________________________________________________*/

/**
 * Creates an entity type constraint definition. This can be used to require
 * certain attributes to exist in the entity type without requiring a complete
 * entity type.
 *
 * This constraint can also be used on attributes that store entity references.
 *
 * All parameters are optional and defaults to no limit.
 *
 * @param nonzero           Cannot be zero (INVALID_ZM_ENTITY).
 * @param requireAttributes Require certain attributes to exist.
 * @param attributes        ADT array with list of attributes names to require.
 * @param dataTypes         ADT array with matching data types of attributes in
 *                          attribute list. See ZMEntityDataType enumeration.
 *
 * @return                  Constraint definition. Must be deleted with
 *                          ZM_DeleteConstraint when no longer used.
 * @error                   Missing list of key names or data types, or list
 *                          lengths doesn't match.
 */
native ZMConstraint:ZM_CreateEntityTypeConstraint(
        bool:nonzero = false,
        bool:requireAttributes = false,
        Handle:attributes = INVALID_HANDLE,
        Handle:dataTypes = INVALID_HANDLE);

/*____________________________________________________________________________*/

/**
 * Creates a custom constraint definition. All parameters are optional and
 * defaults to no limit.
 *
 * @param callback          Callback to custom validation handler.
 *
 * @return                  Constraint definition. Must be deleted with
 *                          ZM_DeleteConstraint when no longer used.
 */
native ZMConstraint:ZM_CreateCustomConstraint(ZMKeyValidator:callback);

/*____________________________________________________________________________*/

/**
 * Creates a lookup constraint definition. This constraint will look up a string
 * value and convert it to a replacement value (such as an identifier to a
 * numer).
 *
 * Set the value using the ZM_SetEntityString function. It will use this
 * constraint to convert the string according to the constraint parameters
 * below.
 *
 * @param method        Lookup method. See ZMLookupMethod.
 *
 * @param entries       (Optional) Lookup entries. Usage depends on method:
 *                      * Array    - ADT array with entry names as strings.
 *                      * Trie     - ADT trie with entry names mapped to
 *                                   replacement values.
 *                      * Callback - (Ignored)
 *
 * @param values        (Optional) Replacement values matching the attribute
 *                      data type.
 *                      Usage depends on method:
 *                      * Array    - ADT array with replacement values at
 *                                   matching indexes as entry names in entries
 *                                   array. Strings and one-dimensional arrays
 *                                   are supported too.
 *                      * Trie     - (Ignored)
 *                      * Callback - (Ignored)
 *
 * @param callback      Lookup callback to use, if callback method is used. The
 *                      callback must match the data type (value, array, or
 *                      string). Ignored when using other methods.
 *
 * @param subConstraints    (Optional) Constraint definition to use on
 *                          destination data type.
 *
 * @return              Constraint definition. Must be deleted with
 *                      ZM_DeleteConstraint when no longer used.
 * @error               Empty list of entries/values. Entry and value list size
 *                      doesn't match. Invalid callback. Invalid sub constraint
 *                      definition.
 */
native ZMConstraint:ZM_CreateLookupConstraint(
        ZMLookupMethod:method,
        Handle:entries = INVALID_HANDLE,
        Handle:values = INVALID_HANDLE,
        ZMLookupCallback:callback = INVALID_FUNCTION,
        ZMConstraint:subConstraints = INVALID_ZM_CONSTRAINT);
