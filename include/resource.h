#pragma once
#include <stdbool.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////

/** A generic managed resource, contains a reference count and resource path ID */
typedef struct Resource
{
	int      refcount;      // @warning: not threadsafe (!!)
	struct ResManager* mgr; // reference to manager
	int      hlen;          // length of path string
	int      fphlen;        // length of path filepart string
	//uint64_t KEY;         // 64-bit path hash
	uint64_t fphash;        // 64-bit filepart hash
	char*    path;          // path for debugging
} Resource;

typedef bool (*ResMgr_LoadFunc)(Resource* res, const char* fullPath);
typedef void (*ResMgr_FreeFunc)(Resource* res);

/** Generic file resource manager for managing assets we do not wish to load multiple times */ 
typedef struct ResManager
{
	int count;    // number of alive/loaded items
	int sizeOf;   // sizeof a single item
	int maxCount; // maximum number of elements we can allocate

	ResMgr_LoadFunc load; // resource load func
	ResMgr_FreeFunc free; // resource free func

	char name[32]; // a small unique name identifier for the resource manager

	// array of [uint64 * maxCount] hash keys
	// array of [sizeOf * maxCount] items follow
} ResManager;

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Loads a resource, if it's already loaded, simply increments refcount
 * @note  If resource does not exist, NULL is returned
 * @param mgr Resourmanager context
 * @param relativePath Relative path to resource file
 */ 
Resource* resource_load(ResManager* rm, const char* relativePath);
#define iresource_load(resmgr, path) resource_load(&resmgr->rm, path)

/** @brief Decrements refcount, but does not free any resources! use resmgr_clean_unused() */
void resource_free(Resource* res);
#define iresource_free(resource) resource_free(&resource->res)

/**
 * @brief Dynamically creates a new Resource Manager
 * @note (!) Items must extend struct Resource (!)
 * 
 * @param maxCount Maximum number of elements this manager can pool
 * @param sizeOf Size of each resource item. SizeOf must be > sizeof(Resource) (!!)
 * @param loadFunc Function to use when initializing a resource
 * @param freeFunc Function to use when destroying a resource
 */
ResManager* res_manager_create(const char* name, int maxCount, int sizeOf, 
	ResMgr_LoadFunc loadFunc, ResMgr_FreeFunc freeFunc);

/** @brief Destroys the resource manager and all its items*/
void res_manager_destroy(ResManager* rm);
#define ires_manager_destroy(resmgr) res_manager_destroy(&resmgr->rm)

/** @brief Returns pointer to the first resource element */
Resource* res_manager_data(ResManager* rm);

/** @brief Frees any unused (refcount == 0) resources */ 
void res_manager_clean_unused(ResManager* rm);

/** @brief Destroys all items, regardless of their refcounts */
void res_manager_destroy_all_items(ResManager* rm);
////////////////////////////////////////////////////////////////////////////////