//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// The project's page is at http://www.assembla.com/wiki/show/VDR-M7x0
//

#define MAX_SCANNER ((1048576))
#define MAX_STREAMS (2)

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <netinet/in.h>

#include <fcntl.h>
#include <unistd.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>

FILE* file_pes=NULL;
unsigned char stream[MAX_SCANNER];
int byte_read=0;
int f=0;
int Need(int Count)
{
	if(f+Count>byte_read)
	{
		if(f)
		{
			if(f<byte_read)
			{
				if(byte_read-f>f)
					memmove(stream,stream+f,byte_read-f);
				else
					memcpy(stream,stream+f,byte_read-f);
				byte_read-=f;
			}
			else
				byte_read=0;
			f=0;
		}
		while(byte_read<MAX_SCANNER)
		{
			int new_byte_read=fread(stream+byte_read,1,MAX_SCANNER-byte_read,file_pes);
			if(new_byte_read<=0)
				return 1;
			byte_read+=new_byte_read;
			if(Count<=byte_read)
				return 0;
			sleep(1);
		}
	}
	return 0;
}

ssize_t safe_write(int filedes, const void *buffer, size_t size)
{
  ssize_t p = 0;
  ssize_t written = size;
  const unsigned char *ptr = (const unsigned char *)buffer;

  while (size > 0) {
        p = write(filedes, ptr, size);
        if (p < 0) {
           if (errno == EINTR || errno == EBUSY) {
              continue;
              }
           break;
           }
        ptr  += p;
        size -= p;
        }
  return p < 0 ? p : written;
}

#define MAX_WRITE (16*524288)
unsigned char *output=NULL;
int fout=0;
int foutput=-1;
int Write(unsigned char *Buffer,int Len)
{	
	if(fout+Len>=MAX_WRITE)
	{
		if(safe_write(foutput,output,fout)!=fout)
		{
			close(foutput);
			fprintf(stderr,"Error (%d,%d) writing file\n",fout,errno);
			foutput=-1;
			return 0;
		}
		fout=0;
	}
	memcpy(output+fout,Buffer,Len);
	fout+=Len;
	return 1;
}


