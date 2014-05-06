/*
 * ============================================================================
 *
 *  Zombie API
 *
 *  File:           resourcemanager.inc
 *  Description:    API for managing general data resources.
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

#if defined _zm_resourcemanager_included
 #endinput
#endif
#define _zm_resourcemanager_included

#include <zombie/core/resourcemanager/accessors>
#include <zombie/core/resourcemanager/constraints>
#include <zombie/core/resourcemanager/repository>

/*____________________________________________________________________________*/

/*

RESOURCE MANAGER

The resource manager is a common module that stores user defined entities and
repositories. It's quite similar to a memory-database, but entities in the
resource manager is accessed directly instead of through database queries.

It supports references between entities and enables data to be stored in a
organized and typesafe manner.

Don't expect the resource manager to be super efficient due to run-time lookup
of attributes and validation. Otherwise the performance depends on the
implementation, but the goal to be good enough. It's recommended to cache data
that's accessed frequently, such as in damage/bullet events, fast timers or
other busy events.

Feature modules will be responsible for declaring entity types and repositories.
The resource manager may help with loading KeyValue files into repositories. If
modules assign constraints it will also validate the data to ensure only valid
data is written to the repositories.

For live debugging purposes an inspector tool may be available, but this is
optional and may not be implemented.


TERMINOLOGY

Entity:
A simple data structure with named attributes of specific types; an object.

Entity type:
Meta data about an entity. Stores the actual attribute names, data types and
constraints. Shared between entities of the same type.

Constraint:
A validation rule applied to a single value. Constraints may be assigned to
entity attributes to ensure data integrity.

Repository:
A collection of entities of a certain type.

*/

/*____________________________________________________________________________*/

/**
 * Default number of cells reserved for key names.
 */
#define ZM_RESOURCE_KEY_SIZE 8      // 32 bytes (including null terminator)

/*____________________________________________________________________________*/

/**
 * Valid data types for entities.
 */
enum ZMEntityDataType
{
    ZMEntityDataType_Cell,          /** Cell or any singular value. */
    ZMEntityDataType_Bool,
    ZMEntityDataType_Float,
    ZMEntityDataType_Array,         /** One-dimensional array. */
    ZMEntityDataType_String,
    ZMEntityDataType_Reference,     /** Reference to another entity. */
    ZMEntityDataType_ReferenceType  /** Reference to an entity type. */
}

/*____________________________________________________________________________*/

/**
 * Entity type ID.
 */
enum ZMEntityType
{
    INVALID_ZM_ENTITY_TYPE = -1
}

/*____________________________________________________________________________*/

/**
 * Entity attribute ID.
 */
enum ZMEntityAttribute
{
    INVALID_ZM_ENTITY_ATTRIBUTE = -1;
}

/*____________________________________________________________________________*/

/**
 * Entity constraint ID.
 */
enum ZMConstraint
{
    INVALID_ZM_CONSTRAINT = -1
}

/*____________________________________________________________________________*/

/**
 * Entity ID.
 */
enum ZMEntity
{
    INVALID_ZM_ENTITY = -1
}

/*____________________________________________________________________________*/

/**
 * Repository ID.
 */
enum ZMRepository
{
    INVALID_ZM_REPOSITORY = -1
}

/*____________________________________________________________________________*/

/**
 * Results from constraint handlers.
 *
 * Some constraint handlers may override the value being set, such as lookup
 * constraints.
 */
enum ZMConstraintResult
{
    ZMConstraintResult_Invalid,     /** Value is invalid. */
    ZMConstraintResult_Valid,       /** Value is valid. */
    ZMConstraintResult_Overridden   /** Value is valid and overridden by constraint handler. */
}

/*____________________________________________________________________________*/

/**
 * Custom key validation callback. Use this if you need custom validation of an
 * entity attribute.
 *
 * It's also possible to override the value being stored by setting new values
 * in the specified entity through the set-accessor functions. Remember to
 * return ZMConstraintResult_Overridden in this case.
 *
 * Note that all other custom constraints are disabled inside this callback.
 * This prevents infinite recursion if the value is overridden.
 *
 * @param entity            Entity being validated.
 * @param entityType        Entity type (meta data).
 * @param attributeName     Attribute being validated.
 * @param dataType          Data type of attribute.
 * @param values            Value(s) to validate. Singular values (cell, float,
 *                          etc.) are stored at index 0.
 * @param size              Number of elements in values parameter (length of
 *                          string or array). For singular values it's always 1.
 *
 * @return                  Specifies whether the value was valid, invalid or
 *                          overridden. See ZMConstraintResult.
 */
functag public ZMConstraintResult:ZMKeyValidator(
        Entity:entity,
        EntityType:entityType,
        const String:attributeName[],
        ZMEntityDataType:dataType,
        const any:values[],
        size);

/*____________________________________________________________________________*/

/**
 * Available lookup methods for lookup constraint handlers.
 */
enum ZMLookupMethod
{
    /**
     * Use two ADT arrays where one contains entry names (strings) and the other
     * contains replacement values at the same indexes as entries in the first
     * array. The handler will search through the array for matching names.
     */
    ZMLookupMethod_Array,
    
    /**
     * Use an ADT Trie where entry names (strings) are mapped to the replacement
     * value. Faster than searching an array.
     */
    ZMookupMethod_Trie,
    
    /**
     * Use a custom callback where the callback decides which value to use.
     * Useful for more advanced lookups (such as using a database). See the
     * ObjLib_StringLookup callback for more details.
     */
    ZMLookupMethod_Callback
}

/*____________________________________________________________________________*/


