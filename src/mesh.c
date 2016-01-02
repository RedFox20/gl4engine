#include "mesh.h"
#include <stdlib.h>
#include "util.h"

////////////////////////////////////////////////////////////////////////////////

vertex_t* model_vertices(BMDModel* model)
{
	return (vertex_t*)((char*)model + model->OffVerts);
}
index_t* model_indices(BMDModel* model)
{
	return (index_t*)((char*)model + model->OffInds);
}

////////////////////////////////////////////////////////////////////////////////

static void _mesh_free(StaticMesh* sm)
{
	if (sm->model) free(sm->model);
	if (sm->data)  va_destroy(sm->data);
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
	sm->data = va_new_indexed_array(
		model_vertices(m), m->NumVertices,
		model_indices(m),  m->NumIndices, descr 
	);

	#if DEBUG
		printf("------------------\n");
		printf("FileSize: %d\n", size);
		printf("Loaded model   %s (%dKB)\n", m->Name, size / 1024);
		printf("  Texture      %s\n", m->TexName);
		printf("  NumVertices  %d\n", m->NumVertices);
		printf("  NumIndices   %d\n", m->NumIndices);
		printf("  Polys        %d\n", m->NumIndices/3);
		vertex_t* verts   = model_vertices(m);
		index_t*  indices = model_indices(m);
		for (int i = 0; i < 20 && i < m->NumVertices; ++i) {
			vertex_t* v = &verts[i];
			printf("  v%d   %f,	%f,	%f	%f,	%f\n", i,v->x,v->y,v->z, v->u,v->v);
		}
		for (int i = 0; i < 20 && i < m->NumIndices; i += 3) {
			printf("  f%d    %d %d %d\n", (i/3), indices[i], indices[i+1], indices[i+2]);
		}
		printf("-------------------\n");
	#endif
	return true;
}

////////////////////////////////////////////////////////////////////////////////
StaticMesh* mesh_load(MeshManager* m, const char* modelPath) {
	return (StaticMesh*)resource_load(&m->rm, modelPath);
}
void mesh_free(StaticMesh* mesh) {
	resource_free(&mesh->res);
}
MeshManager* mesh_manager_create(int maxCount) {
	return (MeshManager*)res_manager_create(maxCount, sizeof(StaticMesh), 
		(ResMgr_LoadFunc)_mesh_load, (ResMgr_FreeFunc)_mesh_free);
}
void mesh_manager_destroy(MeshManager* m) {
	res_manager_destroy(&m->rm);
}
StaticMesh* mesh_manager_data(MeshManager* m) {
	return (StaticMesh*)res_manager_data(&m->rm);
}
void mesh_manager_clean_unused(MeshManager* m) {
	res_manager_clean_unused(&m->rm);
}
void mesh_manager_destroy_all_items(MeshManager* m) {
	res_manager_destroy_all_items(&m->rm);
}

////////////////////////////////////////////////////////////////////////////////

