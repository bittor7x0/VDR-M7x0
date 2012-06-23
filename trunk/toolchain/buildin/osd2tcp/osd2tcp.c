/***************************************************************************
 *       Copyright (c) 2005 by Andreas Koch <m740av@karlmarx.ping.de>      *
 *                                                                         *
 *   Parts are taken and adapted from stdin2tcp written by Dirk Clemens.   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/fb.h>
#include <signal.h>

///////////////////////////////////////////////////////////////////////////////
#define min(a,b) (a<=b?a:b)
#define max(a,b) (a>=b?a:b)

#define WIDTH 720
#define FIELDHEIGHT 288
#define HEIGHT 576
#define ZLIBOVERHEAD 165900
#define ROWDATALENGTH WIDTH
#define SNDFIELD ROWDATALENGTH*FIELDHEIGHT

#define NAME "osd2tcp"
#define TITLE NAME " V 1.0.0 - Andreas Koch - 2006-11-04"

sig_atomic_t sigtermrecv=0;

void sighandler(int signu) {
  sigtermrecv=1;
}

// Osd Block - Struktur
struct block_s {
    int changed;
    int clear;
    uint32_t adler;
    uint8_t *raw_buf;
    uint32_t raw_length;
    const uint32_t **rowPointer;
};

// -- Bits/Pixel reduzieren

int generate_8bpp_block_buf(struct block_s *block,const int block_width, 
        const int block_height, const int *check_pixel_in_row, 
        const int check_pixel_per_row, const int *check_rows, 
        const int check_rows_count, const uint32_t clear_block_adler, 
        const int compression_level)
{
    int x,y;
    const uint32_t *pixSrc;
    uint32_t aktPixel;
    int rleCount=127;
    uint8_t lastPixel=0;
    uint8_t temp_p;
    uint8_t *fb_8bpp=block->raw_buf;
    uint32_t new_adler=adler32(0,Z_NULL,0);
    int check_r,check_p;
    
    block->raw_length=compression_level?0:block_height*block_width;
    for (y=0,check_r=0; y < block_height; y++) 
    {   
        pixSrc = block->rowPointer[y];
        for (x=0,check_p=0; x < block_width; x++,pixSrc++) 
        {
            aktPixel = *pixSrc;
            if ( (aktPixel & 0xff000000) == 0) 
            {
                aktPixel=0;
                temp_p=0;
            } else {
                temp_p= 0xc0 |
                   ((aktPixel>>18)&0x30) |
                   ((aktPixel>>12)&0xc) |
                   ((aktPixel>>6)&0x3);
            }
            if ( check_p < check_pixel_per_row && 
                    check_r < check_rows_count && check_rows[check_r]==y &&
                    check_pixel_in_row[check_p]==x)
            { 
                new_adler=adler32(new_adler,(Bytef *) &aktPixel,4);
                check_p++;
            }
            if (compression_level) 
            {
                if ( lastPixel == temp_p && rleCount!=127) 
                {
                    rleCount++;
                } else {
                    if ( x != 0 || y != 0){
                        if (lastPixel != 0 ){
                            *fb_8bpp= (uint8_t) rleCount|0x80;
                            fb_8bpp++;
                            *fb_8bpp=lastPixel;
                            fb_8bpp++;
                            block->raw_length+=2;
                        } else {
                            *fb_8bpp= (uint8_t) rleCount;
                            fb_8bpp++;
                            block->raw_length++;
                        }
                    }
                    lastPixel=temp_p;
                    rleCount=0;
                }
            } else {
                *fb_8bpp=temp_p;
                fb_8bpp++;
            }
        }
        
        if (check_r < check_rows_count && check_rows[check_r]==y)
            check_r++;
    }
    if (compression_level){
        if (lastPixel != 0 ) {
            *fb_8bpp= (uint8_t) rleCount | 0x80;
            *(fb_8bpp+1)=lastPixel;
            block->raw_length+=2;
        } else {
            *fb_8bpp= (uint8_t) rleCount;
            block->raw_length++;
        }
    }
    if (block->adler!=new_adler)
    {
       block->adler=new_adler;
       block->clear=new_adler==clear_block_adler;
       return 1;
    }   
    
    return 0;
}


int generate_16bpp_block_buf(struct block_s *block,const int block_width,
        const int block_height, const int *check_pixel_in_row, 
        const int check_pixel_per_row, const int *check_rows, 
        const int check_rows_count, const uint32_t clear_block_adler,
        const int compression_level)
{
    int x,y;
    const uint32_t *pixSrc;
    uint16_t temp_p,lastPixel=0;
    uint16_t *fb_16bpp=(uint16_t *) block->raw_buf;
    uint8_t *fb_8b=(uint8_t *) block->raw_buf;
    uint32_t new_adler=adler32(0,Z_NULL,0);
    int check_r,check_p;
    uint32_t aktPixel;
    int rleCount=127;
    
    block->raw_length=compression_level?0:(block_height*block_width)<<1;
    for (y=0,check_r=0; y < block_height; y++) 
    {
        pixSrc= block->rowPointer[y];
        for (x=0,check_p=0; x < block_width; x++,pixSrc++) 
        {
            aktPixel=*pixSrc; 
            if ( ( aktPixel & 0xff000000 ) == 0) 
            {
                temp_p=0;
                aktPixel=0;
            } else  {
                temp_p=0x8000 |
                   ((aktPixel>>9)&0x7c00) |
                   ((aktPixel>>6)&0x3E0) |
                   ((aktPixel>>3)&0x1f);
            }
            if ( check_p < check_pixel_per_row && 
                    check_r < check_rows_count && check_rows[check_r]==y &&
                    check_pixel_in_row[check_p]==x)
            { 
                new_adler=adler32(new_adler,(Bytef *) &aktPixel,4);
                check_p++;
            }
            
            if (compression_level) 
            {
                if ( lastPixel == temp_p && rleCount!=127) 
                {
                    rleCount++;
                } else {
                    if ( x != 0 || y != 0){
                        if (lastPixel != 0 ){
                            *fb_8b= (uint8_t) rleCount | 0x80;
                            fb_8b++;
                            *((uint16_t *)fb_8b)=lastPixel;
                            fb_8b+=2;
                            block->raw_length+=3;
                        } else {
                            *fb_8b= (uint8_t) rleCount;
                            fb_8b++;
                            block->raw_length++;
                        }
                    }
                    lastPixel=temp_p;
                    rleCount=0;
                }
            } else {
                *fb_16bpp=temp_p;
                fb_16bpp++;
            }
        }
        if (check_r < check_rows_count && check_rows[check_r]==y)
            check_r++;
    }
    if (compression_level) 
    {
        if ( lastPixel != 0 ) {
            *fb_8b= (uint8_t) rleCount | 0x80;
            *((uint16_t *)(fb_8b+1))=lastPixel;
            block->raw_length+=3;
        } else {
            *fb_8b= (uint8_t) rleCount;
            block->raw_length++;
        }
    }
    if (block->adler!=new_adler)
    {
        block->adler=new_adler;
        block->clear=new_adler==clear_block_adler;
        return 1;
    }
    return 0;
}

int generate_32bpp_block_buf(struct block_s *block,const int block_width,
        const int block_height, const int *check_pixel_in_row, 
        const int check_pixel_per_row, const int *check_rows, 
        const int check_rows_count, const uint32_t clear_block_adler,
        const int compression_level )
{
    int x,y;
    const uint32_t *pixSrc;
    uint32_t aktPixel,temp_p,lastPixel=0;
    uint32_t *fb_32bpp=(uint32_t *) block->raw_buf;
    uint8_t *fb_8b=(uint8_t *) block->raw_buf;
    uint32_t new_adler=adler32(0,Z_NULL,0);
    int check_r,check_p;
    int rleCount=127;
    
    block->raw_length=compression_level?0:(block_height*block_width)<<2;
    for (y=0,check_r=0; y < block_height; y++) 
    {
        pixSrc= block->rowPointer[y];
        for (x=0,check_p=0; x < block_width; x++,pixSrc++) 
        {
            temp_p=aktPixel = *pixSrc;
            if ( (aktPixel & 0xff000000) == 0) 
                temp_p=aktPixel=0;
            else 
                temp_p|=0xff000000;
            
            if ( check_p < check_pixel_per_row && 
                    check_r < check_rows_count && check_rows[check_r]==y &&
                    check_pixel_in_row[check_p]==x)
            { 
                new_adler=adler32(new_adler,(Bytef *) &aktPixel,4);
                check_p++;
            }
            if (compression_level) 
            {
                if ( lastPixel == temp_p && rleCount!=127) 
                {
                    rleCount++;
                } else {
                    if ( x != 0 || y != 0){
                        if ( lastPixel!=0 ) {
                            *((uint32_t *)fb_8b)=((rleCount | 0x80)<<24) |
                                 (lastPixel & 0xffffff);
                            fb_8b+=4;
                            block->raw_length+=4;
                        } else {
                            *fb_8b= (uint8_t) rleCount;
                            fb_8b++;
                            block->raw_length++;
                        }
                    }
                    lastPixel=temp_p;
                    rleCount=0;
                }
            } else {
                *fb_32bpp=temp_p;
                fb_32bpp++;
            }
        }
        if (check_r < check_rows_count && check_rows[check_r]==y)
            check_r++;
    }
    if (compression_level) 
    {
        if ( lastPixel!=0 ) {
            *((uint32_t *)fb_8b)=((rleCount | 0x80)<<24) |
                (lastPixel & 0xffffff);
            block->raw_length+=4;
        } else {
            *fb_8b= (uint8_t) rleCount;
            fb_8b++;
            block->raw_length++;
        }
    }
    if (block->adler!=new_adler)
    {
        block->adler=new_adler;
        block->clear=new_adler==clear_block_adler;
        return 1;
    }
    return 0;
}



///////////////////////////////////////////////////////////////////////////////

#define MAX_CLIENTS 10

int client[MAX_CLIENTS];
int n_clients = 0;
int* client_blocks_changed[MAX_CLIENTS];
int client_blocks_changed_count[MAX_CLIENTS];
///////////////////////////////////////////////////////////////////////////////

void accept_client(const int main_socket, const int block_count, const int verbose)
{
    static char title[] = TITLE "\r\n";
    static char errmsg[] = "*** ERROR: TO MUCH CLIENTS ***\r\n";

    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);
    int status = accept(main_socket,(struct sockaddr*)&sa,&len);

    if ( status != -1 )
    {
	send(status,title,sizeof(title)-1,MSG_DONTWAIT|MSG_NOSIGNAL);

	int i;
	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
	    if ( client[i] == -1 )
	    {
		n_clients++;
		if (verbose)
		    fprintf(stderr,"ACCEPTED: index=%d, handle=%d  [%u client%s now]\n",
				i, status, n_clients, n_clients==1 ? "" : "s");
		client[i] = status;
                int j;
	        for (j=0; j< block_count; j++)
                    (client_blocks_changed[i])[j]=1;
                client_blocks_changed_count[i]=block_count;    
		return;
	    }
	}

	// keinen freien Platz gefunden
	send(status,errmsg,sizeof(errmsg)-1,MSG_DONTWAIT|MSG_NOSIGNAL);
	if (verbose)
	    fprintf(stderr,"TO MUCH CLIENTS (max=%d)\n",MAX_CLIENTS);
	close(status);
    }
}

///////////////////////////////////////////////////////////////////////////////

void close_client ( const int index, const int verbose)
{
    if ( index >= 0 && index < MAX_CLIENTS && client[index] != -1 )
    {
	n_clients--;
	if (verbose)
	    fprintf(stderr,"CLOSE CLIENT: index=%d, handle=%d [%u client%s now]\n",
		index, client[index], n_clients, n_clients==1 ? "" : "s" );
	close(client[index]);

	client[index] = -1;
    }
}

///////////////////////////////////////////////////////////////////////////////

#define RECV_UNKNOWN -1
#define RECV_AGAIN -2
#define RECV_CLOSE -3
#define RECV_GET_CONFIG 1
#define RECV_WAKEUP 2
#define RECV_GET_BLOCK 3
#define RECV_GET_BLOCKS_MODIFIED 4
#define RECV_GET_WHICH_BLOCKS_MODIFIED 5
#define RECV_GET_VERSION 6

int receive_data ( const int index, const int flags, const int verbose, const int block_count, int *block_nr)
{
    char buf[10000];
    int status = recv(client[index],buf,sizeof(buf),flags==0?MSG_PEEK:flags);
    int err = errno;
    int used=status;
    int ret=RECV_UNKNOWN;
    buf[status]=0;
    if (verbose)
        fprintf(stderr,"RECEIVE: index=%d, handle=%d, flags=%x -> status=%d, errno=%d \n",
	   	index, client[index], flags, status, status<0 ? err : 0 );
    if (  buf[0] == 4 && ( status == 1
			|| ( status >= 2 && ( buf[1] == '\n' || buf[1] == '\r' ) ) ) )
    {
	if (verbose)
	    fprintf(stderr,"CTRL-D RECEIVED -> CLOSE CLIENT\n");
	close_client(index,verbose);
	return RECV_CLOSE;
    }
    
    if ( status == -1 )
    {
	if ( err != EWOULDBLOCK && err != EAGAIN ) 
	{
	    close_client(index,verbose);
	    return RECV_CLOSE;
	}
	return RECV_AGAIN;    
    }
    
    if ( status == 0 ) 
    {
	close_client(index,verbose);
	return RECV_CLOSE;
    }
    
    if (ret==RECV_UNKNOWN && status >= 11 && (strncmp(buf,"GET_CONFIG",10) == 0) &&
            (buf[10] == '\r' || buf[10] == '\n' ) ){
        used=11;    
        ret=RECV_GET_CONFIG;
        }
    
    if (ret==RECV_UNKNOWN && status >= 7  && (strncmp(buf,"WAKEUP",6) == 0) &&
            (buf[6] == '\r' || buf[6] == '\n' ) ){
        used=7;    
        ret=RECV_WAKEUP;    
        }
        
    if (ret==RECV_UNKNOWN && status >= 24  && (strncmp(buf,"GET_OSD_BLOCKS MODIFIED",23) == 0) &&
            (buf[23] == '\r' || buf[23] == '\n' ) ){
        used=24;    
        ret=RECV_GET_BLOCKS_MODIFIED;        
        }
    if (ret==RECV_UNKNOWN && status >= 30  && (strncmp(buf,"GET_WHICH_OSD_BLOCKS_MODIFIED",29) == 0) &&
            (buf[29] == '\r' || buf[29] == '\n' ) ) {
        used=30;    
        ret= RECV_GET_WHICH_BLOCKS_MODIFIED;
        }
        
    if (ret==RECV_UNKNOWN && status >= 12  && (strncmp(buf,"GET_VERSION",11) == 0) &&
            (buf[11] == '\r' || buf[11] == '\n' ) ) {
        used=12;    
        ret=RECV_GET_VERSION;    
        }
        
    if (ret==RECV_UNKNOWN && ( sscanf(buf,"GET_OSD_BLOCK %d %*[\r\n]",block_nr) == 1 ) 
            && ( *block_nr >=0 ) && ( *block_nr < block_count) ) {
        for (used=0; buf[used]!='\n' && buf[used]!='\r' ;used++)
          ;
        used++;  
        ret=RECV_GET_BLOCK;
        }
        
    if (used>0 && buf[used-1]!='\n')
       used++;
       
    if (flags==0)   
       recv(client[index],buf,used,0);
    return ret;
}

///////////////////////////////////////////////////////////////////

int send_msg (const char *msg, const size_t msg_len, const int index, const int flags, const int verbose) 
{
    if ( index >= 0 && index < MAX_CLIENTS && client[index] != -1 )
    {
        int status=send(client[index],msg,msg_len,flags);
        int err = errno;
        if ( verbose ) 
            fprintf (stderr,"SEND: msg_len=%d, index=%d, handle=%d, flags=%d -> status=%d, errno=%d\n",
                     msg_len,index, client[index], flags, status, status<0 ? err : 0 );
                    
        if ( status == -1 )
        {
	    if ( err != EWOULDBLOCK && err != EAGAIN ){
                close_client(index,verbose);
                return msg_len;
            }
            return 0;    
        }
        return status;
    }
    return msg_len;
}


void send_config (const int block_width, const int block_height, 
        const int bpp, const int index, const int verbose)
{

    char buf[300];
    int len;    
    len=snprintf(buf,300,"OSD_BLOCK_CONFIG %d %d %d\r\n",
            block_width, block_height, bpp);
    send_msg(buf,len,index,MSG_NOSIGNAL,verbose);
}

void send_version (const int index,
        const int verbose) {
    send_msg("OSD2TCP_VERSION 0 0 4\r\n",23,index,MSG_NOSIGNAL,verbose);    
}        
        

void send_modified (const int changed_count, const int verbose)
{

    char buf[300];
    int len,i;    
    for (i=0 ; i < MAX_CLIENTS ; i++) {
        if (client[i]==-1 || client_blocks_changed_count[i]==0)
            continue;
        len=snprintf(buf,300,"OSD_BLOCKS_MODIFIED %d %d\r\n", changed_count,client_blocks_changed_count[i]);
        send_msg(buf,len,i,MSG_NOSIGNAL,verbose);      
    }
}

void send_which_blocks_modified (const int index, const int block_count, const int verbose) 
{
    char buf[300];
    int len,i;    
    for (i=0 ; i < block_count ; i++) 
    {
        if (! (client_blocks_changed[index])[i] )
           continue;
        len=snprintf(buf,300,"BLOCK_MODIFIED %d\r\n",i);
        send_msg(buf,len,index,MSG_NOSIGNAL,verbose);
    }
}

        
void send_osd_block_clear (const int block_nr, const int index,
        const int verbose)
{
    char buf[100];
    int len=snprintf(buf,100,"OSD_BLOCK_CLEAR %d\r\n",block_nr);

    send_msg(buf,len,index,MSG_NOSIGNAL,verbose);
    
}

void send_osd_block_raw (const struct block_s *block, const int block_nr,
        const int index, const int verbose , const int compression_level)
{
    char buf[100];
    int len=snprintf(buf,100,"OSD_BLOCK_%s %d %u\r\n",
            compression_level?"RLE":"RAW", block_nr,
            block->raw_length);

    send_msg(buf,len,index,MSG_NOSIGNAL,verbose);
    send_msg(block->raw_buf,block->raw_length,index,MSG_NOSIGNAL,verbose);
}


int send_block ( struct block_s *block, const int block_nr,
        const int block_width, const int block_height, const int bpp, 
        const int compression_level, const int index, const int verbose, 
        const int *check_pixel_in_row, const int check_pixel_per_row, 
        const int *check_rows, const int check_rows_count, 
        const uint32_t clear_block_adler)
{
    int ret=0,i;
    if ( (client_blocks_changed[index])[block_nr] ) 
    {
        (client_blocks_changed[index])[block_nr]=0;
        client_blocks_changed_count[index]--;
    }
  		        
    if ( block->clear ) 
    {
        send_osd_block_clear (block_nr,index,verbose);
        block->changed=0;
        return 0;
    }
                       
    if ( block->changed )
    {
        switch (bpp) 
        {
            case 8:
                ret=generate_8bpp_block_buf(block, block_width, block_height,
                        check_pixel_in_row, check_pixel_per_row,
                        check_rows, check_rows_count, clear_block_adler, 
                        compression_level);
                break;
            case 16:
                ret=generate_16bpp_block_buf(block, block_width, block_height,
                        check_pixel_in_row, check_pixel_per_row,
                        check_rows, check_rows_count, clear_block_adler,
                        compression_level);
                break;
            case 32: 
                ret=generate_32bpp_block_buf(block, block_width, block_height,
                        check_pixel_in_row, check_pixel_per_row,
                        check_rows, check_rows_count, clear_block_adler,
                        compression_level);
                break;   
        }
                            
        block->changed=0;             
    }
    
    if (ret)
        for (i=0; i < MAX_CLIENTS ; i++)
        {
            if (client[i]==-1 || i==index)
                continue;
            if (! (client_blocks_changed[i])[block_nr] ) 
            {
                (client_blocks_changed[i])[block_nr]=1;
                client_blocks_changed_count[i]++;
            }    
        }
                        
    send_osd_block_raw (block, block_nr, index, verbose,compression_level);
    return ret;
}        


int send_modified_blocks (struct block_s *blocks, const int block_count,
        const int block_width, const int block_height, const int bpp, 
        const int compresstion_level, const int index, const int verbose, 
        const int *check_pixel_in_row, const int check_pixel_per_row,
        const int *check_rows, const int check_rows_count,
        const uint32_t clear_block_adler)
{
    int i;
    int changed_count=0;    
    for (i=0 ; i < block_count ; i++) 
    {
        if (! (client_blocks_changed[index])[i] )
           continue;
        
        
        changed_count+=send_block (&blocks[i], i, block_width, block_height, bpp, 
                compresstion_level, index, verbose,
                check_pixel_in_row, check_pixel_per_row,
                check_rows, check_rows_count, clear_block_adler);
    }
    return changed_count;
}

// -- OSD geändert
uint32_t generate_clear_block_adler ( const int check_point_count )
{
    uint32_t adler;
    uint32_t null_data=0;
    int i;
    adler=adler32(0,Z_NULL,0);
    for (i=0; i < check_point_count; i++)
        adler = adler32 (adler,(unsigned char*)&null_data,4);
    return adler;    
}


int osd_blocks_changed ( struct block_s *blocks, const int block_count,
       const int *check_pixel_in_row, const int check_pixel_per_row,
       const int *check_rows, const int check_rows_count,
       const uint32_t clear_block_adler) 
{
    uint32_t new_adler;
    uint32_t null_data=0;
    const uint32_t *aktcheckRow;
     
    register int i;
    register int j;
    register int k;
    int changed_count=0;
  
    for (i=0; i < block_count; i++) 
    {
        new_adler=adler32(0,Z_NULL,0);
        for (j=0; j < check_rows_count; j++) 
        {
            aktcheckRow=blocks[i].rowPointer[check_rows[j]];
            for (k=0; k < check_pixel_per_row ; k++) 
            {
                if ( *((unsigned char*)(aktcheckRow+check_pixel_in_row[k])) == 0 )
                    new_adler = adler32 (new_adler,(unsigned char*)&null_data,4);
                else    
                    new_adler = adler32 (new_adler,(unsigned char*)(aktcheckRow+check_pixel_in_row[k]),4);
            }
        }        
        if (blocks[i].adler != new_adler ) 
        {
            blocks[i].adler = new_adler;
            blocks[i].changed = 1;
            blocks[i].clear = new_adler == clear_block_adler;
            for (k=0; k < MAX_CLIENTS ; k++)
            {
                if (client[k]==-1)
                    continue;
                if (! (client_blocks_changed[k])[i] ) 
                {
                    (client_blocks_changed[k])[i]=1;
                    client_blocks_changed_count[k]++;
                }    
            }     
            changed_count++;
        }
    }         
    return changed_count;
}


///////////////////////////////////////////////////////////////////////////////
int init_blocks ( const uint32_t *fb_mem, const int block_width, 
        const int block_height,  const int bpp, const int check_prom, 
        const int compression_level,  struct block_s **blocks, 
        int *block_count, int **check_pixel_in_row, int *check_pixel_per_row,
        int **check_rows, int *check_rows_count, const int verbose)
{
    int block_pixel = block_width*block_height;
    int block_row_count = (WIDTH/block_width);
    int block_col_count = (HEIGHT/block_height);
    
    *block_count = block_row_count*block_col_count;
    int check_pixel_wished = max(9,(check_prom*block_pixel+500)/1000);

    int i,j;
    
    double check_block_width=block_width/2;
    double check_block_height=block_height/2;
    int check_x_count=3;
    int check_y_count=3;
    while (check_x_count*check_y_count < check_pixel_wished && 
            (check_block_width > 2 || check_block_height>2) &&
            (check_y_count < block_height || check_x_count < block_width) ) 
    {
        if (check_block_width>check_block_height) {
           check_block_width/=2;
           check_x_count+=check_x_count-1;
           continue;
        }
        check_block_height/=2;
        check_y_count+=check_y_count-1;
    }
    *check_pixel_per_row=check_x_count;
    *check_rows_count=check_y_count;
    if ( ( *check_pixel_in_row = malloc (sizeof(int) * check_x_count) ) == NULL)
    {
        fprintf(stderr,"Kann keinen Speicher allocieren!\n");
        return 0;
    }
    if ( ( *check_rows = malloc (sizeof(int) * check_y_count) ) == NULL)
    {
        fprintf(stderr,"Kann keinen Speicher allocieren!\n");
        return 0;
    }
    (*check_pixel_in_row)[0]=0;
    (*check_pixel_in_row)[check_x_count-1]=block_width-1;
    (*check_rows)[0]=0;
    (*check_rows)[check_y_count-1]=block_height-1;
    for (i=1; i< check_x_count-1; i++) 
        (*check_pixel_in_row)[i]=min(((int)(check_block_width*i+0.5))-1,block_width-1);
 
    for (i=1; i<check_y_count-1; i++) 
       (*check_rows)[i]=min(((int)(check_block_height*i+0.5))-1,block_height-1); 
    
    
    if (verbose) {
       fprintf(stderr,"CHECK PIXEL: ");
       for (i=0; i<check_x_count; i++)
          fprintf(stderr,"%d ", (*check_pixel_in_row)[i]);
       fprintf(stderr,"\nIN ROWS: ");
       for (i=0; i<check_y_count; i++)
          fprintf(stderr,"%d ", (*check_rows)[i]);
       fprintf(stderr,"\nOF EACH BLOCK\n");
    }
    
    
    if ( ( *blocks = malloc (sizeof(struct block_s) * *block_count) ) == NULL ) 
    {
        fprintf(stderr,"Kann keinen Speicher allocieren!\n");
        return 0;
    }
    
    for (i=0; i < MAX_CLIENTS; i++)
        if ( ( client_blocks_changed[i] = malloc (sizeof(int) * *block_count) ) == NULL ) 
        {
            fprintf(stderr,"Kann keinen Speicher allocieren!\n");
            return 0;
        }
        
    int x_off,y_off,y;
    struct block_s *akt_block;
    for (i = 0,y_off=0, akt_block=*blocks; i < block_col_count ; i++,y_off+=(block_height>>1)) 
    {
        for (j=0,x_off=0; j < block_row_count ; j++, akt_block++, x_off+=block_width) 
        {
            if ( (akt_block->rowPointer = malloc(sizeof(uint32_t *)*block_height )) == NULL)
            {
                fprintf(stderr,"Kann keinen Speicher allocieren!\n");
                return 0;
            }
            
            if ( (akt_block->raw_buf = malloc(block_pixel*(bpp>>3) + (compression_level?block_pixel:0) )) == NULL)
            {
                fprintf(stderr,"Kann keinen Speicher allocieren!\n");
                return 0;
            } 
            
            // Setup Row-Pointer
            for (y=0 ; y < block_height ; y+=2) 
            {
                 akt_block->rowPointer[y]=fb_mem + ((y_off+(y>>1))*WIDTH) + x_off;
                 akt_block->rowPointer[y+1]=fb_mem + ((y_off+(y>>1))*WIDTH) + x_off + SNDFIELD;
            }
            akt_block->adler=0;
            akt_block->changed=1;
        }
    }
    return 1;
}

void free_blocks (struct block_s *blocks, int block_count,
        int *check_pixel_in_row, int *check_rows)
{
    int i;
    for (i=0; i< block_count; i++)
    {
         free (blocks[i].rowPointer);
         free (blocks[i].raw_buf);
    }
    for (i=0; i< MAX_CLIENTS; i++)
        free(client_blocks_changed[i]);
    free(blocks);
    free(check_pixel_in_row);
    free(check_rows);
}

void main_loop(const int main_socket, const uint32_t *fb_mem, 
        const int verbose, const int block_width, const int block_height, 
        const int check_prom, const int bpp, const int compression_level, 
        const struct timeval *sleep_time_tv )
{
    fd_set read_set, except_set;
    int i;
    int max_socket,status;
    struct block_s *blocks;
    int block_count;
    int *check_pixel_in_row;
    int check_pixel_per_row;
    int *check_rows;
    int check_rows_count;
    struct timeval timeout;
    int received;
    uint32_t clear_block_adler;
    int block_nr;
    int wakeuped;

    
    if ( !init_blocks (fb_mem, block_width, block_height, bpp, check_prom, 
            compression_level,  &blocks, &block_count, &check_pixel_in_row,
            &check_pixel_per_row, &check_rows, &check_rows_count, 
            verbose) )
        return;
    
    
    clear_block_adler = generate_clear_block_adler ( check_pixel_per_row * check_rows_count );

    int changed_count=0;
    osd_blocks_changed ( blocks, block_count, check_pixel_in_row, 
           check_pixel_per_row, check_rows, check_rows_count,
           clear_block_adler); 
    
    
    for(;!sigtermrecv;)
    {
	FD_ZERO(&except_set);
	FD_ZERO(&read_set);

	// LISTEN-HANDLE eintragen
	FD_SET(main_socket,&read_set);
        max_socket = main_socket+1;

	// clients eintragen
	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
	    if ( client[i] != -1 )
	    {
		FD_SET(client[i],&except_set);
		FD_SET(client[i],&read_set);
		if ( max_socket <= client[i] )
		    max_socket = client[i]+1;
	    }
	}

        timeout.tv_sec  = sleep_time_tv->tv_sec;
        timeout.tv_usec = sleep_time_tv->tv_usec;

        status = select(max_socket,&read_set,NULL,&except_set,n_clients==0?NULL:&timeout);

        if ( status < 0 )
        {
   	    /* sollte nicht vorkommen, kommt es aber bei Sigalen an den Client
	    if (verbose)
	       fprintf(stderr,"SELECT ERROR #%d\n",status);*/
	    continue;   
        }

        wakeuped=0;   
        if (status)
	{
	    if ( FD_ISSET(main_socket,&read_set) )
	         accept_client(main_socket,block_count,verbose);
		    
	    for ( i = 0; i < MAX_CLIENTS; i++ )
	    {
	        if ( client[i] == -1 )
		    continue;
		      
		if ( FD_ISSET(client[i],&except_set) )
		    receive_data(i,MSG_OOB,verbose,block_count,&block_nr);
			
                if ( FD_ISSET(client[i],&read_set) ) 
                {
		    received=receive_data(i,0,verbose,block_count,&block_nr);

		    switch (received) 
		    {
                        case RECV_UNKNOWN:
 		            send_msg("ERROR\r\n",7,i,MSG_DONTWAIT|MSG_NOSIGNAL,verbose);
		            break;
                    
		        case RECV_GET_CONFIG:
 		            send_config (block_width,block_height,bpp,i,
 		                    verbose);
		            break;
		            
		        case RECV_WAKEUP:
		            if (wakeuped)
		                break;
		                
		            wakeuped=1;    
                            if ( (changed_count = osd_blocks_changed ( blocks, 
                                    block_count, check_pixel_in_row, 
                                    check_pixel_per_row, check_rows, 
                                    check_rows_count, clear_block_adler)) > 0)
                            {
                                send_modified ( changed_count, verbose);
                            }    
                            break;
                            
                        case RECV_GET_BLOCK:
                            changed_count=send_block ( &blocks[block_nr], block_nr,
                                    block_width, block_height, bpp, 
                                    compression_level, i,
                                    verbose, check_pixel_in_row, 
                                    check_pixel_per_row, check_rows, 
                                    check_rows_count, clear_block_adler); 
                            if (changed_count>0)
                                send_modified ( changed_count, verbose);
                            break;
                        
                        case RECV_GET_BLOCKS_MODIFIED:
  		            changed_count=send_modified_blocks (blocks, block_count, 
                                    block_width, block_height, bpp, 
                                    compression_level, i, 
                                    verbose, check_pixel_in_row, 
                                    check_pixel_per_row, check_rows, 
                                    check_rows_count, clear_block_adler);
                            if (changed_count>0)
                                send_modified ( changed_count, verbose);
                            break;
                            
                        case RECV_GET_WHICH_BLOCKS_MODIFIED: 
  		            send_which_blocks_modified (i, block_count,
  		                    verbose);
                            break;

                        case RECV_GET_VERSION:
                            send_version (i,verbose);
                            break; 
                    }          
                } 
            }
	}
	if (!wakeuped && n_clients > 0  && 
             (changed_count = osd_blocks_changed ( blocks, block_count, 
                    check_pixel_in_row, check_pixel_per_row, check_rows, 
                    check_rows_count, clear_block_adler)) > 0)
        {
            send_modified ( changed_count, verbose);
        }
    }
    free_blocks (blocks, block_count, check_pixel_in_row, check_rows);
}

