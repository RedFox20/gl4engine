#include "vertex_array.h"
#include <stdarg.h>
#include <GL/glew.h>
#include <assert.h>
#include <stdlib.h>

// by binding opengl VAO we record all the enabled attribute locations
// with their respective array offsets
static void init_vertex_array_object(vertex_array* v, int sizeOf, va_list ap)
{
	glGenVertexArrays(1, &v->vao);
	glBindVertexArray(v->vao);
	glGenBuffers(GL_ARRAY_BUFFER, &v->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, v->vbo);

	for (int offset = 0; offset < sizeOf;)
	{
		ShaderAttribute a = va_arg(ap, ShaderAttribute); // attrib location
		const int s        = va_arg(ap, int);              // attrib size in floats
		#if DEBUG
			assert(0 < a && a < a_MaxAttributes && "Invalid attribute value, check descriptor list!");
			assert(1 <= s && s <= 4 && "Invalid attribute size, check descriptor list!");
			const int remElements = (sizeOf - offset) / sizeof(float);
			assert(s <= remElements && "Invalid layout, sizeOf doesn't match descriptor list!");
		#endif
		glVertexAttribPointer(a, s, GL_FLOAT, GL_FALSE, sizeOf, (void*)offset);
		glEnableVertexAttribArray(a);
		offset += s * sizeof(float); // offset is in bytes
	}
}

vertex_array* va_new_array(const void* vertices, int numVerts, int sizeOf, 
                          ShaderAttribute a0, int size0, ...)
{
	vertex_array* v = malloc(sizeof(*v));
	v->ebo = 0;
	v->num_verts   = numVerts;
	v->num_indices = 0;

	init_vertex_array_object(v, sizeOf, (va_list)&a0);
	glBufferData(GL_ARRAY_BUFFER, numVerts*sizeOf, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return v;
}

vertex_array* va_new_indexed_array(const void* vertices, int numVerts,
                                   const unsigned* indices, int numIndices, int sizeOf,
                                   ShaderAttribute a0, int size0, ...)
{
	vertex_array* v = malloc(sizeof(*v));
	v->num_verts   = numVerts;
	v->num_indices = numIndices;

	init_vertex_array_object(v, sizeOf, (va_list)&a0);
	glBufferData(GL_ARRAY_BUFFER, numVerts*sizeOf, vertices, GL_STATIC_DRAW);
	glGenBuffers(GL_ELEMENT_ARRAY_BUFFER, &v->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(int), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return v;
}

void va_destroy(vertex_array* va)
{
	if (va->vbo) glDeleteBuffers(1, &va->vbo), va->vbo = 0;
	if (va->ebo) glDeleteBuffers(1, &va->ebo), va->ebo = 0;
	if (va->vao) glDeleteVertexArrays(1, &va->vao), va->vao = 0;
	free(va);
}



void va_draw(vertex_array* va)
{
	glBindVertexArray(va->vao);

	if (va->ebo)
		glDrawElements(GL_TRIANGLES, va->num_indices, GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, va->num_verts);

	glBindVertexArray(0);
}