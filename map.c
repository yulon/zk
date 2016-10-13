#include "map.h"
#include <stdlib.h>
#include <string.h>

typedef struct ZKPair {
	void *key;
	void *value;
} *ZKPairList;

struct ZKMap {
	size_t base;
	size_t size;
	ZKPairList list;
};

ZKMap new_ZKMap(size_t baseSize) {
	ZKMap map = calloc(1, sizeof(struct ZKMap));
	map->base = baseSize;
	map->size = baseSize;
	map->list = calloc(baseSize, sizeof(struct ZKPair));
	return map;
}

bool ZKMap_resize(ZKMap map, size_t size) {
	if (size <= map->base) {
		return false;
	}
	ZKPairList newList = calloc(size, sizeof(struct ZKPair));
	ZKPairList oldList = map->list;
	memcpy(newList, oldList, map->size);
	map->list = newList;
	map->size = size;
	free(oldList);
	return true;
}

bool ZKMap_set(ZKMap map, void *key, void *value) {
	for (size_t dvdnd = map->base; dvdnd <= map->size; dvdnd += map->base) {
		size_t ix = (size_t)key % dvdnd;
		if (!map->list[ix].key) {
			map->list[ix].key = key;
			map->list[ix].value = value;
			return true;
		}
	}
	for (size_t size = map->size * 2; size <= SIZE_MAX; size = size * 2) {
		for (size_t dvdnd = map->size + map->base; dvdnd <= size; dvdnd += map->base) {
			size_t ix = (size_t)key % dvdnd;
			if (ix < map->size) {
				if (!map->list[ix].key) {
					map->list[ix].key = key;
					map->list[ix].value = value;
					ZKMap_resize(map, size);
					return true;
				}
			} else {
				ZKMap_resize(map, size);
				map->list[ix].key = key;
				map->list[ix].value = value;
				return true;
			}
		}
	}
	return false;
}

void *ZKMap_get(ZKMap map, void *key) {
	for (size_t dvdnd = map->base; dvdnd <= map->size; dvdnd += map->base) {
		size_t ix = (size_t)key % dvdnd;
		if (map->list[ix].key == key) {
			return map->list[ix].value;
		}
	}
	return NULL;
}

void ZKMap_delete(ZKMap map, void *key) {
	for (size_t dvdnd = map->base; dvdnd <= map->size; dvdnd += map->base) {
		size_t ix = (size_t)key % dvdnd;
		if (map->list[ix].key == key) {
			map->list[ix].key = NULL;
			map->list[ix].value = NULL;
		}
	}
}

void ZKMap_free(ZKMap map) {
	free(map->list);
	free(map);
}
