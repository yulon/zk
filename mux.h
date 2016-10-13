#ifndef _ZK_MUX_H
#define _ZK_MUX_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ZKMux *ZKMux;
ZKMux new_ZKMux(void);
void ZKMux_lock(ZKMux);
void ZKMux_unlock(ZKMux);
void ZKMux_free(ZKMux);

#ifdef __cplusplus
}
#endif

#endif // !_ZK_MUX_H
