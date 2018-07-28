

#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef int (*comparefn_t)(void *, void *);
typedef void (*printfn_t)(void *);
typedef void (*freefunc_t)(void *);

#define ARRAYLIST_STARTSIZE 10


typedef struct arraylist{
	void ** value;
	int size;
	int filled;
}arraylist_t;

extern inline arraylist_t * arraylist_new(){
	arraylist_t * arraylist = malloc(sizeof(arraylist_t));
	*arraylist = (arraylist_t){
		malloc(ARRAYLIST_STARTSIZE*sizeof(void *)),
		ARRAYLIST_STARTSIZE,
		0
	};
	return arraylist;
}

static inline void arraylist_resize(arraylist_t * arraylist,int newsize){
	arraylist->value = realloc(arraylist->value,newsize*sizeof(void *));
	arraylist->size = newsize;
}

extern inline void arraylist_setvalue(arraylist_t * arraylist,int index,void * value){
	if(index > arraylist->filled){
		printf("arraylist index out of bounds\n");
		exit(-1);
		return;
	}
	arraylist->value[index] = value;
}

extern inline void * arraylist_getvalue(arraylist_t * arraylist, int index){
	if(index > arraylist->filled){
		printf("arraylist index out of bounds\n");
		exit(-1);
	}

	if(index < 0){
		index = arraylist->filled + index;
	}

	if(index < 0){
		printf("arraylist index out of bounds\n");
		exit(-1);
	}

	return arraylist->value[index];
}

extern inline void arraylist_print(arraylist_t * arraylist, printfn_t printfn){
	printf("arraylist [");
	for(int i = 0; i<arraylist->filled;i++){
		void * item = arraylist->value[i];
		if(i != 0) {printf(",");}
		printfn(item);
	}
	printf("]\n");
}

extern inline void arraylist_print_nonewline(arraylist_t * arraylist, printfn_t printfn){
	printf("arraylist [");
	for(int i = 0; i<arraylist->filled;i++){
		void * item = arraylist->value[i];
		if(i != 0) {printf(",");}
		printfn(item);
	}
	printf("]");
}


extern inline void arraylist_append(arraylist_t * arraylist,void * value){
	if(arraylist->filled >= arraylist->size){
		arraylist_resize(arraylist,arraylist->size*2);	
	}
	arraylist->value[arraylist->filled] = value;
	arraylist->filled++;
}

extern inline void arraylist_insert(arraylist_t * arraylist,int index,void * value){
	if(arraylist->filled >= arraylist->size){
		arraylist_resize(arraylist,arraylist->size*2);	
	}
	for (int i = arraylist->filled; i >= index; --i){
		arraylist->value[i+1] = arraylist->value[i];
	}
	arraylist->value[index] = value;
	arraylist->filled++;
}

extern inline int arraylist_length(arraylist_t * arraylist){
	return arraylist->filled;
}

#if DEBUG
extern inline int arraylist_size(arraylist_t * arraylist){
	return arraylist->size;
}
#endif

extern inline int arraylist_find(arraylist_t * arraylist,void * value, comparefn_t comparefn){
	for(int i = 0; i<arraylist->filled;i++){
		void * item = arraylist->value[i];
		if(comparefn(item,value)) {
			return i;
		}
	}
	return -1;	
}


static inline int arraylist_findn(arraylist_t * arraylist,void * value, comparefn_t comparefn, int start){
	for(int i = 0; i<arraylist->filled;i++){
		void * item = arraylist->value[i];
		if(comparefn(item,value)) {
			return i;
		}
	}
	return -1;	
}

extern inline bool arraylist_contains(arraylist_t * arraylist,void * value, comparefn_t comparefn){
	return arraylist_find(arraylist,value,comparefn) != -1;
}

extern inline void * arraylist_delete(arraylist_t * arraylist,int index){
	if(index < 0){
		index = arraylist->filled + index;
	}

	if(index > arraylist->filled || index < 0){
		return NULL;
	}

	if(arraylist->filled < arraylist->size / 2){
		arraylist_resize(arraylist,arraylist->size/2);
	}

	void * removed_data = arraylist->value[index];
	for (int i = index; i < arraylist->filled-1; ++i){
		arraylist->value[i] = arraylist->value[i+1];
	}
	arraylist->filled--;
	return removed_data;
}

extern inline void * arraylist_remove(arraylist_t * arraylist,void * value, comparefn_t comparefn){
	int index = arraylist_find(arraylist,value,comparefn);
	if(index == -1){
		return NULL;
	}
	return arraylist_delete(arraylist,index);
}

extern inline arraylist_t * arraylist_removeall(arraylist_t * arraylist,void * value, comparefn_t comparefn){
	int index = 0;
	arraylist_t * removed = arraylist_new(); 
	while(true){
		index = arraylist_findn(arraylist,value,comparefn,index);
		if(index == -1){
			return removed;
		}
		arraylist_append(removed,arraylist_delete(arraylist,index));
		index--;
	}
	
}


extern inline void arraylist_free(arraylist_t * arraylist){
	free(arraylist->value);
	free(arraylist);
}

extern inline void arraylist_freeall(arraylist_t * arraylist){
	for (int i = 0; i < arraylist->filled; ++i){
		free(arraylist->value[i]);
	}
	free(arraylist->value);
	free(arraylist);
}

extern inline void arraylist_freefunc(arraylist_t * arraylist, freefunc_t freefunc){
	for (int i = 0; i < arraylist->filled; ++i){
		freefunc(arraylist->value[i]);
	}
	free(arraylist->value);
	free(arraylist);
}

extern inline arraylist_t * arraylist_copy(arraylist_t * arraylist){
	arraylist_t * newlist = arraylist_new();
	for(int i = 0; i<arraylist->filled;i++){
		void * item = arraylist->value[i];
		arraylist_append(newlist,item);
	}
	return newlist;
}

extern inline void arraylist_extend(arraylist_t * arraylist,arraylist_t * other){
	for(int i = 0; i<arraylist->filled;i++){
		arraylist_append(arraylist,arraylist->value[i]);
	}
}

extern inline void arraylist_clear(arraylist_t * arraylist){
	free(arraylist->value);
	arraylist->value = malloc(ARRAYLIST_STARTSIZE*sizeof(void *));
	arraylist->size = ARRAYLIST_STARTSIZE;
	arraylist->filled = 0;
}

extern inline void arraylist_clearall(arraylist_t * arraylist){
	for (int i = 0; i < arraylist->filled; ++i){
		free(arraylist->value[i]);
	}
	free(arraylist->value);
	arraylist->value = malloc(ARRAYLIST_STARTSIZE*sizeof(void *));
	arraylist->size = ARRAYLIST_STARTSIZE;
	arraylist->filled = 0;
}


#endif