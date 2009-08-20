#include <string.h>
#include <stdio.h>
#include "selector.h"
#include "lock.h"

/**
 * Structure used to store selectors in the list.
 */
struct objc_selector
{
	char * name;
	char * types;
};

/**
 * Mapping from selector hashes to selector numbers.
 */
static SparseArray * selector_table = NULL;
DECLARE_LOCK(selector_table);
/**
 * Mapping from selector numbers to objc_selectors.
 */
static SparseArray * selector_list = NULL;
DECLARE_LOCK(selector_list);
/**
 * Location to insert next selector in the list.
 */
static uint32_t selector_next = 1;

/**
 * Create data structures to store selectors.
 */
void objc_selector_init()
{
	selector_table = SparseArrayNew();
	INIT_LOCK(selector_table);
	selector_list = SparseArrayNew();
	INIT_LOCK(selector_list);
}

/**
 * Test two selectors for equality.
 */
static inline int sel_equal(struct objc_selector * a, struct objc_selector * b)
{
	return (strcmp(a->name, b->name) == 0) 
		&& (strcmp(a->types, b->types) == 0);
}

/**
 * Hash a selector.
 */
uint32_t hash_selector(char * name, char * types)
{
	uint32_t hash = 5381;
	uint32_t c;
	if(name != NULL)
	{
		while((c = (uint32_t)*name++))
		{
			hash = hash * 33 + c;
		}
	}
	if(types != NULL)
	{
		while((c = (uint32_t)*types++))
		{
			hash = hash * 33 + c;
		}
	}
	return hash;
}

/**
 * Perform selector lookup from selector table.
 * UNSAFE: Caller must lock selector_table.
 */
static uint32_t unsafe_lookup_typed_selector(char * name, char * types)
{
	uint32_t hash = hash_selector(name, types);
	uint32_t index = ((uint32_t) (unsigned long)SparseArrayLookup(selector_table , hash));
	struct objc_selector * sel = SparseArrayLookup(selector_list, index);
	while(sel != NULL)
	{
		if((strcmp(name, sel->name) == 0) 
			&&
			(strcmp(name, sel->name) == 0))
		{
			return index;
		}
		sel = SparseArrayLookup(selector_table, ++index);
	}
	return 0;
}
uint32_t default_lookup_typed_selector(char * name, char * types)
{
	LOCK(selector_table);
	uint32_t sel = unsafe_lookup_typed_selector(name, types);
	UNLOCK(selector_table);
	return sel;
}
lookup_typed_selector_t lookup_typed_selector = default_lookup_typed_selector;

uint32_t default_register_typed_selector(char * name, char * types)
{
	uint32_t hash = hash_selector(name, types);
	LOCK(selector_table);
	uint32_t uid = unsafe_lookup_typed_selector(name, types);
	if(!uid)
	{
		struct objc_selector * sel = calloc(1, sizeof(struct objc_selector));
		sel->name = strdup(name);
		sel->types = strdup(types);
		LOCK(selector_list);
		uid = selector_next;
		SparseArrayInsert(selector_list,
				selector_next++,
				sel);
		UNLOCK(selector_list);
		SparseArrayInsert(selector_table, hash, ((void*) (unsigned long)uid));
	}
	UNLOCK(selector_table);
	return uid;
}
register_typed_selector_t register_typed_selector = default_register_typed_selector;
int register_selector(char * name)
{
	return register_typed_selector(name, NULL);
}

char * name_for_selector(uint32_t uid)
{
	struct objc_selector * sel = SparseArrayLookup(selector_list, uid);
	if(sel == NULL)
	{
		return NULL;
	}
	return sel->name;
}

char * types_for_selector(uint32_t uid)
{
	struct objc_selector * sel = SparseArrayLookup(selector_list, uid);
	if(sel == NULL)
	{
		return NULL;
	}
	return sel->types;
}
