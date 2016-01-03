#include "mesh.h"
#include <stdlib.h>
#include "util.h"

////////////////////////////////////////////////////////////////////////////////

vertex_t* model_vertices(BMDModel* model)
{
	return (vertex_t*)((char*)model + model->off_verts);
}
index_t* model_indices(BMDModel* model)
{
	return (index_t*)((char*)model + model->off_indices);
}

////////////////////////////////////////////////////////////////////////////////

static void _mesh_free(StaticMesh* sm)
{
	if (sm->model) free(sm->model);
	if (sm->array)  va_destroy(sm->array);
}
static bool _mesh_load(StaticMesh* sm, const char* fullPath)
{
	// read the 3D model data
	FILE* f = fopen(fullPath, "rb");
	if (f == 0) {
		printf("meshmgr_load(): fopen failed %s\n", fullPath);
		return false;
	}
	
	int size = sm->size = fsize(f);
	// allocate memory for the model and read the binary data:
	BMDModel* m = sm->model = malloc(size);
	if (!m) { // good practice: recover gracefully if something breaks
		printf("meshmgr_load(): malloc %dKB failed %s\n", size/1024, fullPath);
		return false;
	}
	fread(m, size, 1, f);
	fclose(f); // close the file handle (!)

	// finalize mesh data by uploading it to the GPU
	vertex_descr descr = { sizeof(vertex_t), a_Position,3, a_Coord,2, a_Normal,3 };
	sm->array = va_new_indexed_array(
		model_vertices(m), m->num_verts,
		model_indices(m),  m->num_indices, descr 
	);

	#if DEBUG
		printf("------------------\n");
		printf("FileSize: %d\n", size);
		printf("Loaded model   %s (%dKB)\n", m->name, size / 1024);
		printf("  Texture      %s\n", m->tex_name);
		printf("  NumVertices  %d\n", m->num_verts);
		printf("  NumIndices   %d\n", m->num_indices);
		printf("  Polys        %d\n", m->num_indices/3);
		vertex_t* verts   = model_vertices(m);
		index_t*  indices = model_indices(m);
		for (int i = 0; i < 20 && i < m->num_verts; ++i) {
			vertex_t* v = &verts[i];
			printf("  v%d   %f,	%f,	%f	%f,	%f\n", i,v->x,v->y,v->z, v->u,v->v);
		}
		for (int i = 0; i < 20 && i < m->num_indices; i += 3) {
			printf("  f%d    %d %d %d\n", (i/3), indices[i], indices[i+1], indices[i+2]);
		}
		printf("-------------------\n");
	#endif
	return true;
}

////////////////////////////////////////////////////////////////////////////////

MeshManager* mesh_manager_create(int maxCount) {
	static int id = 0;
	char name[32]; snprintf(name, 32, "mesh_manager_$%d_[%d]", id++, maxCount);
	return (MeshManager*)res_manager_create(name, maxCount, sizeof(StaticMesh), 
		(ResMgr_LoadFunc)_mesh_load, (ResMgr_FreeFunc)_mesh_free);
}


////////////////////////////////////////////////////////////////////////////////

