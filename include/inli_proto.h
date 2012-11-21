/* From 'src/inli.c': */
Inli * inli_initall (Inli * self , Inli * next , Inli * prev );
Inli * inli_init (Inli * self );
Inli * inli_remove (Inli * self );
Inli * inli_add (Inli * self , Inli * other );
Inli * inli_next (Inli * self );
Inli * inli_prev (Inli * self );
void * inli_data (Inli * self , int offset );
