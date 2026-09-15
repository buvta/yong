#pragma once

#include <stdint.h>
#include "pinyin.h"
#include "mb.h"

enum{
	FUZZY_DEFAULT,
	FUZZY_FORCE,			// 目标不存在时，是否强制添加
};

#define FUZZY_TO_MAX		4

typedef struct{
	char code[8];				// 目标编码
	uint8_t len;
	bool valid;
}FUZZY_TO;

typedef struct{
	void *next;
	char from[8];				// 要模糊的编码
	FUZZY_TO to[FUZZY_TO_MAX];	// 模糊目标
	void *begin;				// 要模糊的字列表的开始
	void *end;					// 要模糊的字列表的结束
}FUZZY_ITEM;

typedef LHashTable FUZZY_TABLE;

FUZZY_TABLE *fuzzy_table_load(const char *file,struct y_mb *mb);
void fuzzy_table_free(FUZZY_TABLE *ft);
FUZZY_ITEM *fuzzy_table_lookup(FUZZY_TABLE *ft,const char *code);
int fuzzy_key_list_py(FUZZY_TABLE *ft,py_item_t *input,int count,LPtrArray *list);
int fuzzy_key_list_simple(FUZZY_TABLE *ft,const char *code,LPtrArray *list);
int fuzzy_key_list2(FUZZY_TABLE *ft,const char *code,int split,LPtrArray *list);
void fuzzy_table_dump(FUZZY_TABLE *ft);

