#include "vertex_array.h"
#include <GL/glew.h> // glGenVertexArrays
#include <assert.h>  // assert
#include <stdlib.h>  // malloc
#include <stdarg.h>  // va_list
#include "util.h"

// validate correctness of vertex descr layout
#if DEBUG
static void vd_validate(vertex_descr* vd)
{
	int offset = 0;
	for (int i = 0; offset < vd->sizeOf && i < 4; ++i) {
		ShaderAttr a = (ShaderAttr)vd->items[i].attr; // attrib location
		const int s  = (const int) vd->items[i].size; // attrib size in floats
		assert(0 <= a && a < a_MaxAttributes && "Invalid attr: check vertex_descr!");
		assert(1 <= s && s <= 4 && "Invalid attr size: check vertex_descr!");
		assert(s <= (vd->sizeOf-offset)/(int)sizeof(float) && "Invalid layout: vertex_descr sizeOf doesn't match total attr sizes!");
		offset += s * sizeof(float); // offset is in bytes
	}
	assert(offset == vd->sizeOf && "Invalid layout: end offset does not match vertex_descr sizeOf!");
}
#endif

vertex_descr vd_create(int sizeOf, ShaderAttr attr0, int size0, ...)
{
	vertex_descr vd = { sizeOf, attr0, size0};
	va_list ap;	va_start(ap, size0);
	
	int offset = size0*sizeof(float);
	for (int i = 1; offset < sizeOf && i < 4; ++i) {
		vd.items[i].attr = va_arg(ap, ShaderAttr);  // attrib location
		vd.items[i].size = va_arg(ap, int);         // attrib size in floats
		offset += vd.items[i].size * sizeof(float); // offset is in bytes
	}
	indebug(vd_validate(&vd));
	return vd;
}

// By using a bound opengl VAO we record all the enabled attribute locations
// with their respective array offsets during calls to
// glEnableVertexAttribArray/glVertexAttribPointer 
static void vao_set_attributes(vertex_descr* vd)
{
	for (int i = 0, off = 0; off < vd->sizeOf; ++i) {
		ShaderAttr a = (ShaderAttr)vd->items[i].attr; // attrib location
		const int s  = (const int) vd->items[i].size; // attrib size in floats
		glVertexAttribPointer(a, s, GL_FLOAT, 0, vd->sizeOf, (void*)off);
		glEnableVertexAttribArray(a);
		off += s * sizeof(float); // offset is in bytes
	}
}

vertex_array* va_new_array(const void* vertices, int numVerts, vertex_descr vd)
{
	indebug(vd_validate(&vd));
	vertex_array* v = malloc(sizeof(*v));
	v->indexBuf    = 0;
	v->vertexCount = numVerts;
	v->indexCount  = 0;
	v->descr       = vd;

	glGenVertexArrays(1, &v->arrayObj);
	glBindVertexArray(v->arrayObj);     // bind VAO to start recording
	{
		// create & fill vertex buffer
		glGenBuffers(1, &v->vertexBuf);
		glBindBuffer(GL_ARRAY_BUFFER, v->vertexBuf);
		glBufferData(GL_ARRAY_BUFFER, numVerts*vd.sizeOf, vertices, GL_STATIC_DRAW);
		// set VAO vertex attributes
		vao_set_attributes(&vd);
	}
	glBindVertexArray(0);
	return v;
}

vertex_array* va_new_indexed_array(const void* vptr, int vtxCnt, 
                                   const index_t* iptr, int idxCnt, vertex_descr vd)
{
	indebug(vd_validate(&vd));
	vertex_array* v = malloc(sizeof(*v));
	v->vertexCount = vtxCnt;
	v->indexCount  = idxCnt;
	v->descr       = vd;

	glGenVertexArrays(1, &v->arrayObj);
	glBindVertexArray(v->arrayObj);     // bind VAO to start recording
	{
		// create and fill index buffer
		glGenBuffers(1, &v->indexBuf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v->indexBuf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxCnt*sizeof(*iptr), iptr, GL_STATIC_DRAW);
		// create & fill vertex buffer
		glGenBuffers(1, &v->vertexBuf);
		glBindBuffer(GL_ARRAY_BUFFER, v->vertexBuf);
		glBufferData(GL_ARRAY_BUFFER, vtxCnt*vd.sizeOf, vptr, GL_STATIC_DRAW);
		// set VAO vertex attributes
		vao_set_attributes(&vd);
	}
	glBindVertexArray(0);
	return v;
}

void va_destroy(vertex_array* va)
{
	if (va->vertexBuf) glDeleteBuffers(1, &va->vertexBuf),     va->vertexBuf = 0;
	if (va->indexBuf)  glDeleteBuffers(1, &va->indexBuf),      va->indexBuf  = 0;
	if (va->arrayObj)  glDeleteVertexArrays(1, &va->arrayObj), va->arrayObj  = 0;
	free(va);
}

void va_draw(vertex_array* va)
{
	glBindVertexArray(va->arrayObj);
	if (va->indexBuf)
	{
		glDrawElements(GL_TRIANGLES, va->indexCount, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, va->vertexCount);
	}
	glBindVertexArray(0);
}