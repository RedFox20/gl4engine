#include "vector.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>


////////////////////////////////////////////////////////////////////////////////


void vector_create(vector* v, int sizeOf)
{
	v->size     = 0;
	v->capacity = 0;
	v->sizeOf   = sizeOf;
	v->data     = NULL;
}

void vector_destroy(vector* v)
{
	v->size = 0;
	v->capacity = 0;
	if (v->data) {
		free(v->data);
		v->data = 0;
	}
}

void vector_clear(vector* v)
{
	v->size = 0;
}

void vector_append(vector* v, const void* itemPtr)
{
	if (v->size == v->capacity) {
		v->capacity += 4 + (v->capacity / 2);
		v->data = (char*)realloc(v->data, v->capacity * v->sizeOf);
	}
	char* end = &v->data[v->size * v->sizeOf];
	memcpy(end, itemPtr, v->sizeOf);
	++v->size;
}

void vector_erase(vector* v, int index)
{
    char* end = &v->data[v->size * v->sizeOf];
    char* dst = &v->data[index * v->sizeOf];
    char* src = dst + v->sizeOf;

    // | 0 | 1 | 2 | 3 | 4 | ... |
    //  dst^   ^src              ^end
    //      <--   move from src to dst, all the bytes between [src .. end]
    // | 0 | 2 | 3 | 4 | ... |
    memmove(dst, src, (char*)end - (char*)src);
    --v->size;
}

void vector_insert(vector* v, int index, const void* itemPtr)
{
	if (v->size == v->capacity) {
		v->capacity += 4 + (v->capacity / 2);
		v->data = (char*)realloc(v->data, v->capacity * v->sizeOf);
	}

    char* end = &v->data[v->size * v->sizeOf];
    char* src = &v->data[index * v->sizeOf];
    char* dst = src + v->sizeOf;

    // | 0 | 1 | 2 | 3 | ... |
    //  src^   ^dst          ^end
    //      -->   move from src to dst, all the bytes between [src .. end]
    // leaving an empty array slot in the middle
    // | 0 | X | 1 | 2 | 3 | ... |
    memmove(dst, src, (char*)end - (char*)src);
	memcpy(src, itemPtr, v->sizeOf);
    ++v->size;
}



////////////////////////////////////////////////////////////////////////////////



void pvector_create(pvector* v)
{
	v->size     = 0;
	v->capacity = 0;
	v->data     = NULL;
}

void pvector_destroy(pvector* v)
{
	v->size = 0;
	v->capacity = 0;
	if (v->data) {
		free(v->data);
		v->data = 0;
	}
}

void pvector_clear(pvector* v)
{
	v->size = 0;
}

void pvector_append(pvector* v, void* item)
{
	if (v->size == v->capacity) {
		v->capacity += 4 + (v->capacity / 2);
		v->data = (void**)realloc(v->data, v->capacity * sizeof(void*));
	}
	v->data[v->size++] = item;
}

void pvector_erase(pvector* v, int index)
{
    void** end = &v->data[v->size];
    void** dst = &v->data[index];
    void** src = dst + 1;

    // | 0 | 1 | 2 | 3 | 4 | ... |
    //  dst^   ^src              ^end
    //      <--   move from src to dst, all the bytes between [src .. end]
    // | 0 | 2 | 3 | 4 | ... |
    memmove(dst, src, (char*)end - (char*)src);
    --v->size;
}

void pvector_insert(pvector* v, int index, void* item)
{
	if (v->size == v->capacity) {
		v->capacity += 4 + (v->capacity / 2);
		v->data = (void**)realloc(v->data, v->capacity * sizeof(void*));
	}

    void** end = &v->data[v->size];
    void** src = &v->data[index];
    void** dst = src + 1;

    // | 0 | 1 | 2 | 3 | ... |
    //  src^   ^dst          ^end
    //      -->   move from src to dst, all the bytes between [src .. end]
    // leaving an empty array slot in the middle
    // | 0 | X | 1 | 2 | 3 | ... |
    memmove(dst, src, (char*)end - (char*)src);
	*src = item;
	++v->size;
}

////////////////////////////////////////////////////////////////////////////////