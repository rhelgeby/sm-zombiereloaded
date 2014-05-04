/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           kvparser.inc
 *  Type:           Library
 *  Description:    Valve KeyValue format parser for objectlib.
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

#include "zombiereloaded/libraries/utilities"
#include "zombiereloaded/libraries/arrays"

/*____________________________________________________________________________*/

/**
 * Enables fallback error handler. Disable to let errors result in a call to
 * ThrowError (and dump a stack trace).
 */
#define OBJLIB_KV_SOFT_ERROR

/*____________________________________________________________________________*/

/**
 * Length of string buffers used in parser. Increase this if you need to read
 * longer string values.
 */
#define OBJLIB_KV_MAX_STRING_LEN    255

/*____________________________________________________________________________*/

/**
 * Length of key name buffers used in parser.
 *
 * Note: Don't reserve too much space. The parser is recursive and very large
 *       key name buffers will consume a lot of stack space if the keyvalue tree
 *       is deep.
 */
#define OBJLIB_KV_MAX_KEY_LEN    64

/*____________________________________________________________________________*/

/**
 * Empty object type template with enough space reserved for strings.
 */
new ObjectType:ObjLib_KvObjectType = INVALID_OBJECT_TYPE;
new bool:ObjLib_KvObjectTypeBuilt = false;

/*____________________________________________________________________________*/

/**
 * Object type template for parser context object.
 */
new ObjectType:ObjLib_KvContextType = INVALID_OBJECT_TYPE;
new bool:ObjLib_KvContextTypeBuilt = false;

/*____________________________________________________________________________*/

/**
 * Parser states. The parser will check the state in parseParams on every
 * iteration.
 */
enum ObjLibParseState
{
    ObjLibState_Stopped,    /** Stop parsing. */
    ObjLibState_Running,    /** Continue parsing. */
    ObjLibState_Aborted,    /** Abort parser due to error. */
    
    // Signals.
    ObjLibState_SkipSection /** Parser should skip current section. */
}

/*____________________________________________________________________________*/

/**
 * Temporary global storage for parser context. Used when the parser context
 * can't be passed to the error handler directly, such as in general objectlib
 * errors.
 */
new Object:TempParseContext = INVALID_OBJECT;

/*____________________________________________________________________________*/

#include "libraries/objectlib/kverrors"

/*____________________________________________________________________________*/

/**
 * Creates a parse context object. This object store the state and various
 * settings required by the parser.
 *
 * @param name              (Optional) Parser name. Useful to identify parser in
 *                          error logs or error handlers.
 * @param rootType          (Optional) Initial type to use when parsing the
 *                          first section. Default is no type; build type while
 *                          parsing.
 * @param addEmptySections  (Optional) Whether to add sections with no keys.
 *                          An empty object will represent an empty section.
 *                          Default is false, not adding.
 * @param continueOnError   (Optional) Continue parsing remaining keys/sections
 *                          if there's an error parsing an element. The element
 *                          that failed will be skipped. Default is true.
 * @param errorHandler      (Optional) Custom error handler. Overrides any other
 *                          error handler if specified. Default is none, use
 *                          built in error handler.
 *
 * @return                  Parse context object. Must be deleted with
 *                          ObjLib_DeleteParseContext when no longer in use.
 */
stock Object:ObjLib_GetParseContext(const String:name[] = "",
                                    ObjectType:rootType = INVALID_OBJECT_TYPE,
                                    bool:ignoreEmptySections = false,
                                    bool:continueOnError = true,
                                    ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Note: See comments in ObjLib_BuildKvContextType for details about the
    //       parse context attributes.
    
    // Make sture type is ready.
    ObjLib_BuildKvContextType();
    
    new Object:parseContext = ObjLib_CreateObject(ObjLib_KvContextType);
    
    // Set parameters.
    ObjLib_SetObjectType(parseContext, "rootType", rootType);
    ObjLib_SetBool(parseContext, "ignoreEmptySections", ignoreEmptySections);
    ObjLib_SetBool(parseContext, "continueOnError", continueOnError);
    ObjLib_SetFunction(parseContext, "errorHandler", errorHandler);
    ObjLib_SetCell(parseContext, "parseState", ObjLibState_Running);
    if (strlen(name) > 0)
    {
        ObjLib_SetString(parseContext, "name", name);
    }
    
    // Create stacks.
    new Handle:objectStack = CreateArray();
    new Handle:typeStack = CreateArray();
    new Handle:nameStack = CreateArray(ByteCountToCells(PLATFORM_MAX_PATH));
    
    // Set stacks.
    ObjLib_SetHandle(parseContext, "objectStack", objectStack);
    ObjLib_SetHandle(parseContext, "typeStack", typeStack);
    ObjLib_SetHandle(parseContext, "nameStack", nameStack);
    
    return parseContext;
}

/*____________________________________________________________________________*/

stock ObjLib_DeleteParseContext(Object:parseContext)
{
    // Get and delete stack arrays.
    new Handle:objectStack = ObjLib_GetHandle(parseContext, "objectStack");
    new Handle:typeStack = ObjLib_GetHandle(parseContext, "typeStack");
    new Handle:nameStack = ObjLib_GetHandle(parseContext, "nameStack");
    
    CloseHandle(objectStack);
    CloseHandle(typeStack);
    CloseHandle(nameStack);
}

/*____________________________________________________________________________*/

/**
 * Parse a KeyValue file in object mode. The entire root section is parsed into
 * an object.
 *
 * @param typeDescriptor    (Optional) Type of object. If not specified it will
 *                          create keys in mutable objects.
 * @param sectionKeyName    Name of key where section name is stored. Must exist
 *                          in type descriptor if a type is used. Otherwise it's
 *                          created automatically.
 *
 * @return                  Object with data from the root section.
 */
stock Object:ObjLib_ParseInObjectMode(ObjectType:typeDescriptor = INVALID_OBJECT_TYPE, const String:sectionKeyName[])
{
    return INVALID_OBJECT;
}

/*____________________________________________________________________________*/

