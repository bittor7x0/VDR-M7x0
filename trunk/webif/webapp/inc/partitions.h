/*
* 
* This source-code is licensed under GPL v2.
* See ../../LICENSE for details
* 
* Originally written for http://vdr-m7x0.foroactivo.com.es by:
* atinar <atinar1@hotmail.com>
* 
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#ifndef __PARTITIONS_H__
#define __PARTITIONS_H__

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
