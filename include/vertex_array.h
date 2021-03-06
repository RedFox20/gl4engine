#pragma once
#include "shader.h"

////////////////////////////////////////////////////////////////////////////////

// describes a single element in a vertex (visualized by .natvis)
typedef struct vertex_descr_elem {
	unsigned char attr; // ShaderAttr vertex attribute slot identifier (a_Position, etc.)
	unsigned char size; // Number of elements per attribute (1-4 floats or ints)
} vertex_descr_elem;

// vertex layout descriptor (visualized by .natvis)
typedef struct vertex_descr
{
	int sizeOf; // Size of your vertex struct in bytes eg: sizeof(Vertex3UV)
	vertex_descr_elem items[4];
} vertex_descr;

// vertex buffer object (uses VAO)
typedef struct vertex_array
{
	unsigned arrayObj;     // vertex array object
	unsigned vertexBuf;    // vertex buffer object
	unsigned indexBuf;     // element buffer object (if exists)
	unsigned vertexCount;  // number of vertices
	unsigned indexCount;   // num element buffer indices (if ebo exists)
	vertex_descr descr;    // vertex layout descriptor
} vertex_array;


////////////////////////////////////////////////////////////////////////////////

/**
 * Creates a new vertex_descr object
 * @param sizeOf Size of your vertex struct in bytes eg: sizeof(Vertex3UV)
 * @note MAX 4 attribute slot identifiers for now
 * @param attr0  First attribute slot identifier (a_Position, etc.)
 * @param size0  Number of elements per attribute (1-4 floats or ints)
 * @example vd_create(sizeof(Vertex3UV), a_Position,3, a_Coord,2);
 */
vertex_descr vd_create(int sizeOf, ShaderAttr attr0, int size0, ...);

/**
 * Creates a new VBO to store a vertex array
 * @param vertices   Pointer to vertex data
 * @param numVerts   Number of vertices, each sizeOf bytes

 * @example struct Vertex3UV { vec3 pos; vec2 tex; };
 * @example vertex_descr vd = { sizeof(Vertex3UV), a_Position,3, a_Coord,2 };
 * @example va_new_array(verts, nverts, vd);
 * 
 */
vertex_array* va_new_array(const void* vertices, int numVerts, vertex_descr vd);

/**
 * Creates a new VBO to store a vertex element array
 * @param vertices   Pointer to vertex data
 * @param numVerts   Number of vertices, each sizeOf bytes
 * @param indices    Pointer to index data
 * @param numIndices Number of indices
 *
 * @example struct Vertex3UV { vec3 pos; vec2 tex; };
 * @example vertex_descr vd = { sizeof(Vertex3UV), a_Position,3, a_Coord,2 };
 * @example va_new_indexed_array(verts, nverts, indices, nindices, vd);
 * 
 */
vertex_array* va_new_indexed_array(const void* vertices, int numVerts, 
                                   const index_t* indices, int numIndices, 
                                   vertex_descr vd);



/** @brief Destroys vertex array object and buffers */
void va_destroy(vertex_array* va);

/** @brief Draws this vertex array object. */
void va_draw(vertex_array* va);

////////////////////////////////////////////////////////////////////////////////