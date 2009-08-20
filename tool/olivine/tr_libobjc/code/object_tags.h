#ifndef __OBJC_TAGS_H__INCLUDED_
#define __OBJC_TAGS_H__INCLUDED
/**
 * Adds a tag to every object.  The return value is an offset which should be
 * added to the object to give the tag.
 */
int add_tag(size_t size);
/**
 * Convenience macro for adding a tag of a specified type.
 */
#define OBJ_TAG(x) add_tag(sizeof(x))
/**
 * Get the tag of the specified type from the given object and offset.  For
 * each tag, a macro using this should be created to access the tag.
 */
#define TAG(object, offset, type) (*(type*)(((char*)object) + offset))
/**
 * Size taken by all existing tags.
 */
extern size_t object_tags_size;
#endif //__OBJC_TAGS_H__INCLUDED
