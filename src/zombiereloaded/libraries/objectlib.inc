/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           objectlib.inc
 *  Type:           Library
 *  Description:    Key/value dynamic object storage manager with validation,
 *                  type safety and support for importing from Valve's KeyValue
 *                  file format.
 *
 *  Copyright (C) 2012  Richard Helgeby, Greyscale
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

#if defined _objectlib_included
 #endinput
#endif
#define _objectlib_included

#include <sourcemod>

#include "zombiereloaded/libraries/arrays.inc"
#include "zombiereloaded/utilities.inc"

/*____________________________________________________________________________*/

/**
 * Object tag.
 */
enum Object
{
    INVALID_OBJECT = 0
}

/*____________________________________________________________________________*/

/**
 * Number of bytes reserved for key names.
 */
#define OBJECT_KEY_NAME_LEN     32

/*____________________________________________________________________________*/

/**
 * Number of bytes reserved for string buffers.
 */
#define OBJLIB_MAX_STRING_LEN   255

/*____________________________________________________________________________*/

/**
 * Object type descriptor tag.
 */
enum ObjectType
{
    INVALID_OBJECT_TYPE = 0
}

/*____________________________________________________________________________*/

/**
 * Collection tag. Collections are objects, but with its own tag so the compiler
 * will be able to distinguish them.
 */
enum Collection
{
    INVALID_COLLECTION = 0
}

/*____________________________________________________________________________*/

/**
 * Data types that objects can store.
 */
enum ObjectDataType
{
    ObjDataType_Invalid = -1,   /** Unspecified type. Only used by validators. */
    ObjDataType_Any,            /** Does not include arrays/strings. */
    ObjDataType_Cell,
    ObjDataType_Bool,
    ObjDataType_Float,
    ObjDataType_Handle,
    ObjDataType_Function,
    ObjDataType_Array,
    ObjDataType_String,
    ObjDataType_Object,
    ObjDataType_ObjectType
}

/*____________________________________________________________________________*/

/**
 * Results from constraint handlers.
 *
 * Some constraint handlers may override the value being set, such as when
 * converting from string names to identifiers (numbers).
 */
enum ObjectConstraintResult
{
    ObjConstraintResult_Invalid,    /** Value is invalid. */
    ObjConstraintResult_Valid,      /** Value is valid. */
    ObjConstraintResult_Overridden  /** Value is valid and overridden by constraint handler. */
}

/*____________________________________________________________________________*/

/**
 * Available lookup methods for lookup constraint handlers.
 */
enum ObjectLookupMethod
{
    /**
     * Use two ADT arrays where one contains entry names (strings) and the other
     * contains replacement values at the same indexes as entries in the first
     * array. The handler will search through the arrays for matches.
     */
    ObjLookupMethod_Array,
    
    /**
     * Use an ADT Trie where entry names (strings) are mapped to the replacement
     * value. Much faster than searching an array.
     */
    ObjLookupMethod_Trie,
    
