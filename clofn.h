#ifndef _ZK_CLOFN_H
#define _ZK_CLOFN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define ZK_CLOFN_ADDRESSING_RANGE 1024
#define _ZK_CLOFN_NUM 0x58ffffbffdffffafULL

#if defined(__unix__)
	#include <sys/mman.h>
	#include <sys/user.h>
	static inline bool _zk_clofn_active_memory(void *ptr, size_t size) {
		return mprotect((void *)(((size_t)ptr >> PAGE_SHIFT) << PAGE_SHIFT), size, PROT_READ | PROT_EXEC | PROT_WRITE) == 0;
	}
#elif defined(_WIN32)
	#include <windows.h>
	static inline bool _zk_clofn_active_memory(void *ptr, size_t size) {
		DWORD oldProtect;
		return VirtualProtect(ptr, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	}
#else
	#error ZK.clofn: not support this OS!
#endif

#define zk_clofn_def(ret_type, name, closure_type, closure_name, args, code) \
	static ret_type _zk_clofn_##name args { \
		volatile closure_type closure_name = (closure_type)_ZK_CLOFN_NUM; \
		code \
	} \
	static size_t _zk_clofn_##name##_offset = 0;

static void *_zk_clofn_mkptr(void *rawFunc, size_t *off, void *data) {
	#ifdef ZK_CLOFN_PRINT_HEADER
		printf("ZK.clofn: raw header (%08X) { ", rawFunc);
	#endif
	for (size_t offset = *off; offset < ZK_CLOFN_ADDRESSING_RANGE; offset++) {
		if (*(size_t *)((uintptr_t)rawFunc + offset) == (size_t)_ZK_CLOFN_NUM) {
			if (!*off) {
				*off = offset;
			}

			#ifdef ZK_CLOFN_PRINT_HEADER
				printf("} @%u+%u\n", offset, sizeof(uintptr_t));
			#endif

			#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64) || defined(__amd64__) || defined(_WIN64)
				size_t bxFuncSize = offset + sizeof(void *) * 2 + 5;
			#elif defined(i386) || defined(__i386__) || defined(_X86_) || defined(__i386) || defined(__i686__) || defined(__i686) || defined(_WIN32)
				size_t bxFuncSize = offset + sizeof(void *) * 2 + 1;
			#else
				#error ZK.clofn: not support this arch!
			#endif

			void *bxFunc = malloc(bxFuncSize);
			if (!_zk_clofn_active_memory(bxFunc, bxFuncSize)) {
				puts("ZK.clofn: could't change memory type of C.malloc allocated!");
				free(bxFunc);
				return NULL;
			}
			memcpy(bxFunc, rawFunc, offset);
			uintptr_t addr = (uintptr_t)bxFunc + offset;
			*(void **)addr = data;
			addr += sizeof(void *);

			#if defined(__x86_64__)  || defined(__x86_64)  || defined(__amd64)  || defined(__amd64__) || defined(_WIN64)
				*(uint8_t *)addr = 0x50;
				addr++;
				*(uint8_t *)addr = 0x48;
				addr++;
				*(uint8_t *)addr = 0xB8;
				addr++;
				*(uintptr_t *)addr = (uintptr_t)rawFunc + offset + sizeof(uintptr_t) - 1; // 0x58 in _ZK_CLOFN_NUM
				addr += sizeof(uintptr_t);
				*(uint16_t *)addr = 0xE0FF;
			#elif defined(i386) || defined(__i386__) || defined(_X86_) || defined(__i386) || defined(__i686__) || defined(__i686) || defined(_WIN32)
				*(uint8_t *)addr = 0xE9;
				addr++;
				*(uintptr_t *)addr = ((uintptr_t)rawFunc + offset + sizeof(uintptr_t)) - ((uintptr_t)bxFunc + bxFuncSize);
			#endif

			#ifdef ZK_CLOFN_PRINT_HEADER
				printf("ZK.clofn: new header (%08X) { ", bxFunc);
				for (size_t i = 0; i < bxFuncSize; i++) {
					printf("%02X ", *(uint8_t *)(bxFunc + i));
				}
				printf("}\n");
			#endif

			return bxFunc;
		}
		#ifdef ZK_CLOFN_PRINT_HEADER
			else printf("%02X ", *(uint8_t *)(rawFunc + offset));
		#endif
	}
	#ifdef ZK_CLOFN_PRINT_HEADER
		printf("...\n");
	#endif

	printf("ZK.clofn: could't find declarations at raw function (%08X)!\n", rawFunc);
	return NULL;
}

#define zk_clofn_mkptr(name, closure_val) _zk_clofn_mkptr(_zk_clofn_##name, &_zk_clofn_##name##_offset, (void *)closure_val)

#ifdef __cplusplus
}
#endif

#endif // !_ZK_CLOFN_H
