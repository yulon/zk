#ifndef _ZK_MAP_H
#define _ZK_MAP_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ZKMap *ZKMap;
ZKMap new_ZKMap(size_t);
bool ZKMap_resize(ZKMap, size_t);
bool ZKMap_set(ZKMap, void *key, void *value);
void *ZKMap_get(ZKMap, void *key);
void ZKMap_delete(ZKMap, void *key);
void ZKMap_free(ZKMap);

#ifdef __cplusplus
}
#endif

#endif // !_ZK_MAP_H
