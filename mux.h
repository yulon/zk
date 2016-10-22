#ifndef _ZK_MUX_H
#define _ZK_MUX_H

typedef struct zk_mux *zk_mux_t;

#include <stdlib.h>

#if defined(_WIN32)
	#include <windows.h>
#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
	#include <pthread.h>
#else
	#pragma message("ZK.mux: not support this OS!")
#endif

static inline zk_mux_t zk_new_mux(void) {
	#if defined(_WIN32)
		HANDLE ntv_mux = CreateMutexW(NULL, FALSE, NULL);
		if (!ntv_mux) {
			puts("ZK.mux: Win32.CreateMutexW error!");
			exit(1);
		}
	#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
		pthread_mutex_t *ntv_mux = calloc(1, sizeof(pthread_mutex_t));
		pthread_mutex_init(ntv_mux, NULL);
		if (!ntv_mux) {
			puts("ZK.mux: POSIX.pthread_mutex_init error!");
			exit(1);
		}
	#endif
	return (zk_mux_t)ntv_mux;
}

static inline void zk_mux_lock(zk_mux_t mux) {
	#if defined(_WIN32)
		WaitForSingleObject((HANDLE)mux, INFINITE);
	#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
		pthread_mutex_lock((pthread_mutex_t *)mux);
	#endif
}

static inline void zk_mux_unlock(zk_mux_t mux) {
	#if defined(_WIN32)
		ReleaseMutex((HANDLE)mux);
	#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
		pthread_mutex_unlock((pthread_mutex_t *)mux);
	#endif
}

static inline void zk_mux_free(zk_mux_t mux) {
	#if defined(_WIN32)
		CloseHandle((HANDLE)mux);
	#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
		pthread_mutex_destroy((pthread_mutex_t *)mux);
		free(mux);
	#endif
}

#endif // !_ZK_MUX_H