/**
 * Parse a KeyValue tree in list mode. This parser assumes that every key in the
 * root is a sub section. Each section is parsed into an object and stored in a
 * list.
 *
 * @param kv                KeyValue tree to parse.
 * @param parseContext      Parse context object. Use ObjLib_GetParseParams to
 *                          create one.
 *
 * @return                  Handle to list of objects. May be empty if tree is
 *                          empty.
 */
stock Handle:ObjLib_ParseInListMode(Handle:kv, Object:parseContext)
{
    decl String:keyName[OBJLIB_KV_MAX_KEY_LEN];
    keyName[0] = 0;
    
    // Create list to store section objects.
    new Handle:sections = CreateArray();
    
    // Go to first sub section in section.
    if (!KvGotoFirstSubKey(kv))
    {
        // No sections. Return list as it is (empty).
        return sections;
    }
    
    // Loop through sections.
    PrintToServer("===== List mode: start =====");
    do
    {
        PrintToServer("----- List mode: iteration start -----");
        
        // Get section name before entering section.
        KvGetSectionName(kv, keyName, sizeof(keyName));
        
        // Enter section.
        if (KvGotoFirstSubKey(kv, false))
        {
            PrintToServer("Parsing section \"%s\"", keyName);
            
            new Object:subObject = ObjLib_ParseSection(kv, parseContext);
            
            // Add object to result list.
            PushArrayCell(sections, subObject);
            
            KvGoBack(kv);
        }
        else
        {
            PrintToServer("List mode: Found empty section.");
            
            // Section is empty.
            new Object:subObject = ObjLib_OnEmptySection(kv, keyName, parseContext);
            
            // Add object to result list, if empty sections are included.
            if (subObject != INVALID_OBJECT)
            {
                PushArrayCell(sections, subObject);
            }
        }
    } while (KvGotoNextKey(kv));    // Go to next section.
    PrintToServer("===== List mode end =====");
    
    return sections;
}

/*____________________________________________________________________________*/

/**
 * Parse the current section into an object. It will do full recursive traversal
 * from the current section.
 *
 * Note: This function expects the KeyValue cursor to be positioned on the first
 *       sub key in this section. Only call this function after a successive
 *       call to KvGotoFirstSubKey.
 *
 * @param kv                Handle to keyvalue tree with the cursor on a
 *                          section.
 * @param parseContext      Parse context object. Use ObjLib_GetParseParams to
 *                          create one.
 *
 * @return                  Object with data from the section, or INVALID_OBJECT
 *                          if the section object was stored in the parent
 *                          object on the object stack.
 */
