/*
 * ============================================================================
 *
 *  Zombie API
 *
 *  File:           accessors.inc
 *  Description:    Resource accessor functions. Include resource manager
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

#if defined _zm_resourceaccessors_included
 #endinput
#endif
#define _zm_resourceaccessors_included

/*____________________________________________________________________________*/

/**
 * Returns whether the value in the specified attribute is null.
 *
 * Note: You should know when attributes are null. Avoid making designs that
 *       depends on this native, but it's available if you still need it.
 *
 * @param entity            Entity to inspect.
 * @param attributeName     Name of attribute to inspect.
 *
 * @return                  True if null, false otherwise.
 */
native bool:ZM_IsNullAttribute(ZMEntity:entity, const String:attributeName[]);

/*____________________________________________________________________________*/

/**
 * Sets an entity attribute to null.
 *
 * @param entity            Entity to update.
 * @param attributeName     Attribute to clear.
 */
native ZM_ClearAttribute(ZMEntity:entity, const String:attributeName[]);

/*____________________________________________________________________________*/

/**
 * Gets a cell value from an entity attribute.
 *
 * @param entity            Entity to retrieve data from.
 * @param attributeName     Name of attribute to read.
 *
 * @return                  Cell value in attribute.
 * @error                   Invalid entity, attribute, null attribute, or
 *                          attribute type mismatch.
 */
native ZM_GetEntityCell(ZMEntity:entity, const String:attributeName[]);

/*____________________________________________________________________________*/

/**
 * Sets a cell value in an entity attribute.
 *
 * @param entity            Entity to update.
 * @param attributeName     Name of attribute to write to.
 * @param value             Value to write.
 *
 * @noreturn
 * @error                   Invalid entity, attribute, or attribute type
 *                          mismatch.
 */
native ZM_SetEntityCell(
        ZMEntity:entity,
        const String:attributeName[],
        any:value);

/*____________________________________________________________________________*/

/**
 * Gets a boolean value from an entity attribute.
 *
 * @param entity            Entity to retrieve data from.
 * @param attributeName     Name of attribute to read.
 *
 * @return                  Boolean value in attribute.
 * @error                   Invalid entity, attribute, null attribute, or
 *                          attribute type mismatch.
 */
native bool:ZM_GetEntityBool(ZMEntity:entity, const String:attributeName[]);

/*____________________________________________________________________________*/

/**
 * Sets a boolean value in an entity attribute.
 *
 * @param entity            Entity to update.
 * @param attributeName     Name of attribute to write to.
 * @param value             Value to write.
 *
 * @noreturn
 * @error                   Invalid entity, attribute, or attribute type
 *                          mismatch.
 */
native ZM_SetEntityBool(
        ZMEntity:entity,
        const String:attributeName[],
        bool:value);

/*____________________________________________________________________________*/

/**
 * Gets a float value from an entity attribute.
 *
 * @param entity            Entity to retrieve data from.
 * @param attributeName     Name of attribute to read.
 *
 * @return                  Boolean value in attribute.
 * @error                   Invalid entity, attribute, null attribute, or
 *                          attribute type mismatch.
 */
native Float:ZM_GetEntityFloat(ZMEntity:entity, const String:attributeName[]);

/*____________________________________________________________________________*/

/**
 * Sets a float value in an entity attribute.
 *
 * @param entity            Entity to update.
 * @param attributeName     Name of attribute to write to.
 * @param value             Value to write.
 *
 * @noreturn
 * @error                   Invalid entity, attribute, or attribute type
 *                          mismatch.
 */
native ZM_SetEntityFloat(
        ZMEntity:entity,
        const String:attributeName[],
        Float:value);

/*____________________________________________________________________________*/

/**
 * Gets a one-dimensional array from an entity attribute.
 *
 * @param entity            Entity to retrieve data from.
 * @param attributeName     Name of attribute to read.
 * @param buffer            (Output) Buffer where attribute value is copied to.
 * @param maxlen            Size of buffer.
 *
 * @return                  Number of cells written to buffer.
 * @error                   Invalid entity, attribute, null attribute, or
 *                          attribute type mismatch.
 */
