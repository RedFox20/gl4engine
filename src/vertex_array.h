#pragma once
#include "shader.h"

////////////////////////////////////////////////////////////////////////////////

// vertex buffer object (uses VAO)
typedef struct vertex_array
{
	unsigned vao;          // vertex array object
	unsigned vbo;          // vertex buffer object
	unsigned ebo;          // element buffer object (if exists)
	unsigned num_verts;    // number of vertices
	unsigned num_indices;  // num element buffer indices (if ebo exists)
} vertex_array;

////////////////////////////////////////////////////////////////////////////////

/**
 * Creates a new VBO to store a vertex array
 * @param vertices Pointer to vertex data
 * @param numVerts Number of vertices, each sizeOf bytes
 * @param sizeOf   Size of Vertex descriptor in bytes
 * @param a0       Type of the first attribute in vertex
 * @param size0    Number of elements per attribute (1-4 floats or ints)

 * @example struct Vertex3UV { vec3 pos; vec2 tex; };
 * @example va_new_array(sizeof(Vertex3UV), a_Position,3,  a_Coord,2);
 * 
 */
vertex_array* va_new_array(const void* vertices, int numVerts, int sizeOf, 
                           ShaderAttribute a0, int size0, ...);

/**
 * Creates a new VBO to store a vertex element array
 * @param vertices Pointer to vertex data
 * @param numVerts Number of vertices, each sizeOf bytes

 * @param sizeOf   Size of Vertex descriptor in bytes
 * @param a0       Type of the first attribute in vertex
 * @param size0    Number of elements per attribute (1-4 floats or ints)

 * @example struct Vertex3UV { vec3 pos; vec2 tex; };
 * @example va_new_indexed_array(sizeof(Vertex3UV), a_Position,3,  a_Coord,2);
 * 
 */
vertex_array* va_new_indexed_array(const void* vertices, int numVerts, 
                                   const unsigned* indices, int numIndices, int sizeOf, 
                                   ShaderAttribute a0, int size0, ...);



/** @brief Destroys vertex array object and buffers */
void va_destroy(vertex_array* va);

/** @brief Draws this vertex array object. */
void va_draw(vertex_array* va);

////////////////////////////////////////////////////////////////////////////////