/**
 * Available lookup callbacks for the lookup constraint handler. Make sure you
 * use a callback that match the destination type (data type of replacement
 * parameter matching attribute data type in the entity).
 */
funcenum ZMLookupCallback
{
    /**
     * Lookup callback for value lookup constraint handler. Used to convert a
     * string into a value.
     *
     * @param entity            Entity being validated.
     * @param attribute         Entity attribute ID.
     * @param lookup            String value to look up.
     * @param replacement       (Output). Replacement value to store in entity.
     *
     * @return                  True if lookup value was valid and successfully
     *                          converted, false otherwise.
     */
    bool:public(
            Entity:entity,
            ZMEntityAttribute:attribute,
            const String:lookup[],
            &any:replacement),
    
    /**
     * Lookup callback for array lookup constraint handler. Used to convert a
     * string into an array.
     *
     * @param entity            Entity being validated.
     * @param attribute         Entity attribute ID.
     * @param lookup            String value to look up.
     * @param replacement       (Output). Replacement array to store in entity.
     * @param maxlen            Size of replacement array.
     *
     * @return                  Number of cells copied to replacement array if
     *                          value was valid, or -1 if value was invalid.
     */
    public(
            Entity:entity,
            ZMEntityAttribute:attribute,
            const String:lookup[],
            any:replacement[],
            maxlen),
    
    /**
     * Lookup callback for string lookup constraint handler. Used to convert a
     * string into another string.
     *
     * @param entity            Entity being validated.
     * @param attribute         Entity attribute ID.
     * @param lookup            String value to look up.
     * @param replacement       (Output). Replacement string to store in entity.
     * @param maxlen            Size of replacement array.
     *
     * @return                  Number of characters copied to replacement
     *                          string if value was valid, or -1 if value was
     *                          invalid.
     */
    public(
            Entity:entity,
            ZMEntityAttribute:attribute,
            const String:lookup[],
            String:replacement[],
            maxlen)
};

/*____________________________________________________________________________*/

/**
 * Creates a new named entity type.
 *
 * @param name          Unique identifier for the entity type.
 * @param blockSize     Number of cells reserved for each attribute value. This
 *                      defines the maximum size of arrays or strings that can
 *                      be stored in attributes.
 * @param keySize       (Optional) Number of cells reserved for attribute names.
 *                      Default is 8 cells (32 bytes).
 *
 * @return              Entity type ID. Must be deleted with ZM_DeleteEntityType
 *                      when no longer in use.
 * @error               Invalid parameter value, or name already in use.
 */
native ZMEntityType:ZM_CreateEntityType(
        const String:name[],
        blockSize = 1,
        keySize = ZM_RESOURCE_KEY_SIZE);

/*____________________________________________________________________________*/

/**
 * Gets an entity type by name.
 *
 * @param name          Name of entity type.
 *
 * @return              Entity type ID, or INVALID_ZM_ENTITY_TYPE if not found.
 */
native ZMEntityType:ZM_GetEntityType(const String:name[]);

/*____________________________________________________________________________*/

/**
 * Deletes the specified entity type.
 *
 * Warning: All references to this entity type will become invalid and cause an
 *          error if accessed.
 *
 * @param type              Entity type ID.
 * @param resetReference    (Optional) Reset type to INVALID_ZM_ENTITY_TYPE when
 *                          deleted. Default is true.
 * @param deleteConstraints (Optional) Delete constraint definitions attached to
 *                          attributes. Default is false.
 *
 * @noreturn
 */
native ZM_DeleteEntityType(
        ZMEntityType:type,
        bool:resetReference = true,
        bool:deleteConstraints = false);

/*____________________________________________________________________________*/

/**
 * Creates a new entity of the specified type with null values.
 *
 * @param type          Entity type ID.
 *
 * @return              Entity ID.
 */
native ZMEntity:ZM_CreateEntity(ZMEntityType:type);

/*____________________________________________________________________________*/

/**
 * Deletes the specified entity.
 *
 * Warning: All references to this entity will become invalid and cause an error
 *          if accessed.
 *
 * @param entity        Entity ID.
 *
 * @noreturn
 * @error               Invalid entity ID.
 */
native ZM_DeleteEntity(ZMEntity:entity);

/*____________________________________________________________________________*/

/**
 * Adds an attribute to the specified entity type.
 *
 * @param type          Entity type to add attribute to.
 * @param name          Name of attribute. Must be unique for the entity type.
 * @param dataType      Data type of attribute.
 * @param constraint    (Optional) Attribute constraints (max/min, etc).
 *
 * @return              Attribute ID.
 * @error               Invalid entity type, data type, or name is already in
 *                      use for this type.
 */
native ZMEntityAttribute:ZM_AddEntityAttribute(
        ZMEntityType:type,
        const String:name[],
        ZMEntityDataType:dataType,
        ZMConstraint:constraint = INVALID_ZM_CONSTRAINT);

/*____________________________________________________________________________*/

/**
 * Returns the ID of an attribute in the specified entity type.
 *
 * @param type          Entity type to inspect.
 * @param name          Name of attribute to look up.
 *
 * @return              Attribute ID if found, otherwise
 *                      INVALID_ZM_ENTITY_ATTRIBUTE.
 * @error               Invalid entity type.
 */
native ZMEntityAttribute:ZM_GetAttributeID(
        ZMEntityType:type,
        const String:name[]);

/*____________________________________________________________________________*/

/*
TODO:
* Validation helpers (to avoid repeated code in accessor functions)
* Load from file (keyvalues)
* Need to handle reloading of modules that attempts to recreate repositories
* Resource events: entity modified, added, removed, etc.
* Event listeners for single entites (may be expensive).
*/
