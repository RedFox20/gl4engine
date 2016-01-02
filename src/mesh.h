#pragma once
#include <stdbool.h>
#include "vertex_array.h"
#include "resmgr.h"

////////////////////////////////////////////////////////////////////////////////

typedef struct BMDModel // definition of our BMDModel format
{
	char Name[32];		// model name
	char TexName[32];	// Texture name
	int NumVertices;	// number of vertices
	int	NumIndices;		// number of indices
	int OffVerts;		// offset to vertices
	int OffInds;		// offset to indices
	// Vertex Data [NumVertices * sizeof(Vertex)] follows
	// Index Data  [NumIndices  * sizeof(Index) ] follows
} BMDModel;

// BMDModel functions
vertex_t* model_vertices(BMDModel* model);
index_t*  model_indices(BMDModel* model);

////////////////////////////////////////////////////////////////////////////////

// Managed by MeshManager and refcounted
typedef struct StaticMesh
{
	Resource       res;   // resource base class
	int            size;  // BMD model size in bytes
	BMDModel*      model; // STRONG REF: model data reference
	vertex_array*  data;  // STRONG REF: GPU vertex array object
} StaticMesh;

typedef struct MeshManager 
{
	ResManager rm;
} MeshManager;

////////////////////////////////////////////////////////////////////////////////
// Implements ResManager Resource interface

// initializes a resource manager for MeshManager objects
MeshManager* meshmgr_init(int maxCount);
// returns pointer to the first resource element
StaticMesh* meshmgr_data(MeshManager* m);
// loads a static mesh, if it's already loaded, simply increments refcount
StaticMesh* meshmgr_load_mesh(MeshManager* m, const char* modelPath);
// decrements refcount, but does not free any resources! use meshmgr_clean_unused()
void meshmgr_free_mesh(StaticMesh* mesh);
// frees any unused (refcount == 0) resources
void meshmgr_clean_unused(MeshManager* m);
// destroys all meshes, regardless of their refcounts
void meshmgr_destroy_all_items(MeshManager* m);
// destroys the resource manager and all its items
void meshmgr_destroy(MeshManager* m);

////////////////////////////////////////////////////////////////////////////////