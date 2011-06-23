#ifndef SI_MACRO_H
#define SI_MACRO_H


#define SI_API extern
// override this macro later in case you need declspec junk. 

/** OS selection macro. */
#ifdef __WIN32__
  #define SI_OS_WIN32
  #define SI_OS "WIN32"
#else  
  #define SI_OS_POSIX
  #define SI_OS "POSIX"
#endif
  
/** Macros for a very simple mixin system. Use it as follows: 
 define the mixin:
 struct Mixin_ { ... } ; typedef struct Mixin_ Mixin;
 
 Then to include the mixin in a struct:
 struct Child_ {
  ...
  SI_MIXIN_TYPEDEF(Mixin); 
  ...
 } ; 
 Then to retrieve the mixin from  a struct Child_ * :
 struct Child_ * child:
  
 SI_MIXIN_OF(child, Mixin);
 
 If you need a pointer to the containing child in functions that use Mixin, 
 use the following macro to retrieve the : 
 SI_MIXIN_CONTAINER(mixin, CHILDTRYPE)
 
 
**/
#define SI_MIXIN_STRUCT(MIXINTYPE)  struct MIXINTYPE MIXINTYPE
#define SI_MIXIN_TYPEDEF(MIXINTYPE) MIXINTYPE MIXINTYPE
#define SI_MIXIN_OF(DATA, MIXINTYPE)        \
        ((MIXTYPE *)&((DATA)->MIXINTYPE))

#define SI_MIXIN_HAS_CONTAINER(CHILDTYPE)     CHILDTYPE * CHILDTYPE
#define SI_MIXIN_CONTAINER_(DATA, MIXINTYPE, CHILDTYPE)  \
        (SI_MIXIN_OF(DATA, MIXINTYPE))->CHILDTYPE) = DATA;

#define SI_CONTAINER_OF(DATA, TYPE, FIELD)    \
        ((TYPE*)(((char *)(DATA)) - offsetof(TYPE, FIELD)))

#define SI_MIXIN_CONTAINER(MIXIN, CHILDTYPE)  \
        SI_CONTAINER_OF(MIXIN, CHILDTYPE, MIXIN);

/** Fake booleans, of course we need them... :p */
#ifndef TRUE 
#define TRUE (!0)
#endif

#ifndef FALSE 
#define FALSE (0)
#endif

/** some syntactic macros that ease very common cases */
#define SI_RETURN_IF(FOO) do { if(FOO) return (FOO); } while (0))



#endif
