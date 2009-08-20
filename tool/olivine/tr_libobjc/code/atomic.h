#ifndef __ATOMIC_H_INCLUDED__
#define __ATOMIC_H_INCLUDED__
#ifdef __i386__
#define ATOMIC_INCREMENT(x) atomic_increment(&x) 
#define ATOMIC_DECREMENT(x) atomic_decrement(&x) 
static inline unsigned int atomic_increment(unsigned int * i)
{
	__asm__ __volatile__ (
			"lock addl $1, %0"
			:"=m" (*i));
	return *i;
}
static inline unsigned int atomic_decrement(unsigned int * i)
{
	__asm__ __volatile__ (
			"lock subl $1, %0"
			:"=m" (*i));
	return *i;
}
#elif defined(__PPC__)
#define ATOMIC_INCREMENT(x) atomic_increment(&x) 
#define ATOMIC_DECREMENT(x) atomic_decrement(&x) 
static inline unsigned int atomic_increment(unsigned int * i)
{
	    int tmp;
		__asm__ __volatile__ (
			"modified:"
			"lwarx %0,0,%1 \n"
			"addic %0,%0,1 \n"
			"stwcx. %0,0,%1 \n"
			"bne- modified \n"
			:"=&r" (tmp)
			:"r" (i)
			:"cc", "memory");
		return *i;
}
static inline unsigned int atomic_decrement(unsigned int * i)
{
	    int tmp;
		__asm__ __volatile__ (
			"modified:"
			"lwarx %0,0,%1 \n"
			"addic %0,%0,-1 \n"
			"stwcx. %0,0,%1 \n"
			"bne- modified \n"
			:"=&r" (tmp)
			:"r" (i)
			:"cc", "memory");
		return *i;
}
#else
#warning Unrecognised architecture.  Please add atomic increment and decrement functions to atomic.h
#endif
#endif //__ATOMIC_H_INCLUDED__
