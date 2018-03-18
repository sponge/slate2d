#include <assert.h>
#include <map>

extern "C" {
#include "tmx.h"
#include "tsx.h"
#include "tmx_utils.h"
}


typedef std::map<std::string, void *> tmx_hash;

void* mk_hashtable(unsigned int initial_size) {
	return new tmx_hash();
}

void hashtable_set(void *hashtable, const char *key, void *val, hashtable_entry_deallocator deallocator) {
	assert(deallocator == NULL);
	tmx_hash &map = *((tmx_hash*)hashtable);
	map[key] = val;
}

void* hashtable_get(void *hashtable, const char *key) {
	tmx_hash map = *((tmx_hash*)hashtable);
	return map.at(key);
}

void hashtable_rm(void *hashtable, const char *key, hashtable_entry_deallocator deallocator) {
	tmx_hash map = *((tmx_hash*)hashtable);
	map.erase(key);
}

void free_hashtable(void *hashtable, hashtable_entry_deallocator deallocator) {
	tmx_hash* map = (tmx_hash*)hashtable;
	delete map;
}

void hashtable_foreach(void *hashtable, hashtable_foreach_functor functor, void *userdata) {
	if (hashtable == nullptr) {
		return;
	}

	tmx_hash* map = (tmx_hash*)hashtable;

	for (auto item = map->begin(); item != map->end(); ++item) {
		functor(item->second, userdata, item->first.c_str());
	}
}

void property_deallocator(void *val, const char *key UNUSED) {
	free_property((tmx_property*)val);
}

void tileset_deallocator(void *val, const char *key UNUSED) {
	free_ts((tmx_tileset*)val);
}
