#pragma once

////////////////////////////////////////////////////////////////////////////////

// dynamic data vector, similar to std::vector<T>
typedef struct vector
{
	int size;     // number of elements in vector
	int capacity; // maximum capacity
	int sizeOf;   // size of a single element
	char* data;   // dynamic data buffer
} vector;

// @return Pointer to vector data, cast to given type
#define vector_data(v, type)   ((type*)(v)->data)
// @return Element of given type at index i
#define vector_at(v, type, i)  vector_data(v,type)[i]
// @return Pointer to item at index i
#define vector_get(v, i)       (&(v)->data[(v)->sizeOf * i])
// @return Pointer to first element
#define vector_begin vector_data
// @return Pointer to last element
#define vector_end(v, type)    (type*)vector_get(v,(v)->size)

/** @brief Creates a new vector to hold objects of given size */
void vector_create(vector* v, int sizeOf);
/** @brief Frees dynamic data buffer. SizeOf is retained. */
void vector_destroy(vector* v);
/** @brief Clears the vector size. Does not free data buffer. */
void vector_clear(vector* v);
/** @brief Appends an item. Pointer to the item must be given */
void vector_append(vector* v, const void* itemPtr);
/** @brief Erases the item at the given index */
void vector_erase(vector* v, int index);
/** @brief Inserts an item at the given index */
void vector_insert(vector* v, int index, const void* itemPtr);

////////////////////////////////////////////////////////////////////////////////

// dynamic vector that contains pointers, similar to std::vector<T*>
typedef struct pvector
{
	int size;      // number of elements in vector
	int capacity;  // maximum capacity
	void** data;   // dynamic buffer of pointers 
} pvector;

#define pvector_T(TYPE) typedef union pvector##TYPE { \
	pvector vec[1]; \
	struct {        \
		int size;           /* number of elements in vector */ \
		int capacity;       /* maximum capacity */             \
		struct TYPE** data; /* dynamic buffer of pointers */   \
	};              \
} pvector##TYPE;

// @return Pointer to pvector data, cast to given type
#define pvector_data(v, type)   ((type**)(v)->data)
// @return Pointer of given type at index i
#define pvector_at(v, type, i)  pvector_data(v,type)[i]
// @return Pointer to item at index i
#define pvector_get(v, i)       (&(v)->data[i])
// @return Pointer to first element
#define pvector_begin pvector_data
// @return Pointer to last element
#define pvector_end(v, type)    (pvector_data(v,type) + (v)->size)

/** @brief Creates a new vector to hold a dynamic array of pointers */
void pvector_create(pvector* v);
/** @brief Frees dynamic data buffer. */
void pvector_destroy(pvector* v);
/** @brief Clears the vector size. Does not free data buffer. */
void pvector_clear(pvector* v);
/** @brief Appends an item. */
void pvector_append(pvector* v, void* item);
/** @brief Erases the item at the given index */
void pvector_erase(pvector* v, int index);
/** @brief Inserts an item at the given index */
void pvector_insert(pvector* v, int index, void* item);

////////////////////////////////////////////////////////////////////////////////