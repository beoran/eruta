#ifndef _SI_NS_H_
#define _SI_NS_H_


#define SI_PP_PASTE_2(A, B) A##B
#define SI_PP_PASTE(A, B) SI_PP_PASE_2(A, B)

#ifndef SI_NS_FUNCTION
#define SI_NS_FUNCTION si
#endif

#ifndef SI_NS_STRUCT
#define SI_NS_STRUCT  Si
#endif

#define SI_NAMESPACE(NAMESPACE, NAME) SI_PP_PASTE(NAMESPACE, NAME)

#define si(FUNCTION) SI_NAMESPACE(SI_NS_FUNCTION, FUNCTION)
#define Si(STRUCT)   SI_NAMESPACE(SI_NS_STRUCT, STRUCT)

#define SI_API extern
/* Can be used for __declspec junk if your platform needs it */



#endif