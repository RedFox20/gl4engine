#pragma once
#include <stdbool.h>
#include "vertex_array.h"
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////

typedef struct BMDModel // definition of our BMDModel format
{
	char name[32];		// model name
	char tex_name[32];	// Texture name
	int num_verts;	    // number of vertices
	int	num_indices;	// number of indices
	int off_verts;		// offset to vertices
	int off_indices;	// offset to indices
	// Vertex Data [num_verts    * sizeof(vertex_t)] follows
	// Index Data  [num_indices  * sizeof(index_t) ] follows
} BMDModel;

// BMDModel functions
vertex_t* model_vertices(BMDModel* model);
index_t*  model_indices(BMDModel* model);

////////////////////////////////////////////////////////////////////////////////

// Managed by ResManager and refcounted
typedef struct StaticMesh
{
	Resource       res;   // resource base class
	int            size;  // BMD model size in bytes
	BMDModel*      model; // STRONG REF: model data reference
	vertex_array*  array; // STRONG REF: GPU vertex array object
} StaticMesh;

typedef struct MeshManager
{
	ResManager rm;
} MeshManager;

// initializes a resource manager for StaticMesh objects
MeshManager* mesh_manager_create(int maxCount);

////////////////////////////////////////////////////////////////////////////////