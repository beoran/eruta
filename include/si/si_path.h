/** File and diractory path name functionality.
*/

#ifndef SI_PATH_H
#define SI_PATH_H

#ifdef SI_OS_WIN32
  #define SI_PATH_SEPSTR "\\"
  #define SI_PATH_SEPCHR '\\'
#else 
  #define SI_PATH_SEPSTR "/"
  #define SI_PATH_SEPCHR '/'
#endif


#define SI_PATH_JOIN(S1, S2)        S1 SI_PATH_SEPSTR S2
#define SI_PATH_JOIN3(S1, STR2, S3) S1 SI_PATH_SEPSTR S2 SI_PATH_SEPSTR S3 

#define SI_PATH_SJOIN(S1, S2)        #S1 SI_PATH_SEPSTR #S2
#define SI_PATH_SJOIN3(S1, S2, S3) #S1 SI_PATH_SEPSTR #S2 SI_PATH_SEPSTR #S3 


#endif