stock Object:ObjLib_ParseSection(Handle:kv, Object:parseContext)
{
    // Note: This function is recursive.
    
    decl String:keyName[OBJLIB_KV_MAX_KEY_LEN];
    decl String:sectionName[OBJLIB_KV_MAX_KEY_LEN];
    keyName[0] = 0;
    sectionName[0] = 0;
    
    // Get current section name. At this point the cursor is pointing at the
    // first sub key, not the section itself. Save the position (keyId) of this
    // key and go back to get the section name.
    new keyId;
    KvGetSectionSymbol(kv, keyId);
    KvGoBack(kv);
    KvGetSectionName(kv, sectionName, sizeof(sectionName));
    KvJumpToKeySymbol(kv, keyId);   // Jump back to the first sub key again.
    
    // Section names are case insensitive. Convert to lower case.
    Array_ToLower(sectionName, sectionName, sizeof(sectionName));
    
    PrintToServer("-- Parse Section Start: %s --", sectionName);
    
    // Prepare new section object.
    ObjLib_OnSectionStart(kv, sectionName, parseContext);
    
    // Get current section object from the object stack (keep it on the stack).
    // It may be invalid if the current section is being skipped due to errors.
    new Object:object = INVALID_OBJECT;
    Array_PeekCell(ObjLib_GetHandle(parseContext, "objectStack"), object);
    
    // Loop through keys and sections in the current kv section.
    do
    {
        PrintToServer("Iteration start.");
        
        // Read parser state to see if the parser should stop.
        new ObjLibParseState:parseState = ObjLibParseState:ObjLib_GetCell(parseContext, "parseState");
        if (parseState != ObjLibState_Running)
        {
            // Parser was interrupted or an error occoured. Exit loop and end
            // this section. The parent section may continue parsing, depending
            // on state.
            break;
        }
        
        // Get current key or section name. Convert to lower case, it's case
        // insensitive.
        KvGetSectionName(kv, keyName, sizeof(keyName));
        Array_ToLower(keyName, keyName, sizeof(keyName));
        PrintToServer("Current key: %s", keyName);
        
        // Check if key is a sub section.
        if (KvGotoFirstSubKey(kv, false))
        {
            // Key is a sub section.
            PrintToServer("Found sub section. Recursive entry.");
            
            // Parse sub section recurively. In case there were errors or the
            // parsing is aborted, this will be caught in the next iteration of
            // this do-loop (where it's reading parseState).
            ObjLib_ParseSection(kv, parseContext);
            
            // Done with sub section. Go out.
            KvGoBack(kv);
            PrintToServer("Going back.");
        }
        else
        {
            // Key is a value, or section is empty. Figure out which one.
            
            // Sections doesn't have a data type. If there is a data type, it's
            // a key.
            new KvDataTypes:dataType = KvGetDataType(kv, NULL_STRING);
            if (dataType != KvData_None)
            {
                // Get key name. Convert to lower case, it's case insensitive.
                KvGetSectionName(kv, keyName, sizeof(keyName));
                Array_ToLower(keyName, keyName, sizeof(keyName));
                PrintToServer("Found key: %s", keyName);
                
                // Execute new key handler.
                ObjLib_OnKeyValue(kv, keyName, dataType, parseContext);
            }
            else
            {
                // Found an empty section.
                PrintToServer("Found empty section.");
                ObjLib_OnEmptySection(kv, keyName, parseContext);
            }
        }
    } while (KvGotoNextKey(kv, false));     // Go to next key or section.
    
    PrintToServer("-- Parse section End: %s --", sectionName);
    
    // End the section object. If an object is returned there is no parent to
    // store it in, or an error occoured. The parseState parameter in
    // parseContext will tell which one.
    return ObjLib_OnSectionEnd(kv, sectionName, parseContext);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Handles a new section. Creates a new object to store section sub keys and
 * does validation if a type descriptor is available.
 *
 * @param kv                Handle to keyvalue tree with the cursor on a
 *                          section.
 * @param sectionName       Name of the current section.
 * @param parseContext      Parse context object. (Type: ObjLib_KvContextType)
 */
static stock ObjLib_OnSectionStart(Handle:kv, const String:sectionName[], Object:parseContext)
{
    PrintToServer("OnSectionStart -- sectionName: %s", sectionName);
    PrintToServer("Global template type: %x", ObjLib_KvObjectType);
    
    // Get stacks.
    new Handle:typeStack = ObjLib_GetHandle(parseContext, "typeStack");
    new Handle:objectStack = ObjLib_GetHandle(parseContext, "objectStack");
    new Handle:nameStack = ObjLib_GetHandle(parseContext, "nameStack");
    
    // Get type and other meta data for for this section.
    new ObjectType:parentType = INVALID_OBJECT_TYPE;
    new Object:constraints = INVALID_OBJECT;
    new ObjectType:typeDescriptor = ObjLib_KvGetSubTypeOrFail(sectionName, parseContext, parentType, constraints);
    if (typeDescriptor == INVALID_OBJECT_TYPE)
    {
        // Failed to get type. An error is already triggered and the parser
        // state is updated.
        return;
    }
    
    // Delegate work to appropriate handlers according to object type.
    new Object:object = INVALID_OBJECT;
    if (ObjLib_IsCollectionType(typeDescriptor))
    {
        // Object is a collection.
        PrintToServer("Creating collection object.");
        object = ObjLib_OnCollectionStart(constraints, parentType, sectionName, parseContext);
    }
    else
    {
        // Object is a general object (typed or untyped).
        object = ObjLib_OnObjectStart(parentType, typeDescriptor, sectionName, parseContext);
    }
    
    // Push to cursor stacks so they become the current type and object.
    PushArrayCell(typeStack, typeDescriptor);
    PushArrayCell(objectStack, object);
    PushArrayString(nameStack, sectionName);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Cleanup handler when the parser reached the end of a section.
 *
 * Current section object is on top of the object stack. This handler will
 * store it in the parent object, or return it if there's no parent.
 *
 * @param kv                Handle to keyvalue tree.
 * @param sectionName       Name of the ending section.
 * @param parseContext      Parse context object. (Type: ObjLib_KvParamsType)
 *
 * @return                  Parsed section object if stack is empty (there is
 *                          no parent that can store it).
 *                          Otheriwse INVALID_OBJECT (the object was stored
 *                          in the parent object).
 */
static stock Object:ObjLib_OnSectionEnd(Handle:kv, const String:sectionName[], Object:parseContext)
{
    // Check if this section was skipped.
    new ObjLibParseState:parseState = ObjLibParseState:ObjLib_GetCell(parseContext, "parseState");
    if (parseState == ObjLibState_SkipSection)
    {
        // The current section was skipped due to an error. An object was not
        // created or pushed on the stack. We just need to restoring the parser
        // state.
        
        // Restore to running state so the parser can continue parsing remaining
        // keys in the parent section.
        ObjLib_SetCell(parseContext, "parseState", ObjLibState_Running);
        
        return INVALID_OBJECT;
    }
    
    // Get stacks.
    new Handle:typeStack = ObjLib_GetHandle(parseContext, "typeStack");
    new Handle:objectStack = ObjLib_GetHandle(parseContext, "objectStack");
    new Handle:nameStack = ObjLib_GetHandle(parseContext, "nameStack");
    
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_KvGetErrorHandler(parseContext);
    
    // Pop current section from name stack.
    Array_Pop(nameStack);
    
    // Get and pop section object from the object stack.
    new Object:object = INVALID_OBJECT;
    Array_PopCell(objectStack, object);
    
    // Validate object.
    if (object == INVALID_OBJECT)
    {
        ThrowError("[BUG] No object on the stack. This is a bug in objectlib.");
    }
    
    // Get and pop object type.
    new ObjectType:typeDescriptor = INVALID_OBJECT_TYPE;
    Array_PopCell(typeStack, typeDescriptor);
    
    // Validate type.
    if (typeDescriptor == INVALID_OBJECT_TYPE)
    {
        ThrowError("[BUG] No type descriptor on the stack. This is a bug in objectlib.");
    }
    
    // Get parent object (don't remove from stack).
    new Object:parent = INVALID_OBJECT;
    Array_PeekCell(objectStack, parent);
    
    // Validate parent.
    if (parent == INVALID_OBJECT)
    {
        // There is no parent object, the object stack is empty. Return the
        // object as it is.
        return object;
    }
    
    // Get parent type.
    new ObjectType:parentType = ObjLib_GetTypeDescriptor(parent);
    
    // Add section to parent object.
    if (ObjLib_IsCollectionType(parentType))
    {
        // Parent is a collection.
        ObjLib_OnCollectionObjectElement(Collection:parent, object, errorHandler);
    }
    else
    {
        // Parent is a general object.
        ObjLib_OnObjectEnd(object, parent, sectionName, errorHandler);
    }
    
    // Object was stored in parent object, nothing to return.
    return INVALID_OBJECT;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Handles an empty section. Reads the parser context to see if it should be
 * included.
 *
 * @param kv                Handle to keyvalue tree.
 * @param sectionName       Name of the empty section.
 * @param parseContext      Parse context object. (Type: ObjLib_KvParamsType)
 *
 * @return                  Parsed section object if stack is empty (there is
 *                          no parent that can store it) - or on errors.
 *                          Otheriwse INVALID_OBJECT (the object was stored
 *                          in the parent object).
 */
static stock Object:ObjLib_OnEmptySection(Handle:kv, const String:sectionName[], Object:parseContext)
{
    // Check if empty sections are included.
    if (!ObjLib_GetBool(parseContext, "ignoreEmptySections"))
    {
        // Add empty section object. The empty object will use a type if
        // available in constraints.
        ObjLib_OnSectionStart(kv, sectionName, parseContext);
        return ObjLib_OnSectionEnd(kv, sectionName, parseContext);
    }
    
    return INVALID_OBJECT;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Handles a new object section. Creates a new object to store values and does
 * validation if a type descriptor is available.
 
 * @param parentType        Type descriptor for parent object, if available.
 * @param sectionType       Type of this section.
 * @param sectionName       Name of the current section.
 * @param parseContext      Parse context object. (Type: ObjLib_KvContextType)
 *
 * @return                  New object representing this section.
 */
static stock Object:ObjLib_OnObjectStart(ObjectType:parentType, ObjectType:sectionType, const String:sectionName[], Object:parseContext)
{
    new bool:mutableObject = false;
    if (sectionType == ObjLib_KvObjectType)
    {
        // Type template used. Make a mutable object so the parser will be
        // able to create keys.
        mutableObject = true;
        PrintToServer("Object is mutable.");
    }
    
    PrintToServer("Creating general object. Type: %x | Mutable: %d", sectionType, mutableObject);
    return ObjLib_CreateObject(sectionType, mutableObject);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Cleanup handler when the parser reached the end of a object section.
 *
 * Current object is on top of the object stack. This handler will store it in
 * the parent object, or return it if there's no parent.
 *
 * @param object            This object.
 * @param parent            Parent object.
 * @param sectionName       Name of the ending section.
 * @param parseContext      Parse context object. (Type: ObjLib_KvParamsType)
 * @param errorHandler      Error handler to use on errors.
 */
static stock ObjLib_OnObjectEnd(Object:object, Object:parent, const String:sectionName[], ObjLib_ErrorHandler:errorHandler)
{
    // Get index of sub section key in parent object. This key may not exist if
    // the parent object type is not predefined in rootType.
    new keyIndex = ObjLib_GetKeyIndex(parent, sectionName);
    
    // Get key constraints, if any.
    new Object:constraints = INVALID_OBJECT;
    if (keyIndex >= 0)
    {
        // Get the constraints from the parent type, if available.
        new ObjectType:parentType = ObjLib_GetTypeDescriptor(parent);
        constraints = ObjLib_GetTypeConstraintsAt(parentType, keyIndex);
    }
    
    // Create key in parent object if there are no constraints and the key
    // doesn't exist.
    if (constraints == INVALID_OBJECT && keyIndex < 0)
    {
        // Verify that this is a mutable object.
        if (!ObjLib_IsMutable(parent))
        {
            ThrowError("[BUG] Parser attempted to add key to immutable object. This is a bug in objectlib.");
        }
        
        // Create object key for storing this section.
        ObjLib_AddObjectKey(parent, sectionName, ObjDataType_Object);
    }
    
    // Add object to parent (implies further validation, if any).
    ObjLib_SetObject(parent, sectionName, object, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Handles a new collection section. Creates a new collection object to store
 * elements.
 *
 * @param constraints       Constraints for this collection object (not its
 *                          elements).
 * @param parentType        Type descriptor for parent object, if available.
 * @param sectionName       Name of the current section.
 * @param parseContext      Parse context object. (Type: ObjLib_KvContextType)
 *
 * @return                  New collection objects representing this section.
 */
static stock Object:ObjLib_OnCollectionStart(Object:constraints, ObjectType:parentType, const String:sectionName[], Object:parseContext)
{
    // Set default type and size.
    new ObjectDataType:dataType = ObjDataType_String;
    new blockSize = ByteCountToCells(OBJLIB_MAX_STRING_LEN);
    new Object:elementConstraints = INVALID_OBJECT;
    
    // Get collection data type from constraints, if any.
    if (constraints != INVALID_OBJECT && ObjLib_GetTypeDescriptor(constraints) == ObjLib_CollectionConstraints)
    {
        // Override defaults with settings in constraint.
        dataType = ObjectDataType:ObjLib_GetCell(constraints, "dataType");
        blockSize = ObjLib_GetCell(constraints, "minBlockSize");
        elementConstraints = ObjLib_GetObject(constraints, "elementConstraints");
    }
    
    PrintToServer("ObjLib_OnCollectionStart -- constraints: %x, dataType: %d, elementConstraints: %x", constraints, dataType, elementConstraints);
    
    // Create collection.
    return Object:ObjLib_CreateCollection(dataType, blockSize, elementConstraints);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Stores an object element in the collection.
 *
 * @param collection        Collection to store object element in.
 * @param element           Object element to store.
 * @param errorHandler      Error handler to use on errors.
 */
static stock ObjLib_OnCollectionObjectElement(Collection:collection, Object:element, ObjLib_ErrorHandler:errorHandler)
{
    // Add the element to the collection (implies validation, if any).
    ObjLib_AddObject(collection, element, errorHandler);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Reads a keyvalue pair.
 *
 * @param kv                Handle to keyvalue tree.
 * @param keyName           Name of the key.
 * @param dataType          Data type in KeyValue tree.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 */
static stock ObjLib_OnKeyValue(Handle:kv, const String:keyName[], KvDataTypes:kvDataType, Object:parseContext)
{
    PrintToServer("OnKeyValue -- keyName: %s", keyName);
    
    // Get stacks.
    new Handle:objectStack = ObjLib_GetHandle(parseContext, "objectStack");
    new Handle:typeStack = ObjLib_GetHandle(parseContext, "typeStack");
    
    // Get current object.
    new Object:object = INVALID_OBJECT;
    Array_PeekCell(objectStack, object);
    if (object == INVALID_OBJECT)
    {
        // Bug. An object should already be prepared and pushed onto the stack
        // when the section started.
        ThrowError("[BUG] No object. This is a bug in objectlib.");
    }
    
    // Get object type.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    PrintToServer("typeDescriptor: %x", typeDescriptor);
    
    // Get and store value. Objectlib will handle type mismatch or constraint
    // violations.
    if (ObjLib_IsCollectionType(typeDescriptor))
    {
        // Object is a collection. Get data type of collection.
        new ObjectDataType:dataType = ObjLib_CollectionGetDataType(Collection:object);
        
        // Add value to collection.
        ObjLib_StoreCollectionValue(kv, Collection:object, dataType, parseContext);
    }
    else
    {
        new ObjectDataType:dataType = ObjDataType_Invalid;
        
        // Get original type (type descriptor object was created with).
        new ObjectType:typeOnStack = INVALID_OBJECT_TYPE;
        Array_PeekCell(typeStack, typeOnStack);
        if (typeOnStack == INVALID_OBJECT_TYPE)
        {
            ThrowError("[BUG] No type on stack. This is a bug in objectlib.");
        }
        PrintToServer("typeOnStack: %x", typeOnStack);
        
        // Check if object is using the type template. Do this by checking the
        // original type.
        if (typeOnStack == ObjLib_KvObjectType)
        {
            PrintToServer("Object is untyped, adding key.");
            
            // Object has no predefined type descriptor. Add string key with no
            // constraints.
            ObjLib_AddObjectKey(object, keyName, ObjDataType_String);
            dataType = ObjDataType_String;
        }
        else
        {
            PrintToServer("Object is typed.");
            
            // Get data type of key. It will be string if key is using lookup
            // constraints.
            dataType = ObjLib_GetKeyTypeOrFail(object, typeDescriptor, keyName, parseContext);
            
            // Check if failed.
            if (dataType == ObjDataType_Invalid)
            {
                // Failed to get key type. Parser state is already updated.
                return;
            }
        }
        
        // Store value in object.
        ObjLib_StoreObjectValue(kv, object, keyName, dataType, parseContext);
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Reads a value from a key in the keyvalue tree and stores it in the object.
 *
 * @param kv            Handle to keyvalue tree.
 * @param object        Object to store value in.
 * @param keyName       Key to read.
 * @param dataType      Key type.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 * @param parseContect  Object with parser settings.
 *                      (Type: ObjLib_KvParamsType)
 */
static stock ObjLib_StoreObjectValue(Handle:kv, Object:object, const String:keyName[], ObjectDataType:dataType, Object:parseContext)
{
    // Note: Using NULL_STRING as key name in KeyValue tree so that it will read
    //       the value at the current cursor position (which is prepared by the
    //       parser).
    
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_KvGetErrorHandler(parseContext);
    
    // Read value from KeyValue tree according to the expected data type.
    // Objectlib will validate type and constraints.
    switch (dataType)
    {
        case ObjDataType_Any:
        {
            new value = KvGetNum(kv, NULL_STRING);
            ObjLib_SetAny(object, keyName, value, errorHandler);
        }
        case ObjDataType_Cell:
        {
            new value = KvGetNum(kv, NULL_STRING);
            ObjLib_SetCell(object, keyName, value, errorHandler);
        }
        case ObjDataType_Bool:
        {
            // Note: This does direct conversion, from string to integer. The
            //       user may use a boolean lookup constraint for converting
            //       boolean words and call SetString instead.
            new bool:value = bool:KvGetNum(kv, NULL_STRING);
            ObjLib_SetBool(object, keyName, value, errorHandler);
        }
        case ObjDataType_Float:
        {
            new Float:value = KvGetFloat(kv, NULL_STRING);
            ObjLib_SetFloat(object, keyName, value, errorHandler);
        }
        case ObjDataType_Handle:
        {
            // Not supported.
            ThrowError("Handle type is not supported by the objectlib KeyValue parser.");
        }
        case ObjDataType_Function:
        {
            // Not supported. It's still possible to look up functions by name,
            // using custom lookup constraints. It might be a bad idea to store
            // function names in config files, though there may be some valid
            // cases.
            ThrowError("Function type is not supported by the objectlib KeyValue parser. Use custom lookup constraints if you really need this.");
        }
        case ObjDataType_Array:
        {
            // Not supported.
            ThrowError("Array type is not supported by the objectlib KeyValue parser. Consider using collection objects.");
        }
        case ObjDataType_String:
        {
            decl String:value[OBJLIB_KV_MAX_STRING_LEN];
            value[0] = 0;
            KvGetString(kv, NULL_STRING, value, sizeof(value));
            ObjLib_SetString(object, keyName, value, errorHandler);
            PrintToServer("Reading value of %s: \"%s\"", keyName, value);
        }
        case ObjDataType_Object:
        {
            // This shouldn't happen. Objects should be read as kv sections or
            // handled as named lookups when available.
            ThrowError("[BUG] Can't read key as an object. The parser should check if this key is a sub section. This is a bug in objectlib.");
        }
        case ObjDataType_ObjectType:
        {
            // Not supported.
            ThrowError("ObjectType type is not supported by the objectlib KeyValue parser.");
        }
        default:
        {
            ThrowError("[BUG] Unexpected data type. This is a bug in objectlib.");
        }
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Reads a value from a key in the keyvalue tree and stores it in the
 * collection.
 *
 * @param kv            Handle to keyvalue tree.
 * @param collection    Collection to store value in.
 * @param dataType      Key type.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 * @param parseContect  Object with parser settings.
 *                      (Type: ObjLib_KvParamsType)
 */
static stock ObjLib_StoreCollectionValue(Handle:kv, Collection:collection, ObjectDataType:dataType, Object:parseContext)
{
    // Note: Using NULL_STRING as key name in KeyValue tree so that it will read
    //       the value at the current cursor position (which is prepared by the
    //       parser).
    
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_KvGetErrorHandler(parseContext);
    
    // Read value from KeyValue tree according to the expected data type.
    // Objectlib will validate type and constraints.
    switch (dataType)
    {
        case ObjDataType_Any:
        {
            //new value = KvGetNum(kv, NULL_STRING);
            //ObjLib_AddAny(collection, value, errorHandler);
            ThrowError("TODO. Not implemented!");
        }
        case ObjDataType_Cell:
        {
            new value = KvGetNum(kv, NULL_STRING);
            ObjLib_AddCell(collection, value, errorHandler);
        }
        case ObjDataType_Bool:
        {
            // Note: This does direct conversion, from string to integer. The
            //       user may use a boolean lookup constraint for converting
            //       boolean words and call SetString instead.
            new bool:value = bool:KvGetNum(kv, NULL_STRING);
            ObjLib_AddBool(collection, value, errorHandler);
        }
        case ObjDataType_Float:
        {
            new Float:value = KvGetFloat(kv, NULL_STRING);
            ObjLib_AddFloat(collection, value, errorHandler);
        }
        case ObjDataType_Handle:
        {
            // Not supported.
            ThrowError("Handle type is not supported by the objectlib KeyValue parser.");
        }
        case ObjDataType_Function:
        {
            // Not supported. It's still possible to look up functions by name,
            // using custom lookup constraints. It might be a bad idea to store
            // function names in config files, though there may be some valid
            // cases.
            ThrowError("Function type is not supported by the objectlib KeyValue parser. Use custom lookup constraints if you really need this.");
        }
        case ObjDataType_Array:
        {
            // Not supported.
            ThrowError("Array type is not supported by the objectlib KeyValue parser. Consider using collection objects.");
        }
        case ObjDataType_String:
        {
            decl String:value[OBJLIB_KV_MAX_STRING_LEN];
            value[0] = 0;
            KvGetString(kv, NULL_STRING, value, sizeof(value));
            ObjLib_AddString(collection, value, errorHandler);
        }
        case ObjDataType_Object:
        {
            // This shouldn't happen. Objects should be read as kv sections or
            // handled as named lookups when available.
            ThrowError("[BUG] Can't read key as an object. The parser should check if this key is a sub section. This is a bug in objectlib.");
        }
        case ObjDataType_ObjectType:
        {
            // Not supported.
            ThrowError("ObjectType type is not supported by the objectlib KeyValue parser.");
        }
        default:
        {
            ThrowError("[BUG] Unexpected data type. This is a bug in objectlib.");
        }
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Builds the default type template used on untyped objects.
 */
static stock ObjLib_BuildKvObjectType()
{
    if (!ObjLib_KvObjectTypeBuilt)
    {
        ObjLib_KvObjectType = ObjLib_CreateType(ByteCountToCells(OBJLIB_KV_MAX_STRING_LEN));
        ObjLib_KvObjectTypeBuilt = true;
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Builds the global context object type used to store parser settings.
 */
static stock ObjLib_BuildKvContextType()
{
    if (!ObjLib_KvContextTypeBuilt)
    {
        // Create parse context type. Reserve some space for strings.
        ObjLib_KvContextType = ObjLib_CreateType(ByteCountToCells(OBJLIB_KV_MAX_KEY_LEN));
        
        // Key for storing a reference to the root type descriptor (the root in
        // this case is the section where the parser started from).
        ObjLib_AddKey(ObjLib_KvContextType, "rootType", ObjDataType_ObjectType, INVALID_OBJECT);
        
        // Flag for specifying whether to ignore empty sections.
        ObjLib_AddKey(ObjLib_KvContextType, "ignoreEmptySections", ObjDataType_Bool, INVALID_OBJECT);
        
        // Continue parsing remaining keys if there's an error parsing a key.
        ObjLib_AddKey(ObjLib_KvContextType, "continueOnError", ObjDataType_Bool, INVALID_OBJECT);
        
        // Optional user error handler callback to use on parser errors
        // (validation, unexpected keys, etc). This must be a
        // ObjLib_ErrorHandler function.
        ObjLib_AddKey(ObjLib_KvContextType, "errorHandler", ObjDataType_Function, INVALID_OBJECT);
        
        // Parser name (optional). Useful to see which parser that failed in
        // error logs.
        ObjLib_AddKey(ObjLib_KvContextType, "name", ObjDataType_String, INVALID_OBJECT);
        
        // State value read by parser on every iteration. Used to stop parser
        // and handle errors.
        ObjLib_AddKey(ObjLib_KvContextType, "parseState", ObjDataType_Cell, INVALID_OBJECT);
        
        // Object cursor stack. The top object is the object representing the
        // current section. ADT array used as a stack.
        ObjLib_AddKey(ObjLib_KvContextType, "objectStack", ObjDataType_Handle, INVALID_OBJECT);
        
        // Object type cursor stack. Stores references to type descriptors used
        // to build the objects in the object stack. ADT array used as a stack.
        //
        // Note: Get type descriptors directly from the object to get the
        //       objects' actual type descriptors. In most cases this stack will
        //       store the same references, but mutable objects will have their
        //       own clones. It's not guaranteed to be the same type descriptor.
        ObjLib_AddKey(ObjLib_KvContextType, "typeStack", ObjDataType_Handle, INVALID_OBJECT);
        
        // Section name stack used to build path to current location. ADT array
        // used as a stack.
        ObjLib_AddKey(ObjLib_KvContextType, "nameStack", ObjDataType_Handle, INVALID_OBJECT);
        
        ObjLib_KvContextTypeBuilt = true;
    }
}

/*____________________________________________________________________________*/

/**
 * Gets either the root type or the type template.
 *
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 *
 * @return                  Type descriptor defined in rootType in the parse
 *                          context, or the type template if not available.
 */
stock ObjectType:ObjLib_KvGetRootOrTypeTemplate(Object:parseContext)
{
    // Make sure object type template is built.
    ObjLib_BuildKvObjectType();
    
    new ObjectType:rootType = ObjLib_GetObjectType(parseContext, "rootType");
    
    // Return either root type or the type template.
    return rootType != INVALID_OBJECT_TYPE ? rootType : ObjLib_KvObjectType;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the specified type descriptor is the type template.
 *
 * @param typeDescriptor    Type descriptor to compare.
 *
 * @return                  Type descriptor is referencing the object type
 *                          template, false otherwise.
 */
stock bool:ObjLib_KvIsTypeTemplate(ObjectType:typeDescriptor)
{
    // Make sure object type template is built.
    ObjLib_BuildKvObjectType();
    
    return typeDescriptor == ObjLib_KvObjectType;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Gets the type of collection element objects of the specified collection. It
 * will trigger errors, if any.
 *
 * This is a helper function to get object the object type of collections that
 * stores object elements.
 *
 * @param collection            Collection to get element type of.
 * @param parseContext          Object with parser settings.
 *                              (Type: ObjLib_KvParamsType)
 * @param elementName           Name of the element in the collection (key or
 *                              section name).
 * @param elementConstraints    (Output) Constraint object for collection
 *                              elements.
 *
 * @return                  Type descriptor of element objects, or type template
 *                          if not specified. On handled errors it returns
 *                          INVALID_OBJECT_TYPE.
 *
 * @error                   Collection is not storing objects, or collection is
 *                          using another constraint type than object
 *                          constraints.
 */
static stock ObjectType:ObjLib_KvGetColElementTypeOrFail(Collection:collection, Object:parseContext, const String:elementName[], &Object:elementConstraints = INVALID_OBJECT)
{
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_KvGetErrorHandler(parseContext);
    
    // Verify that the collection is storing objects.
    new ObjectDataType:dataType = ObjLib_CollectionGetDataType(collection);
    if (dataType != ObjDataType_Object)
    {
        // Something is trying to get an object type of a collection that
        // doesn't store objects. This is an error.
        ObjLib_KvCollectionTypeMismatchError(collection, ObjLib_GetTypeDescriptor(Object:collection), errorHandler, parseContext, elementName, dataType, ObjDataType_Object);
        return INVALID_OBJECT_TYPE;
    }
    
    // Get collection element constraints, if any.
    elementConstraints = ObjLib_CollectionGetConstraints(collection);
    if (elementConstraints == INVALID_OBJECT)
    {
        // No element constraints, use type template.
        return ObjLib_KvObjectType;
    }
    
    // Get constraint type.
    new ObjectType:constraintType = ObjLib_GetTypeDescriptor(elementConstraints);
    if (constraintType == ObjLib_LookupConstraints)
    {
        // Collection is using lookup constraints. Get and validate sub
        // constraints.
        new Object:subConstraints = ObjLib_GetObject(elementConstraints, "subConstraints");
        if (subConstraints != INVALID_OBJECT)
        {
            // Replace constraint type so it will be validated below.
            constraintType = ObjLib_GetTypeDescriptor(subConstraints);
        }
        else
        {
            // No sub constraints, use type template.
            return ObjLib_KvObjectType;
        }
    }
    
    // Validate constraint type.
    if (constraintType != ObjLib_ObjectConstraints)
    {
        // Not using object constraints. This is an error.
        ObjLib_KvConstraintTypeMismatch(Object:collection, ObjLib_GetTypeDescriptor(Object:collection), errorHandler, parseContext, elementName, constraintType);
        return INVALID_OBJECT_TYPE;
    }
    
    // Get object type descriptor constraints.
    new ObjectType:elementType = ObjLib_GetObjectType(elementConstraints, "type");
    
    // Use either element type if available, otherwise use the type
    // template.
    return elementType != INVALID_OBJECT_TYPE ? elementType : ObjLib_KvObjectType;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Gets the type of the specified sub section (relative to the current section,
 * on the top of the object stack). It will trigger errors, if any.
 *
 * If a key with object data type has no constraints, the type template will be
 * used.
 *
 * Note: If a type template is returned, only use it to create mutable objects
 *       from it. Otherwise it will be locked and further parsing may stop
 *       working.
 *
 * @param subSection        Name of the sub section to get type of.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 * @param parentType        (Output) Parent type template for sub section.
 * @param constraints       (Output) Constraint object for sub section, if
 *                          available.
 *
 * @return                  Type of the specified sub section. If no type is
 *                          specified it will return the type template
 *                          (ObjLib_KvObjectType). On errors it will return
 *                          INVALID_OBJECT_TYPE and update the parser state.
 */
static stock ObjectType:ObjLib_KvGetSubTypeOrFail(const String:subSection[], Object:parseContext, &ObjectType:parentType = INVALID_OBJECT_TYPE, &Object:constraints = INVALID_OBJECT)
{
    PrintToServer("ObjLib_KvGetSubTypeOrFail -- subSection: %s", subSection);
    
    // Goal: Figure out whether the sub section is one of the following types:
    // - Untyped object (using type template)
    // - Typed object (defined by rootType tree, or parent collection)
    // - Collection object (defined by parent, or parent constraints)
    
    // Make sure type template is ready.
    ObjLib_BuildKvObjectType();
    
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_KvGetErrorHandler(parseContext);
    
    // Get stacks.
    new Handle:objectStack = ObjLib_GetHandle(parseContext, "objectStack");
    new Handle:typeStack = ObjLib_GetHandle(parseContext, "typeStack");
    
    // Get parent type template.
    new ObjectType:typeTemplate = INVALID_OBJECT_TYPE;
    Array_PeekCell(typeStack, typeTemplate);
    
    // Check if parent is untyped.
    if (ObjLib_KvIsTypeTemplate(typeTemplate))
    {
        // Nothing more to do, all sub sections will also be untyped. This also
        // implies that collections in untyped parents are not supported.
        return ObjLib_KvObjectType;
    }
    
    // Get parent object, which is currently on the top of the object stack
    // (not removing it from the stack).
    new Object:parent = INVALID_OBJECT;
    Array_PeekCell(objectStack, parent);
    
    // Get type descriptor of parent object. This will be either the actual
    // type descriptor, or the type descriptor used to create it.
    if (parent == INVALID_OBJECT)
    {
        // Parent object is not available, the sub section is the root section.
        // Use root type or type template.
        parentType = ObjLib_KvGetRootOrTypeTemplate(parseContext);
    }
    else
    {
        // Get parent type.
        parentType = ObjLib_GetTypeDescriptor(parent);
    }
    
    // At this point the parent has a predefined type. Figure out whether it's
    // a collection or a regular object.
    
    // Check if parent is a collection (handles nested collections). This is the
    // place to check for other object types if more types are added in the
    // future.
    if (ObjLib_IsCollectionType(parentType))
    {
        // Parent is a collection collection object. Get type of collection 
        // elements.
        return ObjLib_KvGetColElementTypeOrFail(Collection:parent, parseContext, subSection, constraints);
    }
    
    // Parent is a general object. Get object constraints for this sub section
    // from the parent.
    new keyIndex = ObjLib_GetKeyIndexFromType(parentType, subSection);
    if (keyIndex < 0)
    {
        // Error. The specified sub section doesn't exist in parent object.
        ObjLib_KvUnexpectedSectionError(parent, parentType, errorHandler, parseContext, subSection);
        return INVALID_OBJECT_TYPE;
    }
    
    // Validate that this key is storing an object.
    new ObjectDataType:keyType = ObjLib_GetKeyDataType(parentType, keyIndex);
    if (keyType != ObjDataType_Object)
    {
        // Invalid sub section. It's not supposed to be an object.
        ObjLib_KvInvalidSectionError(parent, parentType, errorHandler, parseContext, subSection);
        return INVALID_OBJECT_TYPE;
    }
    
    // Check sub section constraints.
    constraints = ObjLib_GetTypeConstraintsAt(parentType, keyIndex);
    if (constraints == INVALID_OBJECT)
    {
        // No constraints. Use type template.
        return ObjLib_KvObjectType;
    }
    
    // Get type of sub section according to constraints:
    
    // Object constraints.
    new ObjectType:constraintType = ObjLib_GetTypeDescriptor(constraints);
    if (constraintType == ObjLib_ObjectConstraints)
    {
        // Use type descriptor specified in the constraint object.
        new ObjectType:typeDescriptor = ObjLib_GetObjectType(constraints, "type");
        
        // Validate type descriptor in constraint.
        if (typeDescriptor == INVALID_OBJECT_TYPE)
        {
            // Constraint object has no type constraint, use type template.
            PrintToServer("Object constraint has no type constraint.");
            return ObjLib_KvObjectType;
        }
        
        // Type is valid. Use type specified in constraint object.
        PrintToServer("Using object constraint. Type: %x", typeDescriptor);
        return typeDescriptor;
    }
    
    // Collection constraints.
    if (constraintType == ObjLib_CollectionConstraints)
    {
        // This section is a collection. Use collection type descriptor.
        return ObjLib_GetCollectionType();
    }
    
    // Unexpected constraint, it's not a compatible object constraint. This is
    // a bug. If other object constraints are added in the future, they must be
    // handled above this statement.
    ThrowError("[BUG] Unexpected constraint. This is a bug in objectlib.");
    return INVALID_OBJECT_TYPE;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Gets the data type of an object key, or throws an error if failed.
 *
 * If a key is using lookup constraints, the string data type will be returned.
 *
 * @param object            Object with the specified key.
 * @param typeDescriptor    Type descriptor for object.
 * @param keyName           Name of key to get data type of.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 *
 * @return                  Key data type, or ObjDataType_Invalid on error.
 */
static stock ObjectDataType:ObjLib_GetKeyTypeOrFail(Object:object, ObjectType:typeDescriptor, const String:keyName[], Object:parseContext)
{
    new ObjectDataType:keyDataType = ObjDataType_Invalid;
    // Get key index.
    new keyIndex = ObjLib_GetKeyIndexFromType(typeDescriptor, keyName);
    if (keyIndex < 0)
    {        
        // Get error handler.
        new ObjLib_ErrorHandler:errorHandler = ObjLib_KvGetErrorHandler(parseContext);
        
        // Unexpected key. Throw validation error.
        new bool:errorHandled = ObjLib_KvUnexpectedKeyError(object, typeDescriptor, errorHandler, parseContext, keyName);
        if (errorHandled)
        {
            // Return to parse loop. The parse state will tell if parser
            // should abort or continue.
            return ObjDataType_Invalid;
        }
    }
    
    // Get data type of key.
    new Handle:keyDataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    keyDataType = ObjectDataType:GetArrayCell(keyDataTypes, keyIndex);
    
    // At this point we have the key data type, but it might be bound to lookup
    // constraints. Check if it has lookup constraints.
    
    // Get key constraint type, if any.
    new ObjectType:constraintType = INVALID_OBJECT_TYPE;
    new Object:constraints = ObjLib_GetTypeConstraintsAt(typeDescriptor, keyIndex);
    if (constraints != INVALID_OBJECT)
    {
        constraintType = ObjLib_GetTypeDescriptor(constraints);
    }
    
    // Check if this is a lookup case.
    if (constraintType == ObjLib_LookupConstraints)
    {
        // Key is using lookup constraints. Set key type to string so it will
        // trigger the lookup handler when reading values.
        return ObjDataType_String;
    }
    
    // No lookup constraints, use key type.
    return keyDataType;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the parser has a root type specified.
 *
 * @param       Object with parser settings.
 *              (Type: ObjLib_KvParamsType)
 *
 * @return      True if root type exist, false otherwise.
 */
stock bool:ObjLib_KvHasRootType(Object:parseContext)
{
    return ObjLib_GetObjectType(parseContext, "rootType") != INVALID_OBJECT_TYPE;
}

/*____________________________________________________________________________*/

/**
 * Example of full recursive traversal.
 */
stock BrowseKeyValues(Handle:kv, level = 0)
{
    new String:sectionName[32];
    new String:value[32];
    
    PrintToServer("-- Section start (level %d) --", level);
    do
    {
        KvGetSectionName(kv, sectionName, sizeof(sectionName));
        PrintToServer("Current key: %s", sectionName);
        
        // Check if current key is a section. Assume it has sub keys and attempt
        // to enter the section.
        if (KvGotoFirstSubKey(kv, false))
        {
            // Success. Confirmed that it's a sub key.
            
            PrintToServer("Sub key. Recursive entry.");
            BrowseKeyValues(kv, level + 1);
            
            PrintToServer("Going back.");
            KvGoBack(kv);
        }
        else
        {
            // Failed. It's a regular key, or the section is empty. Find out by
            // getting the data type of the key.
            
            // Get data type of current key.
            new KvDataTypes:dataType = KvGetDataType(kv, NULL_STRING);
            
            // Check if key has a data type. If not, the section is empty.
            if (dataType != KvData_None)
            {
                // Cursor is on a key.
                KvGetString(kv, NULL_STRING, value, sizeof(value));
                PrintToServer("Regular key. Value: \"%s\"", value);
            }
            else
            {
                // Section is empty. Do nothing.
                PrintToServer("Section is empty.");
            }
        }
    } while (KvGotoNextKey(kv, false));
    PrintToServer("-- Section end --");
}
