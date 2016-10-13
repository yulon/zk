#include "mux.h"
#include <stdlib.h>

#if defined(_WIN32)
	#include <windows.h>
#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
	#include <pthread.h>
#else
	#pragma message("ZKMux: not support this OS!")
#endif

ZKMux new_ZKMux(void) {
	#if defined(_WIN32)
		return (ZKMux)CreateMutexW(NULL, FALSE, NULL);
	#elif defined(__unix__)
		pthread_mutex_t *ptMux = calloc(1, sizeof(pthread_mutex_t));
		pthread_mutex_init(ptMux, NULL);
		return (ZKMux)ptMux;
	#else
		return NULL;
	#endif
}

void ZKMux_lock(ZKMux mux) {
	if (mux) {
		#if defined(_WIN32)
			WaitForSingleObject((HANDLE)mux, INFINITE);
		#elif defined(__unix__)
			pthread_mutex_lock((pthread_mutex_t *)mux);
		#endif
	}
}

void ZKMux_unlock(ZKMux mux) {
	if (mux) {
		#if defined(_WIN32)
			ReleaseMutex((HANDLE)mux);
		#elif defined(__unix__)
			pthread_mutex_unlock((pthread_mutex_t *)mux);
		#endif
	}
}

void ZKMux_free(ZKMux mux) {
	if (mux) {
		#if defined(_WIN32)
			CloseHandle((HANDLE)mux);
		#elif defined(__unix__)
			pthread_mutex_destroy((pthread_mutex_t *)mux);
			free(mux);
		#endif
	}
}