// ---- Command-Line - Parsing ----------------

void print_usage () {
    fprintf(stderr,TITLE);
    fprintf(stderr,"\n"
		   "usage: " NAME " [-v] [-x|--block_width n] [-y|--block_height n] [-p|--check_block_part n] [-b|--bits_per_pixel n] [-c|--compression_level n] [-w|--wait_frame n] port\n"
		   "   -v : \n"
		   "       verbose\n"
		   "   -x | --block_width n :\n" 
		   "       Breite eines Blocks des OSDs. n muss ein Teiler von 720 sein.\n"
		   "       Bereich: 10..720\n"
		   "       Standardwert: 60\n"
		   "   -y | --block_height n :\n" 
		   "       Hoehe eines Blocks des OSDs. n muss ein Teiler von 576 sein.\n"
		   "       Bereich: 12..576\n"
		   "       Standardwert: 32\n"
		   "   -c | --compression_level n :\n" 
		   "       Kompression des uebertragenen Bilds\n"
		   "       Werte: 0 (keine Kompression), 1 (RLE)\n"
		   "       Standardwert: 1\n"
		   "   -p | --check_block_part n :\n"
		   "       n/1000 eines Blocks bei der Aenderungserkennung verwenden.\n"
		   "       Bereich: 1..500\n"
		   "       Standardwert: 15\n"
		   "   -b | --bits_per_pixel n :\n"
		   "       Bits pro Pixel die uebertragen werden\n"
		   "       Moegliche Werte: 1 := 32 Bits/Pixel, 2 := 16 Bits/Pixel, 3:= 8 Bits/Pixel\n"
                   "       Standardwert: 3\n"
		   "   -w | --wait_frames n :\n" 
		   "       n Frames (1/25 Sekunde) zwischen Aenderungspruefungen warten\n"
		   "       Bereich: 1..50 \n"
		   "       Standardwert: 13\n"
		   "   port : 1..65535\n"
		   "\n");
}

