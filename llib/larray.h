#ifndef _LARRAY_H_
#define _LARRAY_H_

typedef struct{
	char *data;
	int len;
	int count;
	int size;
}LArray;

LArray *l_array_new(int count,int size);
void l_array_free(LArray *array,LFreeFunc func);
void l_array_append(LArray *array,const void *val);
#define l_array_nth(array,n) ((void*)(array->data+(n)*array->size))
void l_array_insert(LArray *array,int n,const void *val);
void l_array_insert_sorted(LArray *array,const void *val,LCmpFunc cmpar);
int l_array_remove(LArray *array,int n);
#define l_array_length(array) (array->len)
void l_array_clear(LArray *array,LFreeFunc func);
void l_array_sort(LArray *array,LCmpFunc cmp);
void l_array_sort_r(LArray *array,LCmpDataFunc cmp,void *arg);

typedef LArray LPtrArray;
#define l_ptr_array_new(count) l_array_new((count),sizeof(void*))
void l_ptr_array_append(LArray *array,const void *val);
void l_ptr_array_insert(LArray *array,int n,const void *val);
#define l_ptr_array_remove(array,n) l_array_remove(array,n)
void l_ptr_array_free(LArray *array,LFreeFunc func);
#define l_ptr_array_nth(array,n) (*(void**)(((LArray*)array)->data+(n)*sizeof(void*)))
#define l_ptr_array_length(array) l_array_length(array)
void l_ptr_array_clear(LArray *array,LFreeFunc func);
void l_ptr_array_sort(LArray *array,LCmpFunc cmp);
void l_ptr_array_sort_r(LArray *array,LCmpDataFunc cmp,void *arg);

#endif/*_LARRAY_H_*/
