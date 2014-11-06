#ifndef pique_H_INCLUDED
#define pique_H_INCLUDED


/* a Pevent is a physical event. */
typedef struct Pevent_ Pevent;
/*  Phisical event queue. */
typedef struct Pique_ Pique;


Pique * pique_new();
void pique_free(Pique * me);

int pique_push(Pique * me, Pevent * data);
int pique_peek(Pique * me, Pevent * data);
int pique_pop(Pique * me, Pevent * data);



#endif




