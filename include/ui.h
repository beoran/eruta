#ifndef ui_H_INCLUDED
#define ui_H_INCLUDED

typedef struct UI_ UI;

UI * ui_alloc(void);
void vva_list_scan (va_list args , char * format , va_list data );
void va_list_scan (va_list args , char * format , ... );




#endif




