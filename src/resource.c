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


#define resmgr_begin(resmgr)      (Resource*)((char*)resmgr + sizeof(ResManager))
#define resmgr_end(resmgr)        (Resource*)((char*)resmgr_begin(resmgr) + resmgr->maxCount * resmgr->sizeOf)
#define resmgr_next(iter, sizeOf) (iter=(Resource*)((char*)iter + sizeOf))
#define resmgr_iterator(iter, rm)    \
	const int SIZEOF = rm->sizeOf;       \
	Resource* iter   = resmgr_begin(rm); \
	Resource* e      = resmgr_end(rm);   \
	for (int rem = rm->count; iter != e && (iter->hlen?--rem:rem)>0; resmgr_next(iter,SIZEOF))

////////////////////////////////////////////////////////////////////////////////

ResManager* res_manager_create(int maxCount, int sizeOf, ResMgr_LoadFunc loadFunc, ResMgr_FreeFunc freeFunc)
{
	assert(sizeOf > sizeof(Resource) && 
		"resmgr_init(): sizeOf not larger than sizeof(Resource)! "
		"Items must extend struct Resource for bookkeeping");
	const int itemBytes  = maxCount * sizeOf;
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

	Resource* res = resmgr_begin(rm); // default init NULL markers:
	for (Resource* e = resmgr_end(rm); res != e; resmgr_next(res, sizeOf)) {
		res->hlen = 0;
	}
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

////////////////////////////////////////////////////////////////////////////////

// TODO: make resource management thread safe (atomic refcounts)
// TODO: implement a proper hashtable
Resource* resource_load(ResManager* rm, const char* relativePath)
{
	char path[260];
	uint64_t hash, fphash;
	int hlen   = init_hash(&hash,   normalized_datapath(path, 260, relativePath));
	int fphlen = init_hash(&fphash, filepart(path));

	Resource* free = NULL;
	resmgr_iterator(res, rm) {
		if (res->hlen) {
			if (res->hlen == hlen && res->fphlen == fphlen && 
				res->hash == hash && res->fphash == fphash) {
				++res->refcount;
				return res; // we have a pretty solid match
			}
		} 
		else if (!free) free = res; // rememeber free slot
	}

	if (rm->count == rm->maxCount) {
		LOG("resmgr_load_item(): out of item slots! Failed to load '%s'\n", path);
		return NULL;
	}

	if (!free) free = resmgr_begin(rm);
	if (rm->load(free, path)) {
		free->refcount = 1;
		free->mgr    = rm;
		free->hlen   = hlen;
		free->fphlen = fphlen;
		free->hash   = hash;
		free->fphash = fphash;
		++rm->count;
		return free;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void resource_free(Resource* item)
{
	ResManager* rm = item->mgr;
	resmgr_iterator(res, rm) {
		if (res == item) {
			--res->refcount;
			assert(res->refcount >= 0 && "resmgr_free_item(): refcount negative, too many frees!");
			return;
		}
	}
	LOG("resmgr_free_item(): Resource sizeOf(%d) not owned "
		"by this ResManager. Critical leak.", rm->sizeOf);
}

////////////////////////////////////////////////////////////////////////////////

void res_manager_clean_unused(ResManager* rm)
{
	resmgr_iterator(res, rm) {
		if (res->hlen && res->refcount <= 0) {
			res->hlen = 0; // hlen=0 - this marks the slot as free
			rm->free(res);
			--rm->count;
		}
	}
}

void res_manager_destroy_all_items(ResManager* rm)
{
	resmgr_iterator(res, rm) {
		if (res->hlen) {
			res->hlen = 0; // hlen=0 - this marks the slot as free
			rm->free(res);
			--rm->count;
		}
	}
}
