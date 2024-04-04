#ifndef _LMACROS_H_
#define _LMACROS_H_

#ifdef __GNUC__
  #define L_LIKELY(x) __builtin_expect(!!(x), 1)
  #define L_UNLIKELY(x) __builtin_expect(!!(x),0)
#else
  #define L_LIKELY(x) (x)
  #define L_UNLIKELY(x) (x)
#endif

#ifdef _WIN32
  #define L_EXPORT(x) __declspec(dllexport) extern x;x
#elif __GNUC__ >= 4
  #define L_EXPORT(x) extern x __attribute__((visibility("default")));x
#else
  #define L_EXPORT(x) x
#endif

#if __STDC_VERSION>=201112L
	#define L_ALIGN(x,a)    alignas(a) x
#elif __GNUC__ >= 4
	#define L_ALIGN(x,a)	x __attribute__((aligned(a)))
#else
	#define L_ALIGN(x,a)	x
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (!FALSE)
#endif

#define L_VA_NUM_ARGS(...) L_VA_NUM_ARGS_IMPL(__VA_ARGS__, 9,8,7,6,5,4,3,2,1)
#define L_VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) N

#define L_CAT(a,b) L_CAT_IMPL(a,b)
#define L_CAT_IMPL(a,b) a##b

#define L_TO_STR_IMPL(s)	#s
#define L_TO_STR(s)	L_TO_STR_IMPL(s)

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifdef __GNUC__
#define array_index(a,c,v)				\
(__extension__							\
	({									\
		typeof(v) *__a=(typeof(v)*)(a);	\
		int __c=(int)(c);				\
		typeof(v) __v=(v);				\
		int __r=-1;						\
		for(int __i=0;i<__c;__i++)		\
		{								\
			if(__v==__a[__i])			\
			{							\
				__r=__i;				\
				break;					\
			}							\
		}								\
		__r;							\
	})									\
)
#define array_includes(a,c,v) (array_index((a),(c),(v))>=0)
#endif

#define SWAP(a,i,j)		\
do{							\
	int _i=(int)(i);		\
	int _j=(int)(j);		\
	typeof(a[0]) t=a[_i];	\
	a[_i]=a[_j];			\
	a[_j]=t;				\
}while(0)

#endif/*_LMACROS_H_*/