    /**
     * Use a custom callback where the callback decides which value to use.
     * Useful for more advanced lookups (such as using a database). See the
     * ObjLib_StringLookup callback for more details.
     */
    ObjLookupMethod_Callback
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Object entries. Used internally to name array indexes.
 *
 * Possible future optimization:
 * Separate strings and numeric data. Currently, each entry has reserved the
 * same amount of space (block size). If an object only has one long string and
 * several number entries, there will be a lot of wasted space.
 *
 * Memory usage can be reduced by storing numeric data in a separate array with
 * block size of 1 cell.
 *
 * If further optmimization is required the data may be serialized, but in a way
 * that allow random access (both reading and writing).
 */
#define OBJECT_DATA_LEN         3   /** Number of elements below. */
enum ObjectData
{
    Object_Data = 0,    /** Data entry. Handle to raw data array. */
    Object_NullKey,     /** Handle to array that tell which key that is null (not initialized). */
    Object_MetaData     /** Object meta data entry. Handle to object type descriptor. */
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Object type entries. Used internally to name array indexes.
 */
#define OBJECT_TYPE_DATA_LEN    9   /** Number of elements below. */
enum ObjectTypeData
{
    ObjectType_Locked = 0,      /** Whether type descriptor is read only. */
    ObjectType_ParentObject,    /** Reference to parent object, if any. Used by mutable objects. */
    ObjectType_KeySize,         /** Block size of key name array. */
    ObjectType_BlockSize,       /** Block size of raw data array. */
    ObjectType_Keys,            /** Handle to array of key names (case sensitive). */
    ObjectType_NameIndex,       /** Handle to trie index of key names. */
    ObjectType_DataTypes,       /** Handle to array of data type for each value entry. */
    ObjectType_Constraints,     /** Handle to validation constraints info for each key. */
    ObjectType_ErrorHandler     /** General error handler. Optional. */
}

/*____________________________________________________________________________*/

/**
 * Error types. Used in error handler callbacks.
 */
enum ObjLibError
{
    // Generic errors. These may occour any time an object or type descriptor is
    // accessed.
    ObjLibError_InvalidKey,         /** Key name or index is invalid. */
    ObjLibError_TypeMismatch,       /** Data type does not match the expected type. */
    ObjLibError_NullDataKey,        /** Attempted to read from uninitialized key. */
    ObjLibError_KeyExist,           /** Attempted to create a key with an existing name. */
    ObjLibError_Immutable,          /** Object or Object type is immutable. */
    ObjLibError_ValidationError,    /** Data constraint violation. */
    
