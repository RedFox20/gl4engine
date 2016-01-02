#include "vertex_array.h"
#include <GL/glew.h> // glGenVertexArrays
#include <assert.h>  // assert
#include <stdlib.h>  // malloc
#include <stdarg.h>  // va_list

vertex_descr vd_create(int sizeOf, ShaderAttr attr0, int size0, ...)
{
	vertex_descr vd = { sizeOf, attr0, size0};
	va_list ap;	va_start(ap, size0);
	
	int offset = size0*sizeof(float);
	for (int i = 1; offset < sizeOf && i < 4; ++i)
	{
		ShaderAttr a = va_arg(ap, ShaderAttr); // attrib location
		const int  s = va_arg(ap, int);        // attrib size in floats
		assert(0 <= a && a < a_MaxAttributes && "Invalid attr: check vertex_descr!");
		assert(1 <= s && s <= 4 && "Invalid attr size: check vertex_descr!");
		assert(s <= (sizeOf-offset)/(int)sizeof(float) && "Invalid layout: vertex_descr sizeOf doesn't match total attr sizes!");
		{
			vd.items[i].attr = a;
			vd.items[i].size = s;
		}
		offset += s * sizeof(float); // offset is in bytes
	}
	assert(offset == sizeOf && "Invalid layout: end offset does not match vertex_descr sizeOf!");
	return vd;
}

// by binding opengl VAO we record all the enabled attribute locations
// with their respective array offsets
static void init_vertex_array_object(vertex_array* v, vertex_descr vd)
{
	glGenVertexArrays(1, &v->vao);
	glBindVertexArray(v->vao);
	glGenBuffers(1, &v->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, v->vbo);

	int offset = 0;
	for (int i = 0; offset < vd.sizeOf && i < 4; ++i)
	{
		ShaderAttr a = (ShaderAttr)vd.items[i].attr; // attrib location
		const int s  = (const int) vd.items[i].size; // attrib size in floats
		assert(0 <= a && a < a_MaxAttributes && "Invalid attr: check vertex_descr!");
		assert(1 <= s && s <= 4 && "Invalid attr size: check vertex_descr!");
		assert(s <= (vd.sizeOf-offset)/(int)sizeof(float) && "Invalid layout: vertex_descr sizeOf doesn't match total attr sizes!");
		{
			glVertexAttribPointer(a, s, GL_FLOAT, GL_FALSE, vd.sizeOf, (void*)offset);
			glEnableVertexAttribArray(a);
		}
		offset += s * sizeof(float); // offset is in bytes
	}
	assert(offset == vd.sizeOf && "Invalid layout: end offset does not match vertex_descr sizeOf!");
}

vertex_array* va_new_array(const void* vertices, int numVerts, vertex_descr vd)
{
	vertex_array* v = malloc(sizeof(*v));
	v->ebo = 0;
	v->num_verts   = numVerts;
	v->num_indices = 0;

	init_vertex_array_object(v, vd);
	glBufferData(GL_ARRAY_BUFFER, numVerts*vd.sizeOf, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return v;
}

vertex_array* va_new_indexed_array(const void* vertices, int numVerts,
                                   const index_t* indices, int numIndices, 
                                   vertex_descr vd)
{
	vertex_array* v = malloc(sizeof(*v));
	v->num_verts   = numVerts;
	v->num_indices = numIndices;

	init_vertex_array_object(v, vd);
	glBufferData(GL_ARRAY_BUFFER, numVerts*vd.sizeOf, vertices, GL_STATIC_DRAW);
	glGenBuffers(1, &v->ebo);
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