native ZM_GetEntityArray(
        ZMEntity:entity,
        const String:attributeName[]
        any:buffer[],
        maxlen);

/*____________________________________________________________________________*/

/**
 * Sets a one-dimensional array in an entity attribute.
 *
 * @param entity            Entity to update.
 * @param attributeName     Name of attribute to write to.
 * @param value             Array to write.
 * @param size              Size of array.
 *
 * @return                  Number of cells written in attribute.
 * @error                   Invalid entity, attribute, or attribute type
 *                          mismatch.
 */
native ZM_SetEntityArray(
        ZMEntity:entity,
        const String:attributeName[],
        const any:value[],
        size);

/*____________________________________________________________________________*/

/**
 * Gets a string value from an entity attribute.
 *
 * @param entity            Entity to retrieve data from.
 * @param attributeName     Name of attribute to read.
 * @param buffer            (Output) String buffer where attribute value is
 *                          copied to.
 * @param maxlen            Size of string buffer.
 *
 * @return                  Number of characters written to buffer.
 * @error                   Invalid entity, attribute, null attribute, or
 *                          attribute type mismatch.
 */
native ZM_GetEntityString(
        ZMEntity:entity,
        const String:attributeName[]
        String:buffer[],
        maxlen);

/*____________________________________________________________________________*/

/**
 * Sets a string value in an entity attribute.
 *
 * Note: This set function handles a special case with lookup constraints on
 *       attributes. Instead of triggering a type mismatch error, lookup
 *       constraints may be able to convert the value. See lookup constraints
 *       for details.
 *
 *       For instance, if the attribute type actually is a boolean and has
 *       lookup constraints assigned to it, it's possible to write a string
 *       (such as "yes" or "no") and let the attribute's lookup constraint
 *       handler convert the value into a boolean.
 *
 * @param entity            Entity to update.
 * @param attributeName     Name of attribute to write to.
 * @param value             Value to write.
 *
 * @return                  Number of characters written in attribute.
 * @error                   Invalid entity, attribute, or attribute type
 *                          mismatch.
 */
native ZM_SetEntityString(
        ZMEntity:entity,
        const String:attributeName[],
        const String:value[]);

/*____________________________________________________________________________*/

/**
 * Gets an entity reference from an entity attribute.
 *
 * @param entity            Entity to retrieve data from.
 * @param attributeName     Name of attribute to read.
 *
 * @return                  Entity reference in attribute.
 * @error                   Invalid entity, attribute, null attribute, or
 *                          attribute type mismatch.
 */
native ZMEntity:ZM_GetEntityReference(
        ZMEntity:entity,
        const String:attributeName[]);

/*____________________________________________________________________________*/

/**
 * Sets an entity reference in an entity attribute.
 *
 * @param entity            Entity to update.
 * @param attributeName     Name of attribute to write to.
 * @param value             Entity reference to write.
 *
 * @noreturn
 * @error                   Invalid entity, attribute, or attribute type
 *                          mismatch.
 */
native ZM_SetEntityReference(
        ZMEntity:entity,
        const String:attributeName[],
        ZMEntity:value);

/*____________________________________________________________________________*/

/**
 * Gets an entity reference type from an entity attribute.
 *
 * @param entity            Entity to retrieve data from.
 * @param attributeName     Name of attribute to read.
 *
 * @return                  Entity type in attribute.
 * @error                   Invalid entity, attribute, null attribute, or
 *                          attribute type mismatch.
 */
native ZMEntityType:ZM_GetEntityReferenceType(
        ZMEntity:entity,
        const String:attributeName[]);

/*____________________________________________________________________________*/

/**
 * Sets an entity type in an entity attribute.
 *
 * @param entity            Entity to update.
 * @param attributeName     Name of attribute to write to.
 * @param value             Entity type to write.
 *
 * @noreturn
 * @error                   Invalid entity, attribute, or attribute type
 *                          mismatch.
 */
native ZM_SetEntityReferenceType(
        ZMEntity:entity,
        const String:attributeName[],
        ZMEntityType:value);
