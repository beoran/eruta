/** 
* A walker is used for an alternate interface to walk collections.
* It will be called with the original 
 * the active element, the key (if any), the collection, and some extra data.
 */
#ifndef SI_WALKER_H
#define SI_WALKER_H

typedef void * (SiWalker)(void * now, void * key, void * bunch, void * extra);


#endif
