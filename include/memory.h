#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rc_math.h"

#define START_BLOCKS_CAP 1
#define DEFAULT_BLOCK_CAP 128

typedef struct FreeAddr{
	struct FreeAddr * next;
}FreeAddr;

typedef struct Block{
	uint8_t * buffer;
	size_t elem_cap;
	size_t count;
	struct Block * next;
}Block;

typedef struct{
	Block * start; // linked list of blocks
	Block * end;
	size_t cap; // how many blocks
	size_t element_size;
	Block * curr_block;
	FreeAddr * free_addrs;
}RC_MemPool;

RC_MemPool * RC_create_mempool(size_t cap, size_t element_size);
void * RC_mempool_alloc(RC_MemPool * pool);
void RC_mempool_free(RC_MemPool * pool, void ** addr);
void RC_destroy_mempool(RC_MemPool * pool);
