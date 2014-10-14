#ifndef _RECORDINFO_H
#define _RECORDINFO_H
/*
#include "Phase2/Record.h"
#include "Phase2/Value.h"
#include "Phase2/Schema.h" 
#include "Phase2/Attribute.h"
#include "Phase2/Catalog.h"
#include "LRU.h"
*/
typedef struct RecInfo {
	char *rec;
	int position; //refers to row in the Run vector
}RecordInfo;

#endif
