/*
    Copyright 2004-2005 Chris Tallon

    This file is part of VOMP.
    and adopted for streamdev to play recordings

    VOMP is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    VOMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VOMP; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "recplayer.h"

#define _XOPEN_SOURCE 600
#include <fcntl.h>

RecPlayer::RecPlayer(cRecording* rec)
{
  file = NULL;
  fileOpen = 0;
  lastPosition = 0;
  recording = rec;
  for(int i = 1; i < 1000; i++) segments[i] = NULL;

  // FIXME find out max file path / name lengths

#if VDRVERSNUM >= 10703
  indexFile = new cIndexFile(recording->FileName(), false, rec->IsPesRecording());
#else
  indexFile = new cIndexFile(recording->FileName(), false);
#endif
  if (!indexFile) esyslog("ERROR: Streamdev: Failed to create indexfile!");

  scan();
}

void RecPlayer::scan()
{
  if (file) fclose(file);
  totalLength = 0;
  fileOpen = 0;
  totalFrames = 0;

  int i = 1;
  while(segments[i++]) delete segments[i];

  char fileName[2048];
  for(i = 1; i < 1000; i++)
  {

#if APIVERSNUM < 10703
    snprintf(fileName, 2047, "%s/%03i.vdr", recording->FileName(), i);
    //log->log("RecPlayer", Log::DEBUG, "FILENAME: %s", fileName);
    file = fopen(fileName, "r");
#else
    snprintf(fileName, 2047, "%s/%05i.ts", recording->FileName(), i);
    file = fopen(fileName, "r");
    if (!file) {
      snprintf(fileName, 2047, "%s/%03i.vdr", recording->FileName(), i);
      file = fopen(fileName, "r");
    }
#endif
    if (!file) break;

    segments[i] = new Segment();
    segments[i]->start = totalLength;
    fseek(file, 0, SEEK_END);
    totalLength += ftell(file);
    totalFrames = indexFile->Last();
    //log->log("RecPlayer", Log::DEBUG, "File %i found, totalLength now %llu, numFrames = %lu", i, totalLength, totalFrames);
    segments[i]->end = totalLength;
    fclose(file);
  }

  file = NULL;
}

RecPlayer::~RecPlayer()
{
  //log->log("RecPlayer", Log::DEBUG, "destructor");
  int i = 1;
  while(segments[i++]) delete segments[i];
  if (file) fclose(file);
}

int RecPlayer::openFile(int index)
{
  if (file) fclose(file);

  char fileName[2048];

#if APIVERSNUM >= 10703
  snprintf(fileName, 2047, "%s/%05i.ts", recording->FileName(), index);
  isyslog("openFile called for index %i string:%s", index, fileName);

  file = fopen(fileName, "r");
  if (file)
  {
    fileOpen = index;
    return 1;
  }
#endif

  snprintf(fileName, 2047, "%s/%03i.vdr", recording->FileName(), index);
  isyslog("openFile called for index %i string:%s", index, fileName);
  //log->log("RecPlayer", Log::DEBUG, "openFile called for index %i string:%s", index, fileName);

  file = fopen(fileName, "r");
  if (file)
  {
    fileOpen = index;
    return 1;
  }

  //log->log("RecPlayer", Log::DEBUG, "file failed to open");
  fileOpen = 0;
  return 0;
}

uint64_t RecPlayer::getLengthBytes()
{
  return totalLength;
}

uint32_t RecPlayer::getLengthFrames()
{
  return totalFrames;
}

unsigned long RecPlayer::getBlock(unsigned char* buffer, uint64_t position, unsigned long amount)
{
  if ((amount > totalLength) || (amount > 500000))
  {
    //log->log("RecPlayer", Log::DEBUG, "Amount %lu requested and rejected", amount);
    return 0;
  }

  if (position >= totalLength)
  {
    //log->log("RecPlayer", Log::DEBUG, "Client asked for data starting past end of recording!");
    return 0;
  }

  if ((position + amount) > totalLength)
  {
    //log->log("RecPlayer", Log::DEBUG, "Client asked for some data past the end of recording, adjusting amount");
    amount = totalLength - position;
  }

  // work out what block position is in
  int segmentNumber;
  for(segmentNumber = 1; segmentNumber < 1000; segmentNumber++)
  {
    if ((position >= segments[segmentNumber]->start) && (position < segments[segmentNumber]->end)) break;
    // position is in this block
  }

  // we could be seeking around
  if (segmentNumber != fileOpen)
  {
    if (!openFile(segmentNumber)) return 0;
  }

  uint64_t currentPosition = position;
  uint32_t yetToGet = amount;
  uint32_t got = 0;
  uint32_t getFromThisSegment = 0;
  uint32_t filePosition;

  while(got < amount)
  {
    if (got)
    {
      // if(got) then we have already got some and we are back around
      // advance the file pointer to the next file
      if (!openFile(++segmentNumber)) return 0;
    }

    // is the request completely in this block?
    if ((currentPosition + yetToGet) <= segments[segmentNumber]->end)
      getFromThisSegment = yetToGet;
    else
      getFromThisSegment = segments[segmentNumber]->end - currentPosition;

    filePosition = currentPosition - segments[segmentNumber]->start;
    fseek(file, filePosition, SEEK_SET);
    if (fread(&buffer[got], getFromThisSegment, 1, file) != 1) return 0; // umm, big problem.

    // Tell linux not to bother keeping the data in the FS cache
    posix_fadvise(file->_fileno, filePosition, getFromThisSegment, POSIX_FADV_DONTNEED);

    got += getFromThisSegment;
    currentPosition += getFromThisSegment;
    yetToGet -= getFromThisSegment;
  }

  lastPosition = position;
  return got;
}

uint64_t RecPlayer::getLastPosition()
{
  return lastPosition;
}

cRecording* RecPlayer::getCurrentRecording()
{
  return recording;
}

uint64_t RecPlayer::positionFromFrameNumber(uint32_t frameNumber)
{
  if (!indexFile) return 0;

#if VDRVERSNUM >= 10703
  uint16_t retFileNumber;
  off_t retFileOffset;
#else
  uchar retFileNumber;
  int retFileOffset;
#endif

  if (!indexFile->Get((int)frameNumber, &retFileNumber, &retFileOffset))
  {
    return 0;
  }

//  log->log("RecPlayer", Log::DEBUG, "FN: %u FO: %i", retFileNumber, retFileOffset);
  if (!segments[retFileNumber]) return 0;
  uint64_t position = segments[retFileNumber]->start + retFileOffset;
//  log->log("RecPlayer", Log::DEBUG, "Pos: %llu", position);

  return position;
}

uint32_t RecPlayer::frameNumberFromPosition(uint64_t position)
{
  if (!indexFile) return 0;

  if (position >= totalLength)
  {
    //log->log("RecPlayer", Log::DEBUG, "Client asked for data starting past end of recording!");
    return 0;
  }

  uint8_t segmentNumber;
  for(segmentNumber = 1; segmentNumber < 255; segmentNumber++)
  {
    if ((position >= segments[segmentNumber]->start) && (position < segments[segmentNumber]->end)) break;
    // position is in this block
  }
  uint32_t askposition = position - segments[segmentNumber]->start;
  return indexFile->Get((int)segmentNumber, askposition);

}


bool RecPlayer::getNextIFrame(uint32_t frameNumber, uint32_t direction, uint64_t* rfilePosition, uint32_t* rframeNumber, uint32_t* rframeLength)
{
  // 0 = backwards
  // 1 = forwards

  if (!indexFile) return false;

  int iframeLength;
  int indexReturnFrameNumber;

  indexReturnFrameNumber = (uint32_t)indexFile->GetNextIFrame(frameNumber, (direction==1 ? true : false), NULL, NULL, &iframeLength);
  //log->log("RecPlayer", Log::DEBUG, "GNIF input framenumber:%lu, direction=%lu, output:framenumber=%i, framelength=%i", frameNumber, direction, indexReturnFrameNumber, iframeLength);

  if (indexReturnFrameNumber == -1) return false;

  *rfilePosition = positionFromFrameNumber(indexReturnFrameNumber);
  *rframeNumber = (uint32_t)indexReturnFrameNumber;
  *rframeLength = (uint32_t)iframeLength;

  return true;
}
