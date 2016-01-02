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

static void meshmgr_free(StaticMesh* sm)
{
	if (sm->model) free(sm->model);
	if (sm->data)  va_destroy(sm->data);
}
static bool _meshmgr_load(StaticMesh* sm, const char* fullPath)
{
	// read the 3D model data
	FILE* f = fopen(fullPath, "rb");
	if (f == 0) {
		printf("meshmgr_load(): fopen failed %s\n", fullPath);
		return false;
	}
	
	int size = fsize(f);
	// allocate memory for the model and read the binary data:
	BMDModel* m = sm->model = malloc(size);
	if (!m) { // good practice: recover gracefully if something breaks
		printf("meshmgr_load(): malloc %dKB failed %s\n", size/1024, fullPath);
		return false;
	}
	fread(m, size, 1, f);
	fclose(f); // close the file handle (!)

	// finalize mesh data by uploading it to the GPU
	sm->size = size;
	sm->data = va_new_indexed_array(
		model_vertices(m), m->NumVertices,
		model_indices(m),  m->NumIndices,
		sizeof(vertex_t), a_Position,3, a_Coord,2, a_Normal,3
	);
	return true;
}
static bool meshmgr_load(StaticMesh* sm, const char* fullPath)
{
	if (_meshmgr_load(sm, fullPath)) {
		#if DEBUG
			BMDModel* m = sm->model;
			printf("------------------\n");
			printf("FileSize: %d\n", sm->size);
			printf("Loaded model   %s (%dKB)\n", m->Name, sm->size / 1024);
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
	meshmgr_free(sm); // unload model on failure
	return false;
}

////////////////////////////////////////////////////////////////////////////////

MeshManager* meshmgr_init(int maxCount) {
	return (MeshManager*)resmgr_init(maxCount, sizeof(StaticMesh), 
		(ResMgr_LoadFunc)meshmgr_load, (ResMgr_FreeFunc)meshmgr_free);
}
StaticMesh* meshmgr_data(MeshManager* m) {
	return (StaticMesh*)resmgr_data(&m->rm);
}
StaticMesh* meshmgr_load_mesh(MeshManager* m, const char* modelPath) {
	return (StaticMesh*)resmgr_load_item(&m->rm, modelPath);
}
void meshmgr_free_mesh(StaticMesh* mesh) {
	resmgr_free_item(&mesh->res);
}
void meshmgr_clean_unused(MeshManager* m) {
	resmgr_clean_unused(&m->rm);
}
void meshmgr_destroy_all_items(MeshManager* m) {
	resmgr_destroy_all_items(&m->rm);
}
void meshmgr_destroy(MeshManager* m) {
	resmgr_destroy(&m->rm);
}

////////////////////////////////////////////////////////////////////////////////

