#include "resource.h"
#include "util.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////

static uint64_t fnv64(const void* data, size_t length)
{
	const uint8_t* p = (const uint8_t*)data;
	uint64_t hash = 14695981039346656037ull;
	for (size_t i = 0; i < length; ++i) {
		hash ^= p[i];
		hash *= 1099511628211ull;
	}
	return hash;
}
static int init_hash(uint64_t* outHash, const char* string)
{
	int slen = strlen(string);
	*outHash = fnv64(string, slen);
	return slen;
}

////////////////////////////////////////////////////////////////////////////////

ResManager* res_manager_create(int maxCount, int sizeOf, ResMgr_LoadFunc loadFunc, ResMgr_FreeFunc freeFunc)
{
	assert(sizeOf > sizeof(Resource) && 
		"resmgr_init(): sizeOf not larger than sizeof(Resource)! "
		"Items must extend struct Resource for bookkeeping");
	const int itemBytes  = maxCount + sizeOf;
	const int totalBytes = sizeof(ResManager) + itemBytes;
	ResManager* rm = malloc(totalBytes);
	if (!rm) {
		LOG("resmgr_init(): failed to allocate manager of size %d bytes\n", totalBytes);
		return NULL;
	}
	rm->load     = loadFunc;
	rm->free     = freeFunc;
	rm->maxCount = maxCount;
	rm->sizeOf   = sizeOf;
	rm->count    = 0;
	// default init items array to NULLs:
	memset((char*)rm + sizeof(ResManager), 0, itemBytes);
	return rm;
}

void res_manager_destroy(ResManager* rm)
{
	res_manager_destroy_all_items(rm);
	free(rm);
}

Resource* res_manager_data(ResManager* rm)
{
	return (Resource*)((char*)rm + sizeof(ResManager));
}

#define resmgr_start(resmgr)      (Resource*)((char*)resmgr + sizeof(ResManager))
#define resmgr_next(iter, sizeOf) (Resource*)((char*)iter + sizeOf)
#define resmgr_iterator(iter, rm) \
	const int MAX    = rm->maxCount;     \
	const int SIZEOF = rm->sizeOf;       \
	Resource* iter   = resmgr_start(rm); \
	for (int i=0,rem=rm->count;  rem>0 && i<MAX;  ++i,iter=resmgr_next(iter,SIZEOF))

////////////////////////////////////////////////////////////////////////////////

// TODO: make resource management thread safe (atomic refcounts)
// TODO: implement a proper hashtable
Resource* resource_load(ResManager* rm, const char* relativePath)
{
	char path[260];
	uint64_t hash, fphash;
	int hlen   = init_hash(&hash,   normalized_datapath(path, 260, relativePath));
	int fphlen = init_hash(&fphash, filepart(path));

	int slot = -1;
	resmgr_iterator(res, rm) {
		if (res->hlen) {
			if (res->hlen == hlen && res->fphlen == fphlen && 
				res->hash == hash && res->fphash == fphash) {
				++res->refcount;
				return res; // we have a pretty solid match
			}
			--rem;
		}
		else if (slot == -1) 
			slot = i; // remember free slot
	}

	// no existing items, we must create a new one
	if (slot == -1) {
		LOG("resmgr_load_item(): out of item slots! Failed to load '%s'\n", path);
		return NULL;
	}

	// take item from a free slot
	res = (Resource*)((char*)resmgr_start(rm)  +  slot*rm->sizeOf);
	if (rm->load(res, path)) {
		res->refcount = 1;
		res->mgr    = rm;
		res->hlen   = hlen;
		res->fphlen = fphlen;
		res->hash   = hash;
		res->fphash = fphash;
		++rm->count;
		return res;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void resource_free(Resource* item)
{
	ResManager* rm = item->mgr;
	resmgr_iterator(res, item->mgr) {
		if (!res->hlen) 
			continue;
		if (res == item) {
			--res->refcount;
			assert(res->refcount >= 0 
				&& "resmgr_free_item(): refcount negative, too many frees!");
			return;
		}
		--rem;
	}
	LOG("resmgr_free_item(): Resource sizeOf(%d) not owned "
		"by this ResManager. Critical leak.", rm->sizeOf);
}

////////////////////////////////////////////////////////////////////////////////

static void resmgr_destroy_item(ResManager* rm, Resource* res)
{
	res->hlen = 0; // hlen=0 - this marks the slot as free
	rm->free(res);
	--rm->count;
}

void res_manager_clean_unused(ResManager* rm)
{
	resmgr_iterator(res, rm) {
		if (res->hlen && res->refcount <= 0)
			resmgr_destroy_item(rm, res);
	}
}

void res_manager_destroy_all_items(ResManager* rm)
{
	resmgr_iterator(res, rm) {
		if (res->hlen) 
			resmgr_destroy_item(rm, res);
	}
}