    // KeyValue parser errors. These errors are related to the tree structure
    // validation.
    ObjLibError_KvUnexpectedKey,            /** Key is not defined in type descriptor. */
    ObjLibError_KvUnexpectedSection,        /** Section is not defined in type descriptor. */
    ObjLibError_KvInvalidSection,           /** Section is supposed to be a key value pair. */
    ObjLibError_KvInvalidConstraint,        /** Constraint type is invalid or unknown. */
    ObjLibError_KvTypeMismatch,             /** Key type mismatch. */
    ObjLibError_KvCollectionTypeMismatch    /** Collection element type mismatch. */
}

/*____________________________________________________________________________*/

/**
 * Number of bytes reserved for various strings.
 */
#define OBJECTLIB_WHITELIST_LEN 64      /** Space reserved in whitelist/blacklist for string constraints. */

/*____________________________________________________________________________*/

/**
 * General error callback.
 *
 * The return value may be used to decide what to do next. In most cases it's
 * irrelevant and the value is ignored. The KeyValue parser is using the return
 * value to abort or continue parsing.
 *
 * It would be considered a good habit to return a proper value even if it's
 * ignored in some cases.
 *
 * @param typeDescriptor    Related type descriptor.
 * @param errorType         Type of error.
 * @param message           Error message.
 * @param object            Related object, if available.
 * @param data              Data bundle with additional data, if available.
 *                          If the error is originating from the KeyValue parser
 *                          it's a reference to the parser context object. You
 *                          may use ObjLib_KvBuildPath to get a path to the
 *                          current location in the tree.
 *
 * @return                  What to do next.
 *                          * Plugin_Handled - Error is handled and further
 *                            processing should be aborted.
 *                          * Plugin_Continue - Continue processing if possible.
 *                            This is useful to let the KeyValue parser continue
 *                            parsing remaining keys.
 *                          * Other values are treated as if Plugin_Handled was
 *                            returned, parsing will be aborted.
 */
functag public Action:ObjLib_ErrorHandler(ObjectType:typeDescriptor, ObjLibError:errorType, const String:message[], Object:object, Object:data);

/*____________________________________________________________________________*/

/**
 * Custom key validation callback.
 *
 * Use this to do custom validation of a key.
 *
 * It's also possible to override the value being stored by setting new values
 * in the specified object through the set-accessor functions. Remember to
 * return ObjConstraintResult_Overridden in this case.
 *
 * Note that all other custom constraints are disabled inside this callback.
 * This prevents infinite recursion if the value is overridden.
 *
 * @param object            Object being validated.
 * @param typeDescriptor    Object type (meta data).
 * @param keyName           Key being validated.
 * @param dataType          Type of key.
 * @param values            Value(s) to validate. If the value is a single value
 *                          it's stored at index 0.
 * @param size              Number of elements in values parameter.
 *
 * @return                  See ObjectConstraintResult. It returns whether the
 *                          value is valid, invalid or overridden.
 */
functag public ObjectConstraintResult:ObjLib_KeyValidator(Object:object, ObjectType:typeDescriptor, const String:keyName[], ObjectDataType:dataType, const any:values[], size);

/*____________________________________________________________________________*/

/**
 * Available lookup callbacks for the lookup constraint handler. Make sure you
 * use a callback that match the destination type (data type of replacement
 * parameter matching key data type in the object).
 */
funcenum ObjLibLookupCallback
{
    /**
     * Lookup callback for value lookup constraint handler. Used to convert a
     * string into a value.
     *
     * @param object            Object being validated.
     * @param index             Index of key, or index where collection element
     *                          will be added.
     * @param lookup            String value to look up.
     * @param replacement       (Output). Replacement value to store in object.
     *
     * @return                  True if lookup value was valid and successfully
     *                          converted, false otherwise.
     */
    bool:public(Object:object, index, const String:lookup[], &any:replacement),
    
    /**
     * Lookup callback for array lookup constraint handler. Used to convert a
     * string into an array.
     *
     * @param object            Object being validated.
     * @param index             Index of key, or index where collection element
     *                          will be added.
     * @param lookup            String value to look up.
     * @param replacement       (Output). Replacement array to store in object.
     * @param maxlen            Size of replacement array.
     *
     * @return                  Number of cells copied to replacement array if
     *                          value was valid, or -1 if value was invalid.
     */
    public(Object:object, index, const String:lookup[], any:replacement[], maxlen),
    
    /**
     * Lookup callback for string lookup constraint handler. Used to convert a
     * string into another string.
     *
     * @param object            Object being validated.
     * @param index             Index of key, or index where collection element
     *                          will be added.
     * @param lookup            String value to look up.
     * @param replacement       (Output). Replacement string to store in object.
     * @param maxlen            Size of replacement array.
     *
     * @return                  Number of characters copied to replacement array
     *                          if value was valid, or -1 if value was invalid.
     */
    public(Object:object, index, const String:lookup[], String:replacement[], maxlen)
};

/*____________________________________________________________________________*/

new ObjectCount = 0;
new ObjectTypeCount = 0;

/*____________________________________________________________________________*/


/************************
 *   Helper functions   *
 ************************/

/**
 * Creates a clone of a trie with the specified keys.
 *
 * @param trie      Handle to ADT trie.
 * @param keys      Handle to ADT array with strings of key names in the trie.
 * @param keySize   Max key name size (in bytes).
 *
 * @return          Handle to cloned trie. Must be closed with CloseHandle.
 */
stock Handle:ObjLib_CloneTrie(Handle:trie, Handle:keys, keySize)
{
    new Handle:newTrie = CreateTrie();
    
    // Loop through each key.
    new len = GetArraySize(keys);
    for (new i = 0; i < len; i++)
    {
        // Get key.
        decl String:key[keySize];
        GetArrayString(keys, i, key, keySize);
        
        // Get source value.
        new value;
        GetTrieValue(trie, key, value);
        
        // Set new key.
        SetTrieValue(newTrie, key, value);
    }
    
    return newTrie;
}

/*____________________________________________________________________________*/

// Object library core modules.
#include "zombiereloaded/libraries/objectlib/objecttype"
#include "zombiereloaded/libraries/objectlib/object"

#include "zombiereloaded/libraries/objectlib/constraints"
#include "zombiereloaded/libraries/objectlib/constrainthandler"

#include "zombiereloaded/libraries/objectlib/collection"

#include "zombiereloaded/libraries/objectlib/errorhandler"

// Accessors.
#include "zombiereloaded/libraries/objectlib/accessors"
#include "zombiereloaded/libraries/objectlib/collectionaccessors"

// Parser modules.
#include "zombiereloaded/libraries/objectlib/kvparser"

// Inspector tool modules.
#include "zombiereloaded/libraries/objectlib/inspector"