int parse_command_line ( int argc, char **argv, int *verbose, int *block_width,
        int *block_height, int *check_prom, int *bpp , int *compression_level, int *sleep_time, int *port)
{
    int  param_count = 0, err = 0, i, wrong_value=0, option,value=0;
    char * arg;
    *block_width=60; *block_height=32; *verbose=0; *compression_level=1; *sleep_time=13;
    *port=0; *check_prom=15; *bpp=8;
    
    for ( i=1; i<argc && !err && !wrong_value ; i++ )
    {
	arg = argv[i];
	option=i;
	if ( *arg== '-' )
	{
	    arg++;
            switch (*arg) 
            {
                case '-':
                    // Long-Parameter
                    arg++;
                    
                    if ( !strncmp(arg,"block_width",11) )
                    {
                        arg+=11;
                        if (! *arg && i < argc) 
                        {
                            i++;
                            arg=argv[i];
                        }
                        value = *block_width = strtoul(arg,&arg,10);
                        if ( *arg || value < 10 || value > 720 || 720 % value != 0 ) 
                           wrong_value++;
                        break;
                    }
                    
                    if ( !strncmp(arg,"block_height",12) )
                    {
                        arg+=12;
                        if (! *arg && i < argc) 
                        {
                            i++;
                            arg=argv[i];
                        }
                        value = *block_height = strtoul(arg,&arg,10);
                        if ( *arg || value < 12 || value > 576 || 576 % value != 0) 
                           wrong_value++;
                        break;
                    }
                    
                    if ( !strncmp(arg,"compression_level",17) )
                    {
                        arg+=17;
                        if (! *arg && i < argc) 
                        {
                            i++;
                            arg=argv[i];
                        }
                        value = *compression_level = strtoul(arg,&arg,10);
                        if ( *arg || value < 0 || value > 1) 
                           wrong_value++;
                        break;
                    }
                    
                    if ( !strncmp(arg,"wait_frames",11) )
                    {
                        arg+=11;
                        if (! *arg && i < argc) 
                        {
                            i++;
                            arg=argv[i];
                        }
                        value = *sleep_time = strtoul(arg,&arg,10);
                        if ( *arg || value < 1 || value > 50) 
                           wrong_value++;
                        break;
                    }
                    
                    if ( !strncmp(arg,"check_block_part",16) )
                    {
                        arg+=16;
                        if (! *arg && i < argc) 
                        {
                            i++;
                            arg=argv[i];
                        }
                        value = *check_prom = strtoul(arg,&arg,10);
                        if ( *arg || value < 1 || value > 500) 
                           wrong_value++;
                        break;
                    }
                    
                    if ( !strncmp(arg,"bits_per_pixel",14) )
                    {
                        arg+=14;
                        if (! *arg && i < argc) 
                        {
                            i++;
                            arg=argv[i];
                        }
                        value = strtoul(arg,&arg,10);
                        if ( *arg || value < 1 || value > 3) {
                           wrong_value++;
                           break;
                        }
                        
                        switch (value) 
                        {
                           case 1: *bpp=32; break;
                           case 2: *bpp=16; break;
                           case 3: *bpp=8; break;
                        }
                        break;
                    }
                    
                    err++;
                    break;
	        case 'v':
	            *verbose = 1;
	            arg++;
	            if (*arg)
	               err++;
		    break;
		case 'x':
		    arg++;
                    if (! *arg && i < argc) 
                    {
                        i++;
                        arg=argv[i];
                    }
                    value = *block_width = strtoul(arg,&arg,10);
                    if ( *arg || value < 10 || value > 720 || 720 % value != 0 ) 
                       wrong_value++;
                    break;
                case 'y':
                    arg++;
                    if (! *arg && i < argc) 
                    {
                        i++;
                        arg=argv[i];
                    }
                    value = *block_height = strtoul(arg,&arg,10);
                    if  ( *arg || value < 12 || value > 576 || 576 % value != 0) 
                        wrong_value++;
                    break;
                case 'c':
                    arg++;
                    if (! *arg && i < argc) 
                    {
                        i++;
                        arg=argv[i];
                    }
                    value = *compression_level = strtoul(arg,&arg,10);
                    if ( *arg || value < 0 || value > 1 ) 
                        wrong_value++;
                    break;
                case 'w':
                    arg++;
                    if (! *arg && i < argc) 
                    {
                        i++;
                        arg=argv[i];
                    }
                    value = *sleep_time = strtoul(arg,&arg,10);
                    if ( *arg || value < 1 || value > 50 ) 
                        wrong_value++;
                    break;
                case 'p':
                    arg++;
                    if (! *arg && i < argc) 
                    {
                        i++;
                        arg=argv[i];
                    }    
                    value = *check_prom = strtoul(arg,&arg,10);
                    if ( *arg || value < 1 || value > 500) 
                        wrong_value++;
                    break;        
                case 'b':    
                    arg++;
                    if (! *arg && i < argc) 
                    {
                        i++;
                        arg=argv[i];
                    }
                    value = strtoul(arg,&arg,10);
                    if ( *arg || value < 1 || value > 3) {
                        wrong_value++;
                        break;
                    }
                        
                    switch (value) 
                    {
                        case 1: *bpp=32; break;
                        case 2: *bpp=16; break;
                        case 3: *bpp=8; break;
                    }
                    break;
		default:
                    err++;
            }    
	}
	else
	{
	    param_count++;
	    *port = strtoul(arg,&arg,10);
	    if (*arg)
		err++;
	}
    }
    if (err)
    {
        fprintf(stderr,"illegal option: %s\n",argv[option]);
        print_usage();
        return EXIT_FAILURE;
    }
    
    if (wrong_value)
    {
        fprintf(stderr,"wrong value %d for option: %s\n",value,argv[option]);
        print_usage();
        return EXIT_FAILURE;
    }
    if ( param_count != 1 || *port < 1 || *port > 65535 ) 
    {
        print_usage();
        return EXIT_FAILURE;
    }
    return 0;
}
        

