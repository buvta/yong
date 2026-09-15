#pragma once

#include <stdint.h>
#include <stdbool.h>

// #define BLOOM_DEBUG

#define BLOOM_CACHE_SIZE		1024
#define BLOOM_KEY_SIZE			63

typedef struct{
	void *next;
	void *data;
	uint8_t ksize;
	char key[63];
}BloomCacheItem;

typedef struct{
	uint32_t size;
#ifdef BLOOM_DEBUG
	uint32_t count;
	uint32_t conflict;
#endif

	const char *last_key;
	uint32_t last_ksize;
	unsigned last_hash;

	BloomCacheItem *cache[BLOOM_CACHE_SIZE];
	BloomCacheItem *idle;
	int cached;

	uint8_t bits[];
}BloomFilter;

BloomFilter *bloom_new(uint32_t size);
void bloom_add(BloomFilter *bf,const char *s,int len);
bool bloom_check(BloomFilter *bf,const char *s,int len,BloomCacheItem **item);
void bloom_free(BloomFilter *bf);

void bloom_cache_add(BloomFilter *bf,void *data);
void bloom_cache_clear(BloomFilter *bf);

