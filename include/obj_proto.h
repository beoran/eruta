/* This file was generated with:
'cfunctions -c -aoff -n -w obj_proto src/obj.c' */
#ifndef CFH_OBJ_PROTO
#define CFH_OBJ_PROTO

/* From 'src/obj.c': */

#line 68 "src/obj.c"
ObjPool * objpool_init (ObjPool * pool );

#line 88 "src/obj.c"
void * objpool_register_data (ObjPool * pool , void * data );

#line 108 "src/obj.c"
void * obj_alloc_in_pool (ObjPool * pool , size_t size , ObjClass * klass );

#line 128 "src/obj.c"
void * obj_alloc (size_t size , ObjClass * klass );

#line 147 "src/obj.c"
void * obj_ref (void * ptr );

#line 159 "src/obj.c"
ObjClass * obj_class (void * ptr );

#line 166 "src/obj.c"
ObjMethod * objclass_method_at (ObjClass * klass , size_t offset );

#line 175 "src/obj.c"
ObjMethod * objclass_getfree (ObjClass * klass );

#line 182 "src/obj.c"
void * obj_done (void * ptr );

#line 202 "src/obj.c"
void * obj_free (void * ptr );

#line 224 "src/obj.c"
void * obj_unref (void * ptr );

#line 244 "src/obj.c"
ObjPool * objpool_unref (ObjPool * pool );

#endif /* CFH_OBJ_PROTO */
