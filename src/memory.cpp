#include "memory.h"

static Block * RC_create_block(size_t element_size){
	Block * block = static_cast<Block *>(malloc(sizeof(Block)));
	assert(block != NULL);
	block->elem_cap = DEFAULT_BLOCK_CAP;
	block->count = 0;
	block->next = NULL;

	size_t size = element_size * block->elem_cap;
	block->buffer = static_cast<uint8_t *>(malloc(sizeof(uint8_t) * size));
	assert(block != NULL);

	memset(block->buffer, 0, sizeof(uint8_t) * size);

	return block;
}

static void RC_destroy_block(Block * block){
	if(block == NULL) return;
	free(block->buffer);
	free(block);
}

RC_MemPool * RC_create_mempool(size_t cap, size_t element_size){
	RC_MemPool * pool = static_cast<RC_MemPool *>(malloc(sizeof(RC_MemPool)));

	assert(pool != NULL);

	pool->cap = MAX(START_BLOCKS_CAP, 1);
	pool->element_size = MAX(element_size, sizeof(struct FreeAddr));
	pool->start = pool->end = NULL;
	pool->free_addrs = NULL;

	// insert cap blocks
	for(size_t i = 0; i < pool->cap; i++){
		Block * b = RC_create_block(pool->element_size);

		if(pool->start == NULL && pool->end == NULL){
			pool->start = pool->end = b;
			continue;
		}

		pool->end->next = b;
		pool->end = b;
	}

	pool->curr_block = pool->start;

	return pool;
}

void * RC_mempool_alloc(RC_MemPool * pool){
	assert(pool != NULL);
	void * mem;

	if(pool->free_addrs != NULL){
		mem = (void *)pool->free_addrs;
		pool->free_addrs = pool->free_addrs->next;
		return mem;
	}

	if(pool->curr_block->count + 1 >= pool->curr_block->elem_cap){
		pool->curr_block = pool->curr_block->next;
		if(pool->curr_block == NULL){ // ran out of blocks
			size_t new_cap = pool->cap * 2;

			// insert new_cap - old_cap new blocks
			for(size_t i = pool->cap; i < new_cap; i++){
				Block * new_block = RC_create_block(pool->element_size);

				if(i == pool->cap){
					pool->curr_block = new_block;
				}

				pool->end->next = new_block;
				pool->end = new_block;

			}
			pool->cap = new_cap;
		}
	}

	assert(pool->curr_block != NULL);
	Block * block = pool->curr_block;

	size_t offset = block->count * pool->element_size;
	mem = (void *)(block->buffer + offset);
	block->count++;
	return mem;
}

void RC_mempool_free(RC_MemPool * pool, void ** addr){
	assert(pool != NULL);
	assert(addr != NULL && *addr != NULL);

	FreeAddr * tmp = pool->free_addrs;
	pool->free_addrs = (FreeAddr *)(*addr);
	pool->free_addrs->next = tmp;
	*addr = NULL;
}

void RC_destroy_mempool(RC_MemPool * pool){
	Block * curr, * prev;

	curr = pool->start;
	while(curr != NULL){
		prev = curr;
		curr = curr->next;
		RC_destroy_block(prev);
	}

	free(pool);
}
