/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <mntent.h>
#include <sys/vfs.h>
#include <klone/utils.h>

#include "i18n.h"
#include "partitions.h"

void initPartitionInfo(partitionInfo_t * const partition) {
    partition->name = NULL;
    partition->usedMB = 0;
    partition->freeMB = 0;
    partition->usedPercent = 0;
}

void freePartitionInfo(partitionInfo_t * const partition) {
    free(partition->name);
    initPartitionInfo(partition);
}

void initPartitionList(partitionList_t * const partitions) {
    partitions->length = 0;
    partitions->partition = NULL;
}

void freePartitionList(partitionList_t * const partitions) {
    int i;
    partitionInfo_t *partition;
    for (i = 0, partition = partitions->partition; i < partitions->length; i++, partition++) freePartitionInfo(partition);
    free(partitions->partition);
    initPartitionList(partitions);
}

void getPartitionList/*Host*/(/*hostConf_t *const host,*/partitionList_t * const partitions) {
    initPartitionList(partitions);
    FILE *mountTable;
    struct mntent *mountEntry;
    mountTable = setmntent("/etc/mtab", "r");
    //TODO deducir las particiones a partir de host->video0
    partitionInfo_t *partition;
    while ((mountEntry = getmntent(mountTable))) {
        if (strncmp(mountEntry->mnt_dir, "/var/media/", 11) == 0) {
            struct statfs s;
            statfs(mountEntry->mnt_dir, &s);
            if (s.f_blocks > 0) {
                partitions->partition = realloc(partitions->partition, (++partitions->length) * sizeof (partitionInfo_t));
                crit_goto_if(partitions->partition == NULL, outOfMemory);
                partition = partitions->partition + partitions->length - 1;
                //partition->hostId=host->id;
                crit_goto_if((partition->name = strdup(mountEntry->mnt_dir)) == NULL, outOfMemory);
                double blocksUsed = s.f_blocks - s.f_bfree;
                partition->usedPercent = (blocksUsed / s.f_blocks)*100.0;
                partition->usedMB = (blocksUsed * s.f_bsize) / 1048576.0;
                partition->freeMB = ((double) s.f_bfree * s.f_bsize) / 1048576.0;
            }
        }
    }
    endmntent(mountTable);
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void printPartitionList(wcontext_t *wctx, const partitionList_t * const partitions) {
    if (partitions->length) {
        WCTX_IPRINTFT("<table id=\"partitionSpace\" class=\"list\">\n", 0, 1);
        WCTX_IPRINTF("<caption>%s</caption>\n", tr("partitionSpace"));
        WCTX_IPRINTT("<thead class=\"listHead\">\n", 0, 1);
        WCTX_IPRINTF(
                "<tr>"
                "<th>%s</th>"
                "<th colspan=\"2\">%s</th>"
                "<th>%s</th>"
                "</tr>\n", tr("partition"), tr("used"), tr("available"));
        WCTX_IPRINTT("</thead>\n", -1, 0);
        int i;
        partitionInfo_t *partition;
        WCTX_IPRINTT("<tbody class=\"listBody\">\n", 0, 1);
        for (i = 0, partition = partitions->partition; i < partitions->length; i++, partition++) {
            WCTX_IPRINTF("<tr><td>%s</td><td>%.0f MB</td><td>%.0f %%</td><td>%.0f MB</td></tr>\n"
                    , partition->name, partition->usedMB, partition->usedPercent, partition->freeMB
                    );
        }
        WCTX_IPRINTT("</tbody>\n", -1, 0);
        WCTX_IPRINTT("</table>\n", -1, 0);
    } else {
        WCTX_MESSAGE(MT_ALERT, NULL, tr("noMediaErr"));
    }
}