int main(int argc, char *argv[])
{
	if(argc<=2)
	{
		fprintf(stderr, "Usage: 'pes2ts filename.pes output.mpeg'\n");
		return 1;
	}
	file_pes=fopen(argv[1],"rb");
	if(!file_pes)
	{
		fprintf(stderr, "Can't open file %s\n", argv[1]);
		return 2;
	}

	if(posix_memalign((void **)&output,65536,MAX_WRITE))
	{
		fprintf(stderr, "Can't allocate aligned memory\n");
		return 2;
	}	
	foutput=open(argv[2],O_CREAT|O_WRONLY|O_APPEND,0600);
	if(foutput<0)
	{
		fprintf(stderr, "Can't open file %s\n", argv[2]);
		return 2;
	}

	unsigned char TS[188];  //Program Association Table
        memset(TS,0xFF,188);
        TS[0]=0x47;
        TS[1]=0x41; //0-error, 1-payloadStart, 0-priority, 00001-Pid=1
        TS[2]=0x00; //00000000-PAT-Pid=0
        TS[3]=0x10; //00-notScrambled=0, 01-noAdaptationOnlyPayload, 0000-continuityCounter
//Adaptation header
        //TS[4]=0x07; //Adaptacion Lenght
        //TS[5]=0x00; //Adaptacion flags
	//TS[6]-TS[11] PCR


	unsigned char TSPAT[188];  //Program Association Table
        memset(TSPAT,0xFF,188);
        TSPAT[0]=0x47;
        TSPAT[1]=0x40; //0-error, 1-payloadStart, 0-priority, 00000-PAT-Pid=0
        TSPAT[2]=0x00; //00000000-PAT-Pid=0
        TSPAT[3]=0x10; //00-notScrambled=0, 01-noAdaptationOnlyPayload, 0000-continuityCounter
        TSPAT[4]=0x00; //Pointer field
        TSPAT[5]=0x00; //Table-Id-PAT
        TSPAT[6]=0xB0; //1-syntax, 0, 11-reserved, 00, 00-lenght-TS-header
        TSPAT[7]=0x0D; //lenght-TS-header
        TSPAT[8]=0x00; //TS-ID
        TSPAT[9]=0x01; //TS-ID  
        TSPAT[10]=0xC1; //11-reserved, 00000-version, 1-current/next
        TSPAT[11]=0x00; //section_number
        TSPAT[12]=0x00; //last_section_number
        TSPAT[13]=0x00; //program_number
        TSPAT[14]=0x01; //program_number
        TSPAT[15]=0xEF; //111-reserved, 01111 program_map_PID
        TSPAT[16]=0xFF; //11111111-program_map_PID
        TSPAT[17]=0x36; //crc-
        TSPAT[18]=0x90; //crc-
        TSPAT[19]=0xE2; //crc-
        TSPAT[20]=0x3D; //crc-

	unsigned char TSPMS[188]; //Program Map Section
        memset(TSPMS,0xFF,188);
        TSPMS[0]=0x47;
        TSPMS[1]=0x4F; //0-error, 1-payloadStart, 0-priority, 01111-program_map_PID 
        TSPMS[2]=0xFF; //11111111-program_map_PID
        TSPMS[3]=0x10; //00-notScrambled, 01-noAdaptationOnlyPayload, 0000-continuityCounter
        TSPMS[4]=0x00; //Pointer field
        TSPMS[5]=0x02; //Table-Id
        TSPMS[6]=0xB0; //1-syntax, 0, 11-reserved, 00, 00-lenght
        TSPMS[7]=0x17; //lenght
        TSPMS[8]=0x00; //Program Number
        TSPMS[9]=0x01; //Program Number
        TSPMS[10]=0xC1; //11-reserved, 00000-version, 1-current/next
        TSPMS[11]=0x00; //section_number
        TSPMS[12]=0x00; //last_section_number
        TSPMS[13]=0xE1; //111-Reserved 00001 PCR_PID
        TSPMS[14]=0x00; //PCR_PID
        TSPMS[15]=0xF0; //1111-reserved, 0000 program_info_leght
        TSPMS[16]=0x00; //program_info_leght

        TSPMS[17]=0x1B; //streamType  // 1B MPEG4 01/02 Video 03/04 Audio
        TSPMS[18]=0xE1; //111-reserved 00001-Video-Pid
        TSPMS[19]=0x00; //Video-Pid
        TSPMS[20]=0xF0; //1111-reserved, 0000-es_info_lenght
        TSPMS[21]=0x00; //es_info_lenght

// Audio Dolby Digital
/*        TSPMS[22]=0x06; //streamType //03-Audio //81-Reserved //06 PES Private
        TSPMS[23]=0xE1; //111-reserved 00001-Audio-Pid
        TSPMS[24]=0x01; //Audio-Pid
        TSPMS[25]=0xF0; //1111-reserved, 0000-es_info_lenght
        TSPMS[26]=0x00; //es_info_lenght

        TSPMS[27]=0x27; //crc-D1
        TSPMS[28]=0xA0; //crc-D7
        TSPMS[29]=0xD9; //crc-F4
        TSPMS[30]=0x10; //crc-B8*/

//Audio MPEG
        TSPMS[22]=0x03; //streamType //03-Audio //81-Reserved //06 PES Private
        TSPMS[23]=0xE1; //111-reserved 00001-Audio-Pid
        TSPMS[24]=0x01; //Audio-Pid
        TSPMS[25]=0xF0; //1111-reserved, 0000-es_info_lenght
        TSPMS[26]=0x00; //es_info_lenght

        TSPMS[27]=0x4E; //crc-D1
        TSPMS[28]=0x59; //crc-D7
        TSPMS[29]=0x3D; //crc-F4
        TSPMS[30]=0x1E; //crc-B8

	unsigned char TSPADDING[188]; //Program Map Section
        memset(TSPADDING,0xFF,188);


	/*if(Need(8))
	{
		fprintf(stderr,"File too short\n");
		return 3;
	}
	if((stream[0])||(stream[1])||(stream[2]!=1)||((stream[3]&0xF0)!=0xE0))
	{
		fprintf(stderr,"No video in first packet\n");
		return 3;
	}*/
	unsigned char streamNumber[MAX_STREAMS];
	memset(streamNumber,0,MAX_STREAMS);
	int streamCounter[MAX_STREAMS];
	memset(streamCounter,0,MAX_STREAMS*4);
	/*streamNumber[0]=stream[3];
	if(!(stream[6]&0x04))
	{
		fprintf(stderr,"First video packet incomplete\n");
		return 3;
	}
	if(!(stream[7]&0x80))
	{
		fprintf(stderr,"First video packet without Time\n");
		return 3;
	}*/
	//unsigned int Time=((stream[9]&0x0E)<<28)|(stream[10]<<24)|((stream[11]&0xFE)<<15)|(stream[12]<<8)|(stream[13]>>1);
	while(!streamNumber[0])
	{
		if(Need(4))
		{
			fprintf(stderr,"No video in record\n");
			return 3;
		}
		if((stream[f])||(stream[f+1])||(stream[f+2]!=1))
		{
			f++;
			continue;
		}
		unsigned char PESID=stream[f+3];
		if((PESID&0xF0)==0xE0)
		{
			if(Need(9))
			{
				fprintf(stderr,"No video in record\n");
				return 4;
			}
			if(stream[f+6]&0x04)
			{
				int HeaderLenght=stream[f+8]+9;
				if(Need(HeaderLenght+4))
				{
					fprintf(stderr,"No video in record\n");
					return 4;
				}
				if((!stream[f+HeaderLenght])||(!stream[f+HeaderLenght+1])||(stream[f+HeaderLenght+2]==1))
				{
					streamNumber[0]=PESID;
					if(stream[f+HeaderLenght+3]!=0x09)
					{
						TSPMS[17]=0x02;
						TSPMS[27]=0xF6; //crc-D1
						TSPMS[28]=0x4A; //crc-D7
						TSPMS[29]=0x03; //crc-F4
						TSPMS[30]=0x55; //crc-B8
						fprintf(stderr,"MPEG2 Video detected\n");
						break;
					}
					else
					{
						fprintf(stderr,"H264 Video detected\n");
						break;
					}
				}	
			}
		}
		if((PESID==0xBD)||((PESID&0xE0)==0xC0)||((PESID&0xF0)==0xE0))
		{
			if(Need(6))
			{
				fprintf(stderr,"No video in record\n");
				return 4;
			}
			f+=stream[f+4]*256+stream[f+5]+6;
		}
		else
			f+=4;
	}
	if(!Write(TSPAT,188))
		return 3;
	if(!Write(TSPMS,188))
		return 3;
	int PAT=0;
	int PatCounter=0;
	while(byte_read>=0)
	{
		if(Need(4))
			break;
		if((stream[f])||(stream[f+1])||(stream[f+2]!=1))
		{
			f++;
			continue;
		}
		unsigned char PESID=stream[f+3];
		int currentStreamNumber;
		if ((!streamNumber[1])&&((PESID&0xE0)==0xC0))
			streamNumber[1]=PESID;
		int g=0;
		for(;g<MAX_STREAMS;g++)
			if(PESID==streamNumber[g])
			{
				currentStreamNumber=g;
				break;
			}
		if(g==MAX_STREAMS) //Forget this packet
		{
			if((PESID==0xBD)||((PESID&0xE0)==0xC0)||((PESID&0xF0)==0xE0))
			{
				if(Need(6))
					break;
				f+=stream[f+4]*256+stream[f+5]+6;
			}
			else
				f+=4;
			continue;
		}
		if(Need(9))
			break;
		int PESLenght=stream[f+4]*256+stream[f+5]+6;
		if(PESLenght==6)
		{
			fprintf(stderr,"No Lenght in PES packet(%d). Not a VDR-NG-EM record\n",PESID);
			break;
		}
		/*unsigned int currentTime=0;
		int HaveTime=stream[f+7]&0x80;
		if(HaveTime)
		{
			if(Need(14))
				continue;
			currentTime=(((stream[f+9]&0x0E)<<28)|(stream[f+10]<<24)|((stream[f+11]&0xFE)<<15)|(stream[f+12]<<8)|(stream[f+13]>>1))-Time;
			stream[f+9]=((currentTime>>28)&0x0E)|((stream[f+7]&0xC0)>>2);
			stream[f+10]=(currentTime>>21)&0xFF;
			stream[f+11]=((currentTime>>13)&0xFE)|0x01;
			stream[f+12]=(currentTime>>7)&0xFF;
			stream[f+13]=((currentTime<<1)&0xFE)|0x01;
			if(stream[f+7]&0x40)
			{
				if(Need(19))
					continue;
				unsigned int NewTime=((stream[f+14]&0x0E)<<28)|(stream[f+15]<<24)|((stream[f+16]&0xFE)<<15)|(stream[f+17]<<8)|(stream[f+18]>>1)-Time;
				stream[f+15]=((NewTime>>28)&0x0E)|0x10;
				stream[f+15]=(NewTime>>21)&0xFF;
				stream[f+16]=((NewTime>>13)&0xFE)|0x01;
				stream[f+17]=(NewTime>>7)&0xFF;
				stream[f+18]=((NewTime<<1)&0xFE)|0x01;
			}
		}*/
		if((!currentStreamNumber)&&(stream[f+6]&0x04))
		{
			if(PAT>=10)
			{
				TSPAT[3]=(0xF0&TSPAT[3])|PatCounter;
				if(!Write(TSPAT,188))
					return 3;
				TSPMS[3]=(0xF0&TSPMS[3])|PatCounter;
				if(!Write(TSPMS,188))
					return 3;
				PAT-=10;
				PatCounter=(PatCounter+1)&0x0F;
			}
			else
				PAT++;
		}
		int Start=1;
		int HaveTime=((!currentStreamNumber)&&(stream[f+7]&0x80));
		int ToWrite=188-(HaveTime?12:4);
		TS[2]=currentStreamNumber;
		int Padding;
		while(PESLenght)
		{
			if(ToWrite>PESLenght)
			{
				//fprintf(stdout,"PES %d ToWrite %d\n",PESLenght,ToWrite);
				Padding=ToWrite-PESLenght;
				ToWrite=PESLenght;
				//fprintf(stdout,"Padding %d ToWrite %d\n",Padding,ToWrite);
			}
			else
				Padding=0;
			if(Need(ToWrite))
			{
				fprintf(stderr,"PES packet error, sincronizing\n");
				break;
			}
			if(Start)
			{
				Start=0;
				TS[1]=0x41;//0-error, 1-payloadStart, 0-priority, 00001 Video/Audio PID
				if(HaveTime)
				{
					HaveTime=0;
					TS[3]=0x30|streamCounter[currentStreamNumber];//00-notScrambled=0, 11-AdaptationAndPayload, 0000-continuityCounter
				        TS[4]=Padding+7; //Adaptacion Lenght
				        TS[5]=0x10; //Adaptacion flags PCR
					TS[6]=((stream[f+9]<<4)&0xE0)|(stream[f+10]>>3);
					TS[7]=(stream[f+10]<<5)|(stream[f+11]>>3);
					TS[8]=((stream[f+11]<<5)&0x60)|(stream[f+12]>>2);
					TS[9]=(stream[f+12]<<6)|(stream[f+13]>>2);
					TS[10]=(stream[f+13]<<7)&(0x80);
					TS[11]=0x00; //PCR program_clock_reference_extension
					if(!Write(TS,12))
						return 3;
					streamCounter[currentStreamNumber]=(streamCounter[currentStreamNumber]+1)&0x0F;
				}
				else
				{
					if(Padding)
					{
						TS[3]=0x30|streamCounter[currentStreamNumber];//00-notScrambled=0, 11-noAdaptationOnlyPayload, 0000-continuityCounter
						TS[4]=Padding-1; //Adaptacion Lenght
						if(TS[4])
						{
							TS[5]=0x00; //Adaptacion flags
							if(!Write(TS,6))
								return 3;
							Padding-=2;
						}
						else
						{
							if(!Write(TS,5))
								return 3;
							Padding--;
						}
					}
					else
					{
						TS[3]=0x10|streamCounter[currentStreamNumber];//00-notScrambled=0, 01-noAdaptationOnlyPayload, 0000-continuityCounter
						if(!Write(TS,4))
							return 3;
					}
					streamCounter[currentStreamNumber]=(streamCounter[currentStreamNumber]+1)&0x0F;			
				}
				TS[1]=0x01;//0-error, 0-payloadStart, 0-priority, 00001 Video/Audio PID
			}
			else
			{
				if(Padding)
				{
					TS[3]=0x30|streamCounter[currentStreamNumber];//00-notScrambled=0, 11-noAdaptationOnlyPayload, 0000-continuityCounter
					TS[4]=Padding-1; //Adaptacion Lenght
					if(TS[4])
					{
						TS[5]=0x00; //Adaptacion flags
						if(!Write(TS,6))
							return 3;
						Padding-=2;
					}
					else
					{
						if(!Write(TS,5))
							return 3;
						Padding--;
					}
				}
				else
				{
					TS[3]=0x10|streamCounter[currentStreamNumber];//00-notScrambled=0, 01-noAdaptationOnlyPayload, 0000-continuityCounter
					if(!Write(TS,4))
						return 3;
				}
				streamCounter[currentStreamNumber]=(streamCounter[currentStreamNumber]+1)&0x0F;			
			}
			if(Padding)
				if(!Write(TSPADDING,Padding))
					return 3;
			if(!Write(stream+f,ToWrite))
				return 3;
			f+=ToWrite;
			PESLenght-=ToWrite;
			ToWrite=188-4;
		}
	}



/*
	int Video=0;
	int VideoType=0;
	int Audio=0;
	int AudioType=0;
	int MasterStream=0;
	unsigned int Time=0;
	int f=0;
	for(;(f<byte_read-13)&&(!(Video&&Audio));f++)
	{
		if(!stream[f])
		{
			if((!stream[f+1])&&(stream[f+2]==1))
			{
				int PacketLenght=(stream[f+4]<<8)|(stream[f+5]);
				if((!Video)&&((stream[f+3]&0xF0)==0xE0))
				{
					if(stream[f+6]&0x04) //data aligment
					{
						int HeaderLenght=stream[f+8]+9;
						if(f+HeaderLenght+3<byte_read)
							if((!stream[HeaderLenght])&&(!stream[HeaderLenght+1])&&(stream[HeaderLenght+2]==0x01))
							{
								Video=stream[f+3];
								if(stream[HeaderLenght+3]==0x09)
									VideoType=0x1B;
								else
									VideoType=0x02;
							}
					}
					if((!Time)&&(stream[f+7]&0x80))
						Time=((stream[f+9]&0x0E)<<28)|(stream[f+10]<<24)|((stream[f+11]&0xFE)<<15)|(stream[f+12]<<8)|(stream[f+13]>>1);
				}
				if((!Audio)&&((stream[f+3]&0xE0)==0xC0))
				{
					if(stream[f+6]&0x04) //data aligment
					{
						int HeaderLenght=stream[f+8]+9;
						if(f+HeaderLenght+1<byte_read)
							if((stream[f+HeaderLenght]==0xFF)&&((stream[f+HeaderLenght+1]&0xF0)==0xF0))
							{
								Audio=stream[f+3];
								AudioType=0x05;
							}
					}
					if((!Time)&&(stream[f+7]&0x80))
						Time=((stream[f+9]&0x0E)<<28)|(stream[f+10]<<24)|((stream[f+11]&0xFE)<<15)|(stream[f+12]<<8)|(stream[f+13]>>1);
				}
				if(PacketLenght)
					f+=PacketLenght+5;
			}
		}
	}
	if(!(Video&&Audio))
	{
		fprintf(stderr,"No video or audio\n");
		return 4;
	}*/
	if(foutput>=0)
	{
		if(fout)
		{
			if(safe_write(foutput,output,fout)!=fout)
			{
				close(foutput);
				fprintf(stderr,"Error (%d,%d) writing file\n",fout,errno);
				foutput=-1;
				return 0;
			}
			fout=0;
		}
		close(foutput);
		foutput=-1;
	}
	free(output);
	return 0;
}

