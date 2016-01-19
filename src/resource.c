#include "resource.h"
#include "util.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////

static int init_hash(uint64_t* outHash, const char* string)
{
	int slen = strlen(string);
	*outHash = fnv64(string, slen);
	return slen;
}
#define keys_begin(resmgr) ((uint64_t*)((char*)resmgr + sizeof(ResManager)))
#define keys_at(resmgr, index) keys_begin(resmgr)[index]
#define resmgr_begin(resmgr) ((Resource*)((char*)resmgr + sizeof(ResManager) \
	+ sizeof(uint64_t)*resmgr->maxCount))
#define resmgr_at(resmgr, index) ((Resource*)((char*)resmgr + sizeof(ResManager)\
	+ sizeof(uint64_t)*resmgr->maxCount + resmgr->sizeOf*index))
#define resmgr_end(resmgr) ((Resource*)((char*)resmgr + sizeof(ResManager)\
	+ resmgr->maxCount*(sizeof(uint64_t) + resmgr->sizeOf)))
#define resmgr_next(resmgr, res, sizeOf) ((Resource*)((char*)res + sizeOf))

////////////////////////////////////////////////////////////////////////////////

ResManager* res_manager_create(const char* name, int maxCount, int sizeOf, 
	ResMgr_LoadFunc loadFunc, ResMgr_FreeFunc freeFunc)
{
	assert(sizeOf > sizeof(Resource) && 
		"resmgr_init(): sizeOf not larger than sizeof(Resource)! "
		"Items must extend struct Resource for bookkeeping");
	const int extraBytes = maxCount * (sizeof(uint64_t) + sizeOf);
	const int totalBytes = sizeof(ResManager) + extraBytes;
	ResManager* rm = malloc(totalBytes);
	if (!rm) {
		LOG("resmgr_init(): failed to allocate manager '%s' of size %d bytes\n", name, totalBytes);
		return NULL;
	}
	rm->count    = 0;
	rm->sizeOf   = sizeOf;
	rm->maxCount = maxCount;
	rm->load     = loadFunc;
	rm->free     = freeFunc;
	strncpy(rm->name, name, sizeof(rm->name));

	// default init keys:
	memset(keys_begin(rm), 0, sizeof(uint64_t) * maxCount);
	// default init values:
	Resource* r = resmgr_begin(rm);
	for (int i = 0; i < maxCount; ++i, r = resmgr_next(rm, r, sizeOf))
		r->hlen = 0; // hlen 0 means uninitialized
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

// TODO: make resource management thread safe (atomic refcounts?)
Resource* resource_load(ResManager* rm, const char* relativePath)
{
	char path[260];
	assert(strlen(relativePath) < 260-6 && "resource_load(): relativePath too long");
	uint64_t hash, fphash;
	int fphlen = 0;
	int hlen = init_hash(&hash, normalized_datapath(path, relativePath));
	if (!hlen) {
		LOG("resource_load(): invalid relativePath '%s'\n", relativePath);
		return NULL;
	}

	int count = rm->count;
	int ifree = count;
	uint64_t* keys = keys_begin(rm);
	for (int i = 0; i < count; ++i) {
		uint64_t key = keys[i];
		if (!key) {
			ifree = i; continue;
		}
		if (key != hash) continue;
		Resource* r = resmgr_at(rm, i);
		if (r->hlen != hlen) continue;

		if (!fphlen) fphlen = init_hash(&fphash, filepart(path, hlen));
		if (r->fphlen == fphlen && r->fphash == fphash) {
			++r->refcount;
			return r; // we have a pretty solid match
		}
	}
	if (count == rm->maxCount) {
		LOG("resource_load(): out of item slots! Failed to load '%s'\n", path);
		return NULL;
	}

	Resource* r = resmgr_at(rm, ifree);
	if (rm->load(r, path)) {
		keys_at(rm, ifree) = hash;
		r->refcount = 1;
		r->mgr    = rm;
		r->hlen   = hlen;
		r->fphlen = fphlen ? fphlen : init_hash(&fphash, filepart(path, hlen));
		r->fphash = fphash;
		r->path   = indebug(strdup(path)) inrelease(NULL);
		++rm->count;
		return r;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void resource_free(Resource* item)
{
	// Note: we don't actually free anything yet, because the resource may be
	// respawned during the next frame
	// res_manager_clean_unused() should be called periodically to remove
	// unused resources
	--item->refcount;
	assert(item->refcount >= 0 && "resource_free(): refcount negative, too many frees!");
}

////////////////////////////////////////////////////////////////////////////////
//// Nasty optimized code follows:

void res_manager_clean_unused(ResManager* rm)
{
	int count  = rm->count;
	int sizeOf = rm->sizeOf;
	Resource* r = resmgr_begin(rm);
	for (; count > 0; r = resmgr_next(rm, r, sizeOf)) {
		if (!r->hlen || r->refcount > 0)
			continue;
		assert(r < resmgr_end(rm) && "res_manager_clean_unused(): resource buffer overflow");
		r->hlen = 0; // hlen=0 - this marks the slot as free
		rm->free(r);
		--rm->count;
		indebug(free(r->path));
		--count;
	}
}

void res_manager_destroy_all_items(ResManager* rm)
{
	int count  = rm->count;
	int sizeOf = rm->sizeOf;
	Resource* r = resmgr_begin(rm);
	for (; count > 0; r = resmgr_next(rm, r, sizeOf)) {
		if (!r->hlen)
			continue;
		assert(r < resmgr_end(rm) && "res_manager_destroy_all_items(): resource buffer overflow");
		r->hlen = 0; // hlen=0 - this marks the slot as free
		rm->free(r);
		indebug(free(r->path));
		--count;
	}
	rm->count = 0;
}

////////////////////////////////////////////////////////////////////////////////