/*
 * ============================================================================
 *
 *  Zombie API
 *
 *  File:           repository.inc
 *  Description:    Repository utilities. Include resource manager instead of
 *                  this file.
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

#if defined _zm_repository_included
 #endinput
#endif
#define _zm_repository_included

/*____________________________________________________________________________*/

/**
 * Creates a new named repository capable of storing entities of the specified
 * type.
 *
 * @param name          Unique identifier for the repository.
 * @param entityType    The type of data structure that is stored.
 * @param isPrivate     (Optional) Whether the repository is only accessible by
 *                      the module (plugin) that created it.
 *
 * @return              Repository ID.
 * @error               Invalid name, type or name already in use.
 */
native ZMRepository:ZM_CreateRepository(
        const String:name[],
        ZMEntityType:entityType,
        bool:isPrivate = false);

/*____________________________________________________________________________*/

/**
 * Gets a repository by name.
 *
 * @param name          Repository name.
 *
 * @return              Repository ID or INVALID_ZM_REPOSITORY if not found.
 */
native ZMRepository:ZM_GetRepository(const String:name[]);

/*____________________________________________________________________________*/

/**
 * Deletes the specified repository.
 *
 * Warning: Does not delete entities in the repository. Those must be deleted
 *          individually.
 *
 * Warning: All references to this repository will become invalid and cause an
 *          error if accessed.
 *
 * @param repository    Repository ID.
 *
 * @noreturn
 * @error               Invalid repository ID.
 */
native ZM_DeleteRepository(ZMRepository:repository);

/*____________________________________________________________________________*/

/**
 * Gets the number of entities stored in the specified repository.
 *
 * @param repository    Repository ID.
 *
 * @return              Number of entities in repository.
 * @error               Invalid repository ID.
 */
native ZM_GetRepositorySize(ZMRepository:repository);

/*____________________________________________________________________________*/

/**
 * Gets an entity at the specified index in a repository.
 *
 * @param repository    Repository ID.
 * @param index         Zero-based index of element in repository.
 *
 * @return              Entity at the specified index.
 * @error               Invalid repository ID, or index out of bounds.
 */
native ZMEntity:ZM_GetEntityAt(ZMRepository:repository, index);

/*____________________________________________________________________________*/

/**
 * Creates a new repository index on the specified attribute.
 *
 * An index enables faster lookup of an entity. It's useful if the collection is
 * large.
 *
 * @param repository    Repository to attach to.
 * @param attribute     Attribute to index.
 *
 * @return              Repository index reference.
 * @error               Invalid repository ID, invalid attribute name, attribute
 *                      is not a string, or attribute is already indexed.
 */
native ZMIndex:ZM_CreateRepositoryIndex(
        ZMRepository:repository,
        const String:attribute[]);

/*____________________________________________________________________________*/

/**
 * Deletes a repository index.
 *
 * @param repository    Repository ID.
 * @param attribute     Attribute to remove index on.
 *
 * @noreturn
 * @error               Invalid repository ID, or attribute is not indexed.
 */
native ZM_DeleteRepositoryIndex(
        ZMRepository:repository,
        const String:attribute[]);

/*____________________________________________________________________________*/

/**
 * Gets a list of indexed attributes for the specified repository. If there are
 * no indexes an empty array will be returned.
 *
 * @param repository    Repository ID.
 *
 * @return              ADT Array with strings of attribute names that have an
 *                      index. Must be closed with CloseHandle when no longer in
 *                      use.
 * @error               Invalid repository ID.
 */
native Handle:ZM_GetIndexedAttributes(ZMRepository:repository);

/*____________________________________________________________________________*/

/**
 * Gets a repository index reference for the specified attribute.
 *
 * @param repository    Repository ID.
 * @param attribute     Indexed attribute name.
 *
 * @return              Repository index.
 * @error               Invalid repository ID, or attribute is not indexed.
 */
native ZMIndex:ZM_GetRepositoryIndex(
        ZMRepository:repository,
        const String:attribute[]);

/*____________________________________________________________________________*/

/**
 * Gets an entity by string lookup using the specified repository index.
 *
 * @param repositoryIndex   Repository index to use.
 * @param lookup            String value to look up.
 *
 * @return                  Entity if found, otherwise INVALID_ZM_ENTITY.
 * @error                   Invalid repository index. Empty lookup value.
 */
native ZMEntity:ZM_GetEntityByString(
        ZMIndex:repositoryIndex,
        const String:lookup[]);