///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
    int verbose, block_width, block_height, check_prom, bpp , compression_level, sleep_time, port;
    
    if ( parse_command_line(argc, argv, &verbose,&block_width,
            &block_height, &check_prom, &bpp , &compression_level, &sleep_time, &port) )
        return EXIT_FAILURE;        

    // Framebuffer öffnen
    int fb_fd;
    
    if ( (fb_fd = open("/dev/fb",O_RDWR)) < 0 ) 
    {
        fprintf(stderr,"Kann Framebuffer-Device nicht oeffnen!\n");
        return EXIT_FAILURE;
    }
    
    // Framebuffer-Format testen
    struct fb_var_screeninfo fb_info;
    
    if ( ioctl(fb_fd,FBIOGET_VSCREENINFO,&fb_info) < 0)
    {
        fprintf(stderr,"Kann Framebuffer Einstellungen nicht lesen!\n");
        return EXIT_FAILURE;
    }
    
    if ( fb_info.xres != WIDTH || fb_info.yres != HEIGHT 
            || fb_info.bits_per_pixel != 32 || !(fb_info.vmode&FB_VMODE_INTERLACED) )
    {
        fprintf(stderr,"Framebuffer nicht im %dx%dx32 interlaced Modus!\n",WIDTH,HEIGHT);
        return EXIT_FAILURE;
    }
    
    // Framebuffer-Memory-Map    
    uint32_t *fb_mem;
    
    fb_mem = (uint32_t*) mmap(NULL,(HEIGHT*WIDTH)<<2,PROT_READ,MAP_SHARED,fb_fd,0);
    
    if ( fb_mem == (uint32_t*) -1 )
    {
        fprintf(stderr,"Kann Framebuffer-Memory nicht mappen!\n");
        return EXIT_FAILURE;
    }
    
    close(fb_fd);
    // initialize

    int i;
    
    for ( i = 0; i < MAX_CLIENTS; i++ )
	client[i] = -1;
	
    struct timeval sleep_time_tv;
    sleep_time_tv.tv_sec=sleep_time/25;
    sleep_time_tv.tv_usec=(sleep_time%25)*40000;

    if (verbose)
    {
	fprintf(stderr,TITLE "\n");
	fprintf(stderr,"PORT: %u\n",port);
	fprintf(stderr,"BLOCK WIDTH: %d\n",block_width);
	fprintf(stderr,"BLOCK HEIGHT: %d\n",block_height);
	fprintf(stderr,"CKECK %d/1000 OF BLOCK\n",check_prom);
	fprintf(stderr,"BITS PER PIXEL: %d\n",bpp);
	fprintf(stderr,"COMPRESSION LEVEL: %d\n",compression_level);
	fprintf(stderr,"WAIT FRAMES: %d\n",sleep_time);
    }

    // create main socket

    int main_socket = socket(PF_INET,SOCK_STREAM,0);
    if (verbose)
	fprintf(stderr,"MAIN SOCKET: %u\n",main_socket);

    if ( main_socket == -1 )
    {
	fprintf(stderr,"call of socket() failed\n");
	return EXIT_FAILURE;
    }

    // bind socket

    struct sockaddr_in sa;
    memset(&sa,0,sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = INADDR_ANY;
    if ( bind(main_socket,(struct sockaddr*)&sa,sizeof(sa)) )
    {
	fprintf(stderr,"call of bind() failed\n");
	return EXIT_FAILURE;
    }
    
    // listen

    if ( listen(main_socket,10) )
    {
	fprintf(stderr,"call of listen() failed\n");
	return EXIT_FAILURE;
    }

    // main loop
    signal (SIGHUP,&sighandler);
    signal (SIGINT,&sighandler);
    signal (SIGTERM,&sighandler);
    main_loop(main_socket, fb_mem, verbose, block_width, block_height, 
        check_prom, bpp, compression_level, &sleep_time_tv);

    // close all clients

    for ( i = 0; i < MAX_CLIENTS; i++ )
	close_client(i,verbose);

    // close main socket

    close(main_socket);
    munmap(fb_mem,(HEIGHT*WIDTH)<<2);

    // terminate

    if (verbose)
	fprintf(stderr,"EXIT SUCCESS\n");
    return EXIT_SUCCESS;
}
