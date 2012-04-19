/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_PARTITIONS_H
#define WI_PARTITIONS_H

#include "conf.h"
#include "misc.h"

typedef struct partitionInfo_s {
	int hostId;
	char * name;
	double usedMB;
	double freeMB;
	double usedPercent;
} partitionInfo_t;

typedef struct partitionList_s {
	int length;
	partitionInfo_t *partition;
} partitionList_t;

void initPartitionInfo(partitionInfo_t *const partition);
void freePartitionInfo(partitionInfo_t *const partition);
void initPartitionList(partitionList_t *const partitions);
void freePartitionList(partitionList_t *const partitions);
void getPartitionListHost(hostConf_t *const host,partitionList_t *const partitions);
void getPartitionList(partitionList_t *const partitions);
void printPartitionList(wcontext_t *wctx, const partitionList_t *const partitions);

#endif
