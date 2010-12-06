/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2005 Stefan Huelswitt <s.huelswitt@gmx.de>
 * (C) 2009 Andreas Koch <andreas@open7x0.org> 
 *
 * Andreas: Rewrite for open7x0 hw nearly the complete file
 * update to gpl version 3 (using option of gpl version 2 license text)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <iconv.h>

#include <vdr/i18n.h>
#include <vdr/config.h>

#include "common.h"
#include "data-mp3.h"
#include "decoder-mp3.h"
#include "stream.h"

// --- cID3Reader --------------------------------------------------------------
#define ID3_MAX_SEARCH_TAG (256)
#define ID3_FLAG_FOOTER (1 << 4)
#define ID3_FLAG_EXT_HEADER (1 << 6)
#define ID3_FLAG_COMPRESSED_V22 (1 << 6)
#define ID3_FLAG_UNSYNC (1 << 7)

#define ID3_EXT_FLAG_UPDATE (1 << 6)

#define ID3_FRAME_FLAG_COMP_V23 (1 << 7)
#define ID3_FRAME_FLAG_ENC_V23 (1 << 6)
#define ID3_FRAME_FLAG_GROUP_V23 (1 << 5)

#define ID3_FRAME_FLAG_GROUP (1 << 6)
#define ID3_FRAME_FLAG_COMP (1 << 3)
#define ID3_FRAME_FLAG_ENC (1 << 2)
#define ID3_FRAME_FLAG_UNSYNC (1 << 1)
#define ID3_FRAME_FLAG_DATA_LEN (1 << 0)

#define ID3_FRAME_TITLE        "TIT2"
#define ID3_FRAME_TITLE_V22    "TT2"
#define ID3_FRAME_ARTIST       "TPE1"
#define ID3_FRAME_ARTIST_V22   "TP1"
#define ID3_FRAME_ALBUM        "TALB"
#define ID3_FRAME_ALBUM_V22    "TAL"
#define ID3_FRAME_TRACK        "TRCK"
#define ID3_FRAME_TRACK_V22    "TRK"
#define ID3_FRAME_YEAR         "TDRC"
#define ID3_FRAME_YEAR_V23     "TYER"
#define ID3_FRAME_YEAR_V22     "TYE"
#define ID3_FRAME_GENRE        "TCON"
#define ID3_FRAME_GENRE_V22    "TCO"
#define ID3_FRAME_LENGTH       "TLEN"
#define ID3_FRAME_LENGTH_V22   "TLE"

#define ID3_FRAME_PIC           "APIC"
#define ID3_FRAME_PIC_V22       "PIC"
#define ID3_FRAME_AUDIO_INDEX   "ASPI"
#define ID3_FRAME_SEEK          "SEEK"
#define ID3_FRAME_MPA_INDEX     "MLLT"
#define ID3_FRAME_MPA_INDEX_V22 "MLL"

#define ID3_KNOWN_FRAME_COUNT 11

struct mp3_vbr_header {
   uint32_t frame_count;
   uint32_t data_start;
   uint32_t data_size;
   uint32_t frames_per_toc;
   uint32_t valid_toc_entries;
   uint32_t *toc;
};

class cID3Reader {
private:
  struct s_frame_decoder {
    const char *frame_id;
    const char *frame_id_v23;
    const char *frame_id_v22;
    bool (cID3Reader::*decoder)(const uint8_t *buf, int len, void *cID3Reader::*arg);
    void* cID3Reader::*arg;
    };
  static const s_frame_decoder frame_decoder[ID3_KNOWN_FRAME_COUNT];
  static const char *id3v1_genre[256];

  cStream *str;
  const uint8_t *cur_buffer;
  uint32_t cur_length;
  int search_skipped;

  char cur_frame_id[4];
  int32_t cur_frame_size;
  int32_t cur_frame_data_size;
  uint8_t frame_header_buf[16];
  const uint8_t *cur_frame_header;
  int32_t cur_frame_header_size;
  bool cur_frame_unsynced;
  bool cur_frame_encrypted;
  bool cur_frame_compressed;
  bool cur_frame_group_id;
  bool cur_frame_data_length;

  int version;
  int64_t tag_offset;
  int32_t tag_size;
  int flags;
  int32_t tag_read;
  bool error;
  int64_t data_offset;
  int64_t data_end_offset;

  char *title;
  char *artist;
  char *album;
  char *track;
  char *year;
  char *genre;
  char *audio_length;
  uint32_t audio_length_ms;
  uint32_t seek_offset;
  bool seek_frame_avail;

  mp3_vbr_header vbr_header;
  bool has_mllt_frame;
  uint8_t *iframe_pic;
  int iframe_pic_length;

  bool decode_text_frame(const uint8_t *buf, int len, void*cID3Reader::*arg);
  bool decode_length_frame(const uint8_t *buf, int len, void*cID3Reader::*arg);
  bool decode_pic_frame(const uint8_t *buf, int len, void* cID3Reader::*arg);
  bool decode_audio_index_frame(const uint8_t *buf, int len, void* cID3Reader::*arg);
  bool decode_seek_frame(const uint8_t *buf, int len, void* cID3Reader::*arg);
  bool decode_mpa_index_frame(const uint8_t *buf, int len, void* cID3Reader::*arg);

  int readIDv1Tag(void);
  void Clear(void);
  int charset_conv(const char *from, const char *to, char **dest, const char *src,
      int slen);
  int decode_text(const uint8_t *buf, int len, char **dest);
  int32_t get_bits(const uint8_t *&buf, int &bits_used, int bits, bool rsigned);
  inline int32_t decode_syncsafe_int(const uint8_t *buf);
  bool deunsync(int32_t &desync_count, int32_t unsync_count, uint8_t *buf);
  int FindTag(void);
  int SeekBackToHeader(void);
  int ParseHeader(void);
  void ResetFrameHeader(void);
  int ReadFrameHeader(void);
  void SetupFrameFlags(void);
  int ReadFrameHeaderAdd(void);
  int ParseFrameHeader(void);
  bool FrameInplaceReadable(bool &readable);
  uint8_t *ReadFrame(int32_t &size);
  bool SkipFrame(void);
  bool ParseFrame(void);
  int SearchTagHeader(bool search);
  bool CheckFrameHeaderID(void);

public:
  cID3Reader(void);
  ~cID3Reader(void);

  const char *getTitle(void) const { return title; }
  const char *getArtist(void) const { return artist; }
  const char *getAlbum(void) const { return album; }
  const char *getTrack(void) const { return track; }
  const char *getYear(void) const { return year; }
  const char *getGenre(void) const { return genre; }
  const uint8_t *getIFramePic(int &length) const {
     length = iframe_pic_length; return iframe_pic; }
  const uint64_t getDataOffset(void) { return data_offset; }
  const uint64_t getDataEndOffset(void) { return data_end_offset; }
  const mp3_vbr_header *getVbrHeader(int samples_per_frame, int sample_rate);
  int ReadID3(cStream *Str, const uint8_t *&buffer, uint32_t &length, bool search);
  int SkipID3Tag(cStream *Str, const uint8_t *&buffer, uint32_t &length);
  };

const cID3Reader::s_frame_decoder
  cID3Reader::frame_decoder[ID3_KNOWN_FRAME_COUNT] = {
  { ID3_FRAME_TITLE, ID3_FRAME_TITLE, ID3_FRAME_TITLE_V22,
    &cID3Reader::decode_text_frame, (void* cID3Reader::*) &cID3Reader::title },
  { ID3_FRAME_ARTIST, ID3_FRAME_ARTIST, ID3_FRAME_ARTIST_V22,
    &cID3Reader::decode_text_frame, (void* cID3Reader::*) &cID3Reader::artist },
  { ID3_FRAME_ALBUM, ID3_FRAME_ALBUM, ID3_FRAME_ALBUM_V22,
    &cID3Reader::decode_text_frame, (void* cID3Reader::*) &cID3Reader::album },
  { ID3_FRAME_TRACK, ID3_FRAME_TRACK, ID3_FRAME_TRACK_V22,
    &cID3Reader::decode_text_frame, (void* cID3Reader::*) &cID3Reader::track },
  { ID3_FRAME_YEAR, ID3_FRAME_YEAR_V23, ID3_FRAME_YEAR_V22,
    &cID3Reader::decode_text_frame, (void* cID3Reader::*) &cID3Reader::year },
  { ID3_FRAME_GENRE, ID3_FRAME_GENRE, ID3_FRAME_GENRE_V22,
    &cID3Reader::decode_text_frame, (void* cID3Reader::*) &cID3Reader::genre },
  { ID3_FRAME_LENGTH, ID3_FRAME_LENGTH, ID3_FRAME_LENGTH_V22,
    &cID3Reader::decode_length_frame, NULL },
  { ID3_FRAME_PIC, ID3_FRAME_PIC, ID3_FRAME_PIC_V22,
    &cID3Reader::decode_pic_frame, NULL },
  { ID3_FRAME_AUDIO_INDEX, ID3_FRAME_AUDIO_INDEX, "   ",
    &cID3Reader::decode_audio_index_frame,  NULL},
  { ID3_FRAME_SEEK, ID3_FRAME_SEEK, "   ",
    &cID3Reader::decode_seek_frame, NULL },
  { ID3_FRAME_MPA_INDEX, ID3_FRAME_MPA_INDEX, ID3_FRAME_MPA_INDEX_V22,
    &cID3Reader::decode_mpa_index_frame, NULL }
 };

const char *cID3Reader::id3v1_genre[256] = {
  "Blues", "Classic Rock", "Country", "Dance",
  "Disco", "Funk",  "Grunge", "Hip-Hop",
  "Jazz", "Metal", "New Age", "Oldies",
  "Other", "Pop", "R&B", "Rap",
  "Reggae", "Rock", "Techno", "Industrial",
  "Alternative", "Ska", "Death Metal", "Pranks",
  "Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop",
  "Vocal", "Jazz&Funk", "Fusion", "Trance",
  "Classical", "Instrumental", "Acid",  "House",
  "Game", "Sound Clip", "Gospel" , "Noise",
  "AlternRock", "Bass", "Soul", "Punk",
  "Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
  "Ethnic", "Gothic", "Darkwave", "Techno-Industrial",
  "Electronic", "Pop-Folk", "Eurodance", "Dream",
  "Southern Rock", "Comedy", "Cult", "Gangsta",
  "Top 40", "Christian Rap", "Pop/Funk", "Jungle",
  "Native American", "Cabaret", "New Wave", "Psychadelic",
  "Rave", "Showtunes", "Trailer", "Lo-Fi",
  "Tribal", "Acid Punk", "Acid Jazz", "Polka",
  "Retro", "Musical",  "Rock & Roll", "Hard Rock",
  "Folk", "Folk-Rock", "National Folk", "Swing",
  "Fast Fusion", "Bebob", "Latin", "Revival",
  "Celtic", "Bluegrass",  "Avantgarde", "Gothic Rock",
  "Progressive Rock", "Psychedelic Rock", "Symphonic Rock", "Slow Rock",
  "Big Band", "Chorus", "Easy Listening", "Acoustic",
  "Humour", "Speech", "Chanson", "Opera",
  "Chamber Music", "Sonata", "Symphony", "Booty Bass",
  "Primus", "Porn Groove", "Satire", "Slow Jam",
  "Club", "Tango", "Samba", "Folklore",
  "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle",
  "Duet", "Punk Rock", "Drum Solo", "A capella",
  "Euro-House", "Dance Hall", "Goa", "Drum'n'Bass",
  "Club-House", "Hardcore", "Terror", "Indie",
  "BritPop", "Negerpunk", "Polsk Punk", "Beat",
  "Christian Gangsta", "Heavy Metal", "Black Metal", "Crossover",
  "Contemporary Christian", "Christian Rock", "Merengue", "Salsa",
  "Thrash Metal", "Anime", "JPop", "SynthPop",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown",
  "unknown", "unknown", "unknown", "unknown"
};

void cID3Reader::Clear(void)
{
  free(title);
  title = NULL;
  free(artist);
  artist = NULL;
  free(album);
  album = NULL;
  free(track);
  track = NULL;
  free(year);
  year = NULL;
  free(genre);
  genre = NULL;
  free(audio_length);
  audio_length = NULL;
  audio_length_ms = 0;

  delete[] vbr_header.toc;
  memset(&vbr_header, 0, sizeof(vbr_header));

  has_mllt_frame = false;
  free(iframe_pic);
  iframe_pic = NULL;
  iframe_pic_length = 0;
}

cID3Reader::cID3Reader(void)
{
  version = 0;
  title = NULL;
  artist = NULL;
  album = NULL;
  track = NULL;
  year = NULL;
  genre = NULL;
  audio_length = NULL;
  audio_length_ms = 0;

  memset(&vbr_header, 0, sizeof(vbr_header));

  has_mllt_frame = false;
  iframe_pic = NULL;
  iframe_pic_length = 0;

  str = NULL;
  cur_buffer = NULL;
  cur_length = 0;
  tag_offset = 0;
  tag_size = 0;
  data_offset = 0;
  data_end_offset = 0;
}

cID3Reader::~cID3Reader()
{
  Clear();
}

int cID3Reader::readIDv1Tag(void)
{
  if (unlikely(!str->StreamSafer(cur_buffer, cur_length, cur_buffer, 128))) {
     esyslog("MP3: cannot fully read ID3v1 Tag");
     return -1;
     }

  free(title);
  title = MALLOC(char, 31);
  if (!title) {
     esyslog("MP3: no memory");
     return -1;
     }
  memcpy(title, cur_buffer + 3, 30);
  title[30] = 0;
  stripspace(title);

  free(artist);
  artist = MALLOC(char, 31);
  if (!artist) {
     esyslog("MP3: no memory");
     return -1;
     }
  memcpy(artist, cur_buffer + 33, 30);
  artist[30] = 0;
  stripspace(artist);

  free(album);
  album = MALLOC(char, 31);
  if (!album) {
     esyslog("MP3: no memory");
     return -1;
     }
  memcpy(album, cur_buffer + 63, 30);
  album[30] = 0;
  stripspace(album);

  free(year);
  year = MALLOC(char, 5);
  if (!album) {
     esyslog("MP3: no memory");
     return -1;
     }
  memcpy(year, cur_buffer + 93, 4);
  year[4] = 0;

  free(genre);
  genre = strdup(id3v1_genre[cur_buffer[127]]);
  return 1;
}

int cID3Reader::charset_conv(const char *from, const char *to, char **dest,
 const char *src, int slen)
{
  free(*dest);
  *dest = MALLOC(char, slen + 1);
  char *p = *dest;
  if (p == NULL) {
     esyslog("MP3: No Memory for ID3v2 text");
     return -1;
     }

  p[slen] = 0;
  if (strcasecmp(from, to) == 0) {
     memcpy(p, src, slen);
     return slen;
     }

  char *_src = (char *) src;
  size_t ibytes = slen;
  size_t obytes = slen;
  iconv_t icd = iconv_open(to, from);
  if (icd == (iconv_t)-1) {
     esyslog("MP3: Cannot convert charset of ID3v2 text (init iconv)");
     goto out_free;
     }

  if (iconv(icd, &_src, &ibytes, &p, &obytes) == (size_t)-1) {
     esyslog("MP3: Cannot convert charset of ID3v2 text (iconv)");
     goto out_close;
     }

  if (ibytes != 0) {
     esyslog("MP3: Warning not all chars of ID3v2 text are converted");
     }

  iconv_close(icd);
  return slen - obytes;
out_close:
  iconv_close(icd);
out_free:
  free(*dest);
  *dest = NULL;
  return -1;
}

int cID3Reader::decode_text(const uint8_t *buf, int len, char **dest)
{
  if (len < 2)
     return -1;

  static const char *csets[4] = {"ISO-8859-1", "UTF-16", "UTF-16BE", "UTF-8"};

  unsigned int enc = buf[0];
  buf++; len--;
  if (enc > 3) {
     buf--; len++; enc = 0;
     esyslog("MP3: unknown text encoding in ID3v2 text");
     }

  const char *fromcode = csets[enc];

  const char *vdrcode = I18nCharSets()[Setup.OSDLanguage];
  int vdrcode_len = strlen(vdrcode);
  char tocode[vdrcode_len + 2];
  memcpy(tocode, vdrcode, vdrcode_len + 1);

  if (strncasecmp(tocode, "ISO", 3) == 0) {
     //Fix codeset-naming for uclibc
     if ((tocode[vdrcode_len - 2] == '1') &
         (tocode[vdrcode_len - 1] == '5'))
        // ISO-8859-15 and ISO-8859-1 are nearly equal
        tocode[vdrcode_len - 1] = 0;

     memmove(tocode + 4,tocode + 3,vdrcode_len - 3 + 1);
     memcpy(tocode, "ISO-",4);
     }

  return charset_conv(fromcode, tocode, dest, (const char *)buf, len);
}

bool cID3Reader::decode_text_frame(const uint8_t *buf, int len, void *cID3Reader::*arg)
{
  char **dest = (char **)&(this->*arg);
  int dest_len = decode_text(buf, len, dest);
  if (dest_len <= 0)
     return false;

  /* Handle multiple Strings (should be rare)
   * Replace '\0' by ';'
   */
  dest_len--;
  char *p = *dest;
  while (version == 0x204 && (p = (char *)memchr(p, 0, dest_len - (p - *dest)))) {
        p[0] = ';';
        }
  (*dest)[dest_len + 1] = 0;
  return true;
}

bool cID3Reader::decode_length_frame(const uint8_t *buf, int len, void *cID3Reader::*arg)
{
  int dest_len = decode_text(buf, len, &audio_length);
  if (dest_len <= 0)
     return false;
  char *end;
  audio_length_ms = strtoul(audio_length, &end, 0);
  if (end[0]) {
     esyslog("MP3: Wrong ID3v2 TLEN frame content");
     audio_length_ms = 0;
     return false;
     }
  return true;
}

bool cID3Reader::decode_pic_frame(const uint8_t *buf, int len, void *cID3Reader::*arg)
{
  if (len < 6) {
     esyslog("MP3: ID3v2 APIC frame too short");
     return false;
     }

  int enc = buf[0];
  int pic_format_length = 3;
  const char *pic_format = (const char *)buf + 1;
  if (version != 0x202)
     pic_format_length = strnlen(pic_format, len);

  const char *p = (const char *)memchr(pic_format, '/', pic_format_length);
  buf += pic_format_length + 1;
  len -= pic_format_length + 1;
  if (p) {
     pic_format = p + 1;
     pic_format_length = (const char *)buf - pic_format;
     }

  if (strncasecmp(pic_format, "MPV", pic_format_length) != 0) {
     dsyslog("MP3: found ID3v2 image. Format '%s' not supported", pic_format);
     return true;
     }

  if (len < 2) {
     esyslog("MP3: ID3v2 APIC frame too short");
     return false;
     }

  if ((buf[0] == 1) | (buf[0] == 2)) {
     dsyslog("MP3: found ID3v2 image. Icons not supported");
     return true;
     }

  /* Skip Description */
  bool two_zero = (enc == 1) | (enc == 2);
  p = (const char *)buf + 1;
  while ((p = (const char *)memchr(p, 0, len - (p - (const char *)buf) - two_zero)) &&
         two_zero && p[1] != 0)
        p++;

  if (!p) {
     esyslog("MP3: ID3v2 APIC frame description text not terminated.");
     return false;
     }

  len -= (p - (const char *)buf) + 1 + two_zero;
  buf = (const uint8_t *)p + 1 +  two_zero;

  if (len <= 0) {
     esyslog("MP3: ID3v2 APIC frame with no image data.");
     return false;
     }

  free(iframe_pic);
  iframe_pic = MALLOC(uint8_t, len);
  if (!iframe_pic) {
     esyslog("MP3: ID3v2 APIC frame no memory");
     return false;
     }

  memcpy(iframe_pic, buf, len);
  iframe_pic_length = len;
  return true;
}

bool cID3Reader::decode_audio_index_frame(const uint8_t *buf, int len, void *cID3Reader::*arg)
{
  if (len < 11) {
     esyslog("MP3: ID3v2 ASPI frame too short.");
     return false;
     }

  // MLLT should be used before this frame
  if (has_mllt_frame)
     return true;

  delete[] vbr_header.toc;
  memset(&vbr_header, 0, sizeof(vbr_header));

  vbr_header.data_start = (buf[0] << 24) | (buf[1] << 16) |
                             (buf[2] << 8) | buf[3];
  vbr_header.data_size = (buf[4] << 24) | (buf[5] << 16) |
                            (buf[6] << 8) | buf[7];
  vbr_header.valid_toc_entries = (buf[8] << 8) | buf[9];
  int index_bits = buf[10];
  buf += 11; len -= 11;

  if ((unsigned int) len < ((index_bits * vbr_header.valid_toc_entries + 7) / 8)) {
     esyslog("MP3: ID3v2 ASPI frame too short for indexes.");
     memset(&vbr_header, 0, sizeof(vbr_header));
     return false;
     }

  vbr_header.toc = new uint32_t[vbr_header.valid_toc_entries];
  if (!vbr_header.toc) {
     esyslog("MP3: ID3v2 ASPI frame no memory.");
     memset(&vbr_header, 0, sizeof(vbr_header));
     return false;
     }

  int bits_used = 0;
  int adder = (1 << index_bits) - 1;
  for (unsigned int i=0; i < vbr_header.valid_toc_entries; i++) {
      uint64_t fi = get_bits(buf, bits_used, index_bits, false);
      fi *= vbr_header.data_size;
      fi += adder;
      vbr_header.toc[i] = vbr_header.data_start + (fi >> index_bits);
      }
  return true;
}

bool cID3Reader::decode_seek_frame(const uint8_t *buf, int len, void *cID3Reader::*arg)
{
  if (len <= 4) {
     esyslog("MP3: ID3v2 SEEK frame too short.");
     return false;
     }
  seek_frame_avail = true;
  seek_offset = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
  return true;
}

bool cID3Reader::decode_mpa_index_frame(const uint8_t *buf, int len, void *cID3Reader::*arg)
{
  dsyslog("MP3: ID3v2 MLLT Frame found. Spec is unclear for this frame. "
          "I haven't found a file using this. "
          "Please contact me if this is working (or not).");

  if (len < (2 + 3 + 3 + 1 + 1)) {
     esyslog("MP3: ID3v2 MLLT frame too short.");
     return false;
     }

  delete[] vbr_header.toc;
  memset(&vbr_header, 0, sizeof(vbr_header));

  vbr_header.frames_per_toc = (buf[0] << 8) | buf[1];
  int32_t bytes_diff = (buf[2] << 16) | (buf[3] << 8) | buf[4];
  int32_t byte_ref_bits = buf[8];
  int32_t ms_ref_bits = buf[9];
  buf += 10; len -= 10;

  vbr_header.valid_toc_entries = ((len * 8) / (byte_ref_bits + ms_ref_bits)) + 1;
  vbr_header.frame_count = vbr_header.valid_toc_entries *
        vbr_header.frames_per_toc;

  vbr_header.toc = new uint32_t[vbr_header.valid_toc_entries];
  if (!vbr_header.toc) {
     esyslog("MP3: ID3v2 MLLT frame no memory.");
     memset(&vbr_header, 0, sizeof(vbr_header));
     has_mllt_frame = false;
     return false;
     }

  vbr_header.toc[0] = 0;

  unsigned int i;
  int bits_used = 0;
  for (i = 1; i < vbr_header.valid_toc_entries; i++) {
      int32_t bytes_ref = get_bits(buf, bits_used, byte_ref_bits, true);
      get_bits(buf, bits_used, ms_ref_bits, false);
      vbr_header.toc[i] = i * bytes_diff - bytes_ref;
      }
  has_mllt_frame = true;
  return true;
}

const mp3_vbr_header *cID3Reader::getVbrHeader(int samples_per_frame, int sample_rate)
{
  if ((vbr_header.valid_toc_entries == 0) |
       ((vbr_header.frame_count == 0) & (audio_length_ms == 0)))
     return NULL;

  if (vbr_header.data_start == 0) {
     vbr_header.data_start = data_offset;
     vbr_header.data_size = data_end_offset - data_offset;
     for (unsigned int i = 0; i < vbr_header.valid_toc_entries; i++)
         vbr_header.toc[i] += data_offset;
     }

  if (vbr_header.frame_count == 0) {
     samples_per_frame *= 1000;
     uint64_t samples1000 = (uint64_t)audio_length_ms * (uint64_t)sample_rate +
                            (samples_per_frame  - 1);
     vbr_header.frame_count = samples1000 / samples_per_frame;
     vbr_header.frames_per_toc = vbr_header.frame_count /
                                vbr_header.valid_toc_entries;
     }
  return &vbr_header;
}

inline int32_t cID3Reader::get_bits(const uint8_t *&buf, int &bits_used,
   int bits, bool rsigned)
{
  if (bits <= 0)
     return 0;

  int32_t ret = 0;
  int real_bits = bits;
  for (bits += bits_used; bits >= 8 ; bits -= 8) {
      ret <<= 8;
      ret |= buf[0];
      buf++;
      }

  ret <<= bits;
  ret |= (buf[0] >> (8 - bits));
  bits_used = bits;
  uint32_t mask = ((int64_t)1 << real_bits) - 1;
  ret &= mask;

  real_bits--;
  if ((!rsigned) | ((ret & (1 << real_bits)) == 0) | (real_bits >= 31))
     return ret;
  mask = (1 << real_bits) -1;
  return (-1 & ~mask) | (ret & mask);
}

inline int32_t cID3Reader::decode_syncsafe_int(const uint8_t *buf)
{
   int32_t r;
   r  = buf[0]; r <<= 7;
   r |= buf[1]; r <<= 7;
   r |= buf[2]; r <<= 7;
   r |= buf[3];
   return r;
}

bool cID3Reader::deunsync(int32_t &desync_count, int32_t unsync_count,
  uint8_t *buf)
{
  int32_t desynced = 0;
  int32_t min_need = 1;

  while ((desynced < desync_count) | (unsync_count > 0)) {
        if (!str->StreamSafer(cur_buffer, cur_length, cur_buffer, min_need)) {
           esyslog("MP3: Cannot unsync id3 data");
           desync_count = desynced;
           return false;
           }

        int32_t s = max(desync_count - desynced, unsync_count);
        s = min(s, (int32_t)cur_length);

        int32_t copy = s;
        const char *p = (const char *)memchr(cur_buffer, 0xFF, s);
        min_need = 1;
        if (p) {
           s = copy = p - (const char *)cur_buffer + 1;
           if ((s < unsync_count) | (unsync_count <= 0)) {
              if ((int32_t)cur_length < s + 1) {
                 s--;
                 copy--;
                 min_need = 2;
                 }
              else if(p[1] == 0)
                 s++;
              }
           }

        if (buf) {
           memcpy(buf, cur_buffer, copy);
           buf += copy;
           }

        desynced += copy;
        tag_read += s;
        cur_buffer += s;
        cur_length -= s;
        unsync_count -= s;
        }
  desync_count = desynced;
  return true;
}

int cID3Reader::FindTag(void)
{
   const uint8_t *buf = cur_buffer;
   const uint8_t *end = buf + cur_length - 10;

   if (cur_length > ID3_MAX_SEARCH_TAG)
      end = buf + ID3_MAX_SEARCH_TAG - 10;

   int found = 0;
   while (buf <= end) {
         if ((buf[0] == 'T') & (buf[1] == 'A') & (buf[2] == 'G')) {
            version = 0x100;
            tag_size = 128;
            if ((!str->Filesize) | (str->Filesize - str->Tell(buf) >= 128)) {
               found = 1;
               break;
               }
            }
         if ((buf[0] == 'I') & (buf[1] == 'D') & (buf[2] == '3')) {
            version = 0x200 | buf[3];
            tag_size = decode_syncsafe_int(buf + 6);
            int64_t full_size = tag_size + 10 + 
                            ((flags & ID3_FLAG_FOOTER) != 0) * 10;
            flags = buf[5];
            bool ok = (version == 0x202) | (version == 0x203) | 
                      (version == 0x204);
            ok &= (!str->Filesize) | 
                  ((int64_t)str->Filesize - str->Tell(buf) >=  full_size);
            if (ok) {
               found = 1;
               break;
               }
            }
         if ((buf[0] == '3') & (buf[1] == 'D') & (buf[2] == 'I')) {
            version = 0x200 | buf[3];
            tag_size = decode_syncsafe_int(buf + 6);
            flags = buf[5];
            bool ok = (version == 0x202) | (version == 0x203) | 
                      (version == 0x204);
            ok &= (str->Tell(buf) >= tag_size + 10);
            ok &= ((flags & ID3_FLAG_FOOTER) != 0);
            if (ok) {
               found = 2;
               break;
               }
            }
         buf++;
         }
   search_skipped += buf - cur_buffer - 1;
   if (found) {
      cur_length -= buf - cur_buffer;
      cur_buffer = buf;
      }
   return found;
}

int cID3Reader::SeekBackToHeader(void)
{
  search_skipped = 0;
  if (unlikely(!str->Seek(str->Tell(cur_buffer) - tag_size - 10))) {
     esyslog("MP3: cannot seek to ID3v2 Tag header from footer");
     return -1;
     }

  if (unlikely(!str->StreamSafer(cur_buffer, cur_length, NULL, 10))) {
     esyslog("MP3: cannot fully read ID3v2 Tag header");
     return -1;
     }

  const uint8_t *buf_safe = cur_buffer;
  int tagFound = FindTag();
  if (unlikely((tagFound != 1) | ((version & 0xF00) != 0x200) | (buf_safe != cur_buffer))) {
     esyslog("MP3: corrupt ID3v2 Tag found");
     return 0;
     }
  tag_offset = str->Tell(cur_buffer);
  return 1;
}

int cID3Reader::ParseHeader(void)
{
  int tagFound = FindTag();
  if (!tagFound)
     return 0;

  tag_offset = str->Tell(cur_buffer);
  if (version == 0x100)
     return 1;

  int r;
  if (unlikely(tagFound == 2 && (r = SeekBackToHeader()) != 1))
     return r;

  cur_buffer += 10;
  cur_length -= 10;
  tag_read = 0;

  if ((version == 0x202) | (!(flags & ID3_FLAG_EXT_HEADER))) {
     Clear();
     return 1;
     }

  /* extended header only needed for update flag */
  if (!str->StreamSafer(cur_buffer, cur_length, cur_buffer, 6)) {
     esyslog("MP3: cannot fully read ID3v2 extended tag header");
     return -1;
     }

  int32_t ext_head_size = decode_syncsafe_int(cur_buffer);
  if (!(cur_buffer[5] & ID3_EXT_FLAG_UPDATE))
     Clear();

  if (version == 0x203) {
     ext_head_size += 4;
     if (flags & ID3_FLAG_UNSYNC) {
        if (deunsync(ext_head_size, 0, NULL))
           return 1;
        esyslog("MP3: cannot skip over ID3v2 extended tag header");
        return -1;
        }
     }

  if (!str->Skip(cur_buffer, cur_length, cur_buffer, ext_head_size)) {
     esyslog("MP3: cannot skip over ID3v2 extended tag header");
     return -1;
     }

  tag_read = ext_head_size;
  return 1;
}

void cID3Reader::ResetFrameHeader(void)
{
  cur_frame_data_size = -1;
  cur_frame_unsynced = false;
  cur_frame_encrypted = false;
  cur_frame_compressed = false;
  cur_frame_group_id = false;
  cur_frame_data_length = false;
  cur_frame_header = NULL;
  cur_frame_header_size = 0;
}

bool cID3Reader::CheckFrameHeaderID(void)
{
  return ((((cur_buffer[0] >= 'A') & (cur_buffer[0] <= 'Z')) |
          ((cur_buffer[0] >= '0') & (cur_buffer[0] <= '9'))) &
         (((cur_buffer[1] >= 'A') & (cur_buffer[1] <= 'Z')) |
          ((cur_buffer[1] >= '0') & (cur_buffer[1] <= '9'))) &
         (((cur_buffer[2] >= 'A') & (cur_buffer[2] <= 'Z')) |
          ((cur_buffer[2] >= '0') & (cur_buffer[2] <= '9'))) &
         (((cur_buffer[3] >= 'A') & (cur_buffer[3] <= 'Z')) |
          ((cur_buffer[3] >= '0') & (cur_buffer[3] <= '9')) |
          (version == 0x202)));
}

int cID3Reader::ReadFrameHeader(void)
{
  cur_frame_header_size = 6;
  if (version > 0x202)
     cur_frame_header_size = 10;

  if (tag_size - tag_read < cur_frame_header_size)
     return 0;

  if (!str->StreamSafer(cur_buffer, cur_length, cur_buffer,
        cur_frame_header_size)) {
     esyslog("MP3: cannot fully read next ID3v2 frame header");
     return -1;
     }

  if (!CheckFrameHeaderID())
     return 0;

  cur_frame_unsynced = (flags & ID3_FLAG_UNSYNC) != 0;
  if ((version != 0x204) & (cur_frame_unsynced)) {
     cur_frame_header = frame_header_buf;
     if (!deunsync(cur_frame_header_size, 0, frame_header_buf))
        return -1;
     }
  else {
     cur_frame_header = cur_buffer;
     cur_buffer += cur_frame_header_size;
     cur_length -= cur_frame_header_size;
     tag_read += cur_frame_header_size;
     }
  return 1;
}

void cID3Reader::SetupFrameFlags(void)
{
  memcpy(cur_frame_id, cur_frame_header, 4);

  cur_frame_size = 0;
  switch (version) {
    case 0x202:
         cur_frame_size = (cur_frame_header[3] << 16) |
                          (cur_frame_header[4] << 8) | cur_frame_header[5];
         break;
    case 0x203:
         cur_frame_size = (cur_frame_header[4] << 24) |
                          (cur_frame_header[5] << 16) |
                          (cur_frame_header[6] << 8) | cur_frame_header[7];
         cur_frame_data_length = cur_frame_compressed =
                  (cur_frame_header[9] & ID3_FRAME_FLAG_COMP_V23) != 0;
         cur_frame_encrypted = (cur_frame_header[9] & ID3_FRAME_FLAG_ENC_V23) != 0;
         cur_frame_group_id = (cur_frame_header[9] & ID3_FRAME_FLAG_GROUP_V23) != 0;
         break;
    case 0x204:
         cur_frame_size = decode_syncsafe_int(cur_frame_header + 4);
         cur_frame_group_id = (cur_frame_header[9] & ID3_FRAME_FLAG_GROUP) != 0;
         cur_frame_compressed = (cur_frame_header[9] & ID3_FRAME_FLAG_COMP) != 0;
         cur_frame_encrypted = (cur_frame_header[9] & ID3_FRAME_FLAG_ENC) != 0;
         cur_frame_unsynced = (cur_frame_header[9] & ID3_FRAME_FLAG_UNSYNC) != 0;
         cur_frame_data_length =(cur_frame_header[9] & ID3_FRAME_FLAG_DATA_LEN) != 0;
         break;
    }
}

int cID3Reader::ReadFrameHeaderAdd(void)
{
  int length = (cur_frame_group_id * 1) + (cur_frame_encrypted * 1) +
               (cur_frame_data_length * 4);
  if (length == 0)
     return 1;

  if (!str->StreamSafer(cur_buffer, cur_length, cur_buffer, length)) {
     esyslog("MP3: cannot fully read next ID3v2 frame header (append)");
     return -1;
     }

  const uint8_t *cur_hbuf;
  if ((version != 0x204) & (cur_frame_unsynced)) {
     cur_hbuf = frame_header_buf + cur_frame_header_size;
     if (!deunsync(length, 0, frame_header_buf + cur_frame_header_size))
        return -1;
     }
  else {
     cur_hbuf = cur_buffer;
     cur_buffer += length;
     cur_length -= length;
     tag_read += length;
     }

  cur_frame_size -= length;

  if (version == 0x204) {
     cur_hbuf += (cur_frame_group_id * 1) + (cur_frame_encrypted * 1);
     if (cur_frame_data_length)
        cur_frame_data_size = decode_syncsafe_int(cur_hbuf);
     }
  else {
     if (cur_frame_data_length)
        cur_frame_data_size = (cur_hbuf[0] << 24) | (cur_hbuf[1] << 16) |
                      (cur_hbuf[2] << 8) | cur_hbuf[3];
     }
  return 1;
}

int cID3Reader::ParseFrameHeader(void)
{
  ResetFrameHeader();
  int r;
  if ((r = ReadFrameHeader()) != 1)
     return r;

  SetupFrameFlags();

  return ReadFrameHeaderAdd();
}

bool cID3Reader::FrameInplaceReadable(bool &readable)
{
  readable = (!cur_frame_unsynced) & ((uint32_t)cur_frame_size <= cur_length);
  if (cur_frame_unsynced | readable)
     return true;

  if (!str->Stream(cur_buffer, cur_length, cur_buffer) || !cur_length) {
     esyslog("MP3: cannot read ID3v2 frame");
     return false;
     }
  readable = (uint32_t)cur_frame_size <= cur_length;
  return true;
}

uint8_t *cID3Reader::ReadFrame(int32_t &size)
{
  uint8_t *buf = MALLOC(uint8_t, cur_frame_size);
  if (!buf) {
     esyslog("MP3: cannot alloc mem for ID3v2 frame");
     return NULL;
     }

  int32_t desynced = cur_frame_size;
  int32_t unsynced = 0;
  if (version == 0x204) {
     unsynced = desynced;
     desynced = 0;
     }

  if (cur_frame_unsynced) {
     if (!deunsync(desynced, unsynced, buf)) {
        esyslog("MP3: cannot deunsync ID3v2 frame");
        free(buf);
        return NULL;
        }
     size = desynced;
     return buf;
     }

  desynced = 0;
  while (desynced < cur_frame_size) {
        if (!str->StreamSafer(cur_buffer, cur_length, cur_buffer, 1)) {
           esyslog("MP3: cannot read ID3v2 frame");
           free(buf);
           return NULL;
           }

        int32_t tocopy = cur_frame_size - desynced;
        if ((uint32_t)tocopy > cur_length)
           tocopy = cur_length;

        memcpy(buf + desynced, cur_buffer, tocopy);
        desynced += tocopy;
        cur_buffer += tocopy;
        cur_length -= tocopy;
        }

  tag_read += desynced;
  size = desynced;
  return buf;
}

bool cID3Reader::SkipFrame(void)
{
  if ((version != 0x204) & (cur_frame_unsynced))
     return deunsync(cur_frame_size, 0, NULL);

  if (!str->Skip(cur_buffer, cur_length, cur_buffer, cur_frame_size)) {
     esyslog("MP3: cannot skip ID3v2 frame");
     return false;
     }
  tag_read += cur_frame_size;
  return true;
}

bool cID3Reader::ParseFrame(void)
{
  if (unlikely(cur_frame_encrypted)) {
     esyslog("MP3: cannot read encrypted ID3v2 frame ");
     return SkipFrame();
     }

  if (unlikely(cur_frame_compressed)) {
     esyslog("MP3: cannot read compressed ID3v2 frame (not yet implemented)");
     return SkipFrame();
     }

  int dec;
  bool found = false;
  for (dec = 0; (!found) & (dec < ID3_KNOWN_FRAME_COUNT); dec++)
      switch (version) {
        case 0x202:
             found = (memcmp(cur_frame_id, frame_decoder[dec].frame_id_v22, 3) == 0);
             break;
        case 0x203:
             found = (memcmp(cur_frame_id, frame_decoder[dec].frame_id_v23, 4) == 0);
             break;
        case 0x204:
             found = (memcmp(cur_frame_id, frame_decoder[dec].frame_id, 4) == 0);
             break;
        }

  if (!found)
     return SkipFrame();
  dec--;
  const uint8_t *frame_buffer;
  uint8_t *alloc_frame_buffer = NULL;
  int32_t frame_size;
  bool inplaceReadable;

  if (!FrameInplaceReadable(inplaceReadable))
     return false;

  frame_buffer = cur_buffer;
  frame_size = cur_frame_size;
  if (likely(inplaceReadable)) {
     cur_buffer += cur_frame_size;
     cur_length -= cur_frame_size;
     tag_read += cur_frame_size;
     }
  else {
     alloc_frame_buffer = ReadFrame(frame_size);
     if (unlikely(!alloc_frame_buffer))
        return false;
     frame_buffer = alloc_frame_buffer;
     }

  if (!(this->*frame_decoder[dec].decoder)(frame_buffer, frame_size,
           frame_decoder[dec].arg))
     esyslog("MP3: Cannot decode ID3v2 Frame");

  free(alloc_frame_buffer);
  return true;
}

int cID3Reader::SearchTagHeader(bool search)
{
  search_skipped = 0;

  if (!search) {
     if (unlikely(!str->StreamSafer(cur_buffer, cur_length, cur_buffer, 10))) {
        esyslog("MP3: Cannot read/search ID3-Tag");
        return -1;
        }
    return ParseHeader();
    }

  cur_length = 0;
  cur_buffer = NULL;
  data_offset = 0;
  data_end_offset = str->Filesize;
  if (unlikely(!str->Seek(0))) {
     esyslog("MP3: Cannot seek to begin of file for reading ID3-Tag");
     return -1;
     }

  do {
     if (unlikely(!str->StreamSafer(cur_buffer, cur_length, cur_buffer, 10))) {
        esyslog("MP3: Cannot read/search ID3-Tag");
        return -1;
        }

     int tagFound = ParseHeader();
     if (tagFound != 0)
        return tagFound;

     int skip = min(cur_length - 9, (unsigned int)ID3_MAX_SEARCH_TAG - 9);
     cur_buffer += skip;
     cur_length -= skip;
     } while(search_skipped < ID3_MAX_SEARCH_TAG - 10);

  if (str->Filesize <= ID3_MAX_SEARCH_TAG)
     return 0;

  search_skipped = 0;
  cur_length = 0;
  cur_buffer = NULL;
  if (unlikely(!str->Seek(str->Filesize - ID3_MAX_SEARCH_TAG))) {
     esyslog("MP3: Cannot seek to end of file for reading ID3-Tag");
     return -1;
     }

  do {
     if (unlikely(!str->StreamSafer(cur_buffer, cur_length, cur_buffer, 10))) {
        esyslog("MP3: Cannot read/search ID3-Tag");
        return -1;
        }

     int tagFound = ParseHeader();
     if (tagFound != 0)
        return tagFound;

     int skip = min(cur_length - 9, (unsigned int)ID3_MAX_SEARCH_TAG - 9);
     cur_buffer += skip;
     cur_length -= skip;
     } while(search_skipped < ID3_MAX_SEARCH_TAG - 10);
  return 0;
}

int cID3Reader::ReadID3(cStream *Str, const uint8_t *&buffer, uint32_t &length, bool search)
{
  str = Str;
  cur_buffer = buffer;
  cur_length = length;
  seek_frame_avail = false;
  if (!data_end_offset)
     data_end_offset = str->Filesize;

  int tagFound = SearchTagHeader(search);
  if (tagFound != 1) {
     buffer = cur_buffer;
     length = cur_length;
     return tagFound;
     }

  if ((tag_offset < ID3_MAX_SEARCH_TAG) | (data_offset == tag_offset))
     data_offset = tag_offset + tag_size + 10 +
            ((flags & ID3_FLAG_FOOTER) != 0) * 10;
  else if (tag_offset + tag_size + 20 >= (int64_t)str->Filesize - ID3_MAX_SEARCH_TAG)
     data_end_offset = tag_offset;

  dsyslog("MP3: Read ID3-Tag Version %x at 0x%llx", version, tag_offset); 
  if (version == 0x100) {
     int readOk = readIDv1Tag();
     buffer = cur_buffer + 128;
     length = cur_length - 128;
     return readOk;
     }
 
  int ok;
  while ((ok = ParseFrameHeader()) == 1) {
        if (!ParseFrame())
           return -1;
        }

  if (ok < 0)
     return -1;

  uint32_t skip = tag_size + ((flags & ID3_FLAG_FOOTER) != 0) * 10 - tag_read;

  bool skip_ok = true;
  if (unlikely(!str->Skip(cur_buffer, cur_length, cur_buffer, skip))) {
     esyslog("MP3: Cannot skip to end of ID3-Tag");
     skip_ok = false;
     cur_buffer = NULL;
     cur_length = 0;
     }

  buffer = cur_buffer;
  length = cur_length;
  if (likely((!search) | (!skip_ok) | (!seek_frame_avail)))
     return 1;

  do {
     seek_offset += tag_offset + tag_size + 10 +
                   ((flags & ID3_FLAG_FOOTER) != 0) * 10;

     buffer = NULL;
     length = 0;

     if (!str->Seek(seek_offset)) {
        esyslog("MP3: Cannot seek to next tag");
        return 1;
        }

     int r = ReadID3(str, buffer, length, false);
     if (r < 0) {
        buffer = NULL;
        length = NULL;
        }

     if (r != 1)
        esyslog("MP3: Cannot read next tag");
     } while (seek_frame_avail);
  return 1;
}

int cID3Reader::SkipID3Tag(cStream *Str, const uint8_t *&buffer, uint32_t &length)
{
  str = Str;
  cur_buffer = buffer;
  cur_length = length;
  tag_size = 10;

  if (unlikely(!str->StreamSafer(cur_buffer, cur_length, cur_buffer, 10))) {
     esyslog("MP3: Cannot read/search ID3-Tag");
     return -1;
     }

  int tagFound = FindTag();
  if (tagFound == 0) {
     buffer = cur_buffer;
     length = cur_length;
     return 0;
     }

  dsyslog("MP3: Skip ID3-Tag Version %x at 0x%llx", version, tag_offset);
  if ((tagFound == 1) & (version == 0x200)) {
     if (unlikely(!str->StreamSafer(cur_buffer, cur_length, cur_buffer, 10))) {
        esyslog("MP3: cannot fully read ID3v2 Tag header");
        return -1;
        }

     tag_size += 10 + ((cur_buffer[5] & ID3_FLAG_FOOTER) != 0) * 10;
     }

  if (unlikely(!str->Skip(cur_buffer, cur_length, cur_buffer, tag_size))) {
     esyslog("MP3: Cannot skip to end of ID3-Tag");
     return -1;
     }
  buffer = cur_buffer;
  length = cur_length;
  return 1;
}

// --- cMP3FrameSyncer ---------------------------------------------------------

#define MP3_MAX_SYNC_SKIP KILOBYTE(32)

enum eMP3FrameSyncerState
{
   mfs_start,
   mfs_synced,
   mfs_sync_lost,
   mfs_eof,
   mfs_error
};

struct mp3_frame
{
   uint32_t mpeg_version;
   uint32_t not_lsf;
   uint32_t layer;
   uint32_t bitrate_i;
   uint32_t samplerate_i;
   uint32_t padding;
   uint32_t mode;
   uint32_t samplerate;
   uint32_t bitrate;
   uint32_t channels;
   uint32_t frame_size;
   uint32_t samples;
   const uint8_t *buffer;
   int64_t offset;
};

class cMP3FrameSyncer
{
private:
  static const uint32_t bit_rates[2][3][16];
  static const uint32_t samplerate[4][4];
  static const uint32_t samples[2][4];
  static const uint32_t channels[4];
  static const uint32_t frame_slots[2][3][3][16];
  static const uint32_t mpeg_shift[4];
  static const uint32_t slot_size_shift[3];
  static const uint32_t side_info_size[2][2];

  cStream *str;
  const uint8_t *cur_buffer;
  uint32_t cur_length;
  eMP3FrameSyncerState state;
  mp3_frame cur_frame;
  mp3_vbr_header vbr_header;

  bool parseFrameHeader(const uint8_t *buffer, mp3_frame &header);
  void calcFrameData(mp3_frame &header);
  void skip_current_frame(void);
  bool searchFrameHeader(bool ingnore_lost, uint32_t &skipped);
  bool parseXING(void);
  bool parseVBRI(void);
public:
  cMP3FrameSyncer(cStream *Str);
  ~cMP3FrameSyncer();
  void setBuffer(const uint8_t *buffer, uint32_t length);
  void getBuffer(const uint8_t *&buffer, uint32_t &length);
  eMP3FrameSyncerState syncToNextFrame(const mp3_frame *&frame, bool ignore_losts);
  const mp3_vbr_header *parseVBRHeader(void);
};

// all values are specified as bits/s
const uint32_t cMP3FrameSyncer::bit_rates[2][3][16] = {
  {
    // MPEG 2/2.5, Layer I
    {      0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,
      128000, 144000, 160000, 176000, 192000, 224000, 256000,      0 },
    // MPEG 2/2.5, Layer II/III
    {      0,   8000,  16000,  24000,  32000,  40000,  48000,  56000,
       64000,  80000,  96000, 112000, 128000, 144000, 160000,      0 },
    // MPEG 2/2.5, Layer II/III
    {      0,   8000,  16000,  24000,  32000,  40000,  48000,  56000,
       64000,  80000,  96000, 112000, 128000, 144000, 160000,      0 }
  },{
    // MPEG 1, Layer I
    {      0,  32000,  64000,  96000, 128000, 160000, 192000, 224000,
      256000, 288000, 320000, 352000, 384000, 416000, 448000,      0 },
    // MPEG 1, Layer II
    {      0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,
      128000, 160000, 192000, 224000, 256000, 320000, 384000,      0 },
    // MPEG 1, Layer III
    {      0,  32000,  40000,  48000,  56000,  64000,  80000,  96000,
      112000, 128000, 160000, 192000, 224000, 256000, 320000,      0 }
  }
};

// sample rates
const uint32_t cMP3FrameSyncer::samplerate[4][4] = {
  { 11025, 12000,  8000, 0 },  // MPEG 2.5
  {     0,     0,     0, 0 },  // reversed
  { 22050, 24000, 16000, 0 },  // MPEG 2
  { 44100, 48000, 32000, 0 }   // MPEG 1
};

// sample per frame
const uint32_t cMP3FrameSyncer::samples[2][4] = {
  {  384, 1152,  576, 0 },  // MPEG 2/2.5
  {  384, 1152, 1152, 0 },  // MPEG 1
};

// channels per mode
const uint32_t cMP3FrameSyncer::channels[4] = { 2, 2, 2, 1};

// slots
const uint32_t cMP3FrameSyncer::frame_slots[2][3][3][16] = {
  {
    {
      {    0,   34,   52,   60,   69,   87,  104,  121,
         139,  156,  174,  191,  208,  243,  278,    0 },
      {    0,  104,  208,  313,  417,  522,  626,  731,
         835, 1044, 1253, 1462, 1671, 1880, 2089,    0 },
      {    0,   52,  104,  156,  208,  261,  313,  365,
         417,  522,  626,  731,  835,  940, 1044,    0 }
    },{
      {    0,   32,   48,   56,   64,   80,   96,  112,
         128,  144,  160,  176,  192,  224,  256,    0 },
      {    0,   96,  192,  288,  384,  480,  576,  672,
         768,  960, 1152, 1344, 1536, 1728, 1920,    0 },
      {    0,   48,   96,  144,  192,  240,  288,  336,
         384,  480,  576,  672,  768,  864,  960,    0 }
    },{
      {    0,   48,   72,   84,   96,  120,  144,  168,
         192,  216,  240,  264,  288,  336,  384,    0 },
      {    0,  144,  288,  432,  576,  720,  864, 1008,
        1152, 1440, 1728, 2016, 2304, 2592, 2880,    0 },
      {    0,   72,  144,  216,  288,  360,  432,  504,
         576,  720,  864, 1008, 1152, 1296, 1440,    0 }
    }
  },{
    {
      {    0,    8,   17,   26,   34,   43,   52,   60,
          69,   78,   87,   95,  104,  113,  121,    0 },
      {    0,  104,  156,  182,  208,  261,  313,  365,
         417,  522,  626,  731,  835, 1044, 1253,    0 },
      {    0,  104,  130,  156,  182,  208,  261,  313,
         365,  417,  522,  626,  731,  835, 1044,    0 }
    },{
      {    0,    8,   16,   24,   32,   40,   48,   56,
          64,   72,   80,   88,   96,  104,  112,    0 },
      {    0,   96,  144,  168,  192,  240,  288,  336,
         384,  480,  576,  672,  768,  960, 1152,    0 },
      {    0,   96,  120,  144,  168,  192,  240,  288,
         336,  384,  480,  576,  672,  768,  960,    0 }
    },{
      {    0,   12,   24,   36,   48,   60,   72,   84,
          96,  108,  120,  132,  144,  156,  168,    0 },
      {    0,  144,  216,  252,  288,  360,  432,  504,
         576,  720,  864, 1008, 1152, 1440, 1728,    0 },
      {    0,  144,  180,  216,  252,  288,  360,  432,
         504,  576,  720,  864, 1008, 1152, 1440,    0 }
    }
  }
};

const uint32_t cMP3FrameSyncer::mpeg_shift[4] = { 0, 0, 1, 0 };
const uint32_t cMP3FrameSyncer::slot_size_shift[3] = { 2, 0, 0 };
const uint32_t cMP3FrameSyncer::side_info_size[2][2] = {
  {17,  9}, // MPEG 2/2.5
  {32, 17}  // MPEG 1
};

bool cMP3FrameSyncer::parseFrameHeader(const uint8_t *buffer, mp3_frame &header)
{
  const uint32_t sync         = ((uint32_t)buffer[0] << 3) | (buffer[1] >> 5);
  const uint32_t mpeg_version = (buffer[1] >> 3) & 0x3;
  const uint32_t layer        = 3 - ((buffer[1] >> 1) & 0x3);
//const uint32_t protection   = buffer[1] & 0x1;
  const uint32_t bitrate_i    = buffer[2] >> 4;
  const uint32_t samplerate_i = (buffer[2] >> 2) & 0x3;
  const uint32_t padding      = (buffer[2] >> 1) & 0x1;
//const uint32_t private_b    = buffer[2] & 0x1;
  const uint32_t mode         = buffer[3] >> 6;
//const uint32_t mode_ext     = (buffer[3] >> 4) & 0x3;
//const uint32_t copyright    = (buffer[3] >> 3) & 0x1;
//const uint32_t original     = (buffer[3] >> 2) & 0x1;
  const uint32_t emphasis     = buffer[3] & 0x3;

  const bool ret = (sync == 0x7FF) & (mpeg_version != 0x1)  & (layer != 3) &
                 (bitrate_i != 0xF) & (samplerate_i != 0x3) & (emphasis != 0x2);

  if (unlikely(!ret))
     return false;
  /*
   * bitrate index = 0 means free bitrate -- this very rarely used
   * (I have never seen this IRL) as it complicates things much
   * we don't support it for now (this has nothing to do with vbr).
   */
  if (unlikely(bitrate_i == 0)) {
     esyslog("MP3: mp3 with free bitrate not (yet) supported");
     return false;
     }

  header.mpeg_version = mpeg_version;
  header.not_lsf = mpeg_version & 1;
  header.layer = layer;
  header.bitrate_i = bitrate_i;
  header.samplerate_i = samplerate_i;
  header.padding = padding;
  header.mode = mode;
  return true;
}

void cMP3FrameSyncer::calcFrameData(mp3_frame &header)
{
  const uint32_t frame_size = ((frame_slots[header.not_lsf][header.samplerate_i]
     [header.layer][header.bitrate_i] >>  mpeg_shift[header.mpeg_version]) +
     header.padding) << slot_size_shift[header.layer];

  header.samplerate = samplerate[header.mpeg_version][header.samplerate_i];
  header.bitrate = bit_rates[header.not_lsf][header.layer][header.bitrate_i];
  header.channels = channels[header.mode];
  header.frame_size = frame_size;
  header.samples = samples[header.not_lsf][header.layer];
}

void cMP3FrameSyncer::skip_current_frame(void)
{
  if (unlikely(state != mfs_synced))
     return;

  if (unlikely(cur_length < cur_frame.frame_size)) {
     state = mfs_error;
     esyslog("MP3: Internal error: only partial frame in buffer (%s,%d)",
             __FILE__, __LINE__);
     return;
     }

  cur_buffer += cur_frame.frame_size;
  cur_length -= cur_frame.frame_size;
}

cMP3FrameSyncer::cMP3FrameSyncer(cStream *Str)
{
  str = Str;
  cur_buffer = NULL;
  cur_length = 0;
  state = mfs_start;
  memset(&vbr_header, 0, sizeof(vbr_header));
}

cMP3FrameSyncer::~cMP3FrameSyncer()
{
  delete[] vbr_header.toc;
}

void cMP3FrameSyncer::setBuffer(const uint8_t *buffer, uint32_t length)
{
  cur_buffer = buffer;
  cur_length = length;
  state = mfs_start;
}

void cMP3FrameSyncer::getBuffer(const uint8_t *&buffer, uint32_t &length)
{
  buffer = cur_buffer;
  length = cur_length;
}

bool cMP3FrameSyncer::searchFrameHeader(bool ignore_lost, uint32_t &skipped)
{
  do {
     if (unlikely(!str->StreamSafer(cur_buffer, cur_length, cur_buffer, 4))) {
        state = cur_length == 0 ? mfs_eof : mfs_error;
        return false;
        }

     if (likely(parseFrameHeader(cur_buffer, cur_frame))) {
        state = mfs_synced;
        return true;
        }

     state = mfs_sync_lost;
     if (likely(!ignore_lost))
        return false;

     cur_buffer++;
     cur_length--;
     skipped++;
     } while (likely(skipped < MP3_MAX_SYNC_SKIP));

  state = mfs_error;
  return false;
}

eMP3FrameSyncerState cMP3FrameSyncer::syncToNextFrame(const mp3_frame *&frame,
  bool ignore_losts)
{
  skip_current_frame();
  if (unlikely(state > mfs_sync_lost))
     return state;
  ignore_losts = (!!ignore_losts) | (state == mfs_sync_lost);

  eMP3FrameSyncerState prev_state;
  uint32_t skipped = 0;
  do {
     prev_state = state;
     if (unlikely(!searchFrameHeader(ignore_losts, skipped)))
        return state;

     calcFrameData(cur_frame);
     if (likely((prev_state == mfs_synced) & (skipped == 0)))
        break;

     if (unlikely(!str->StreamSafer(cur_buffer, cur_length, cur_buffer,
           cur_frame.frame_size + 4))) {
        state = cur_length == 0 ? mfs_eof : mfs_error;
        return state;
        }

     mp3_frame next_frame;
     if (likely(parseFrameHeader(cur_buffer + cur_frame.frame_size,
           next_frame)))
        break;

     state = mfs_sync_lost;
     if (unlikely(!ignore_losts))
        return state;

     cur_buffer++;
     cur_length--;
     skipped++;
     } while (true);

  if (unlikely(!str->StreamSafer(cur_buffer, cur_length, cur_buffer,
        cur_frame.frame_size))) {
     state = cur_length == 0 ? mfs_eof : mfs_error;
     return state;
     }

  cur_frame.buffer = cur_buffer;
  cur_frame.offset = str->Tell(cur_buffer);
  frame = &cur_frame;
  return state;
}

#define XING_FLAG_FRAMES 0x1
#define XING_FLAG_SIZE 0x2
#define XING_FLAG_TOC 0x4
#define XING_FLAG_QUAL 0x8

bool cMP3FrameSyncer::parseXING(void)
{
  static const uint32_t size_by_flag[8] = { 8, 12, 12, 16, 108, 112, 112, 116};
  uint32_t offset = 4 +
     side_info_size[cur_frame.not_lsf][cur_frame.channels == 1];

  if (unlikely(cur_frame.frame_size < offset + 8))
     return false;

  const uint8_t *buf = cur_frame.buffer + offset;

  if ((memcmp(buf, "Xing", 4) != 0) & (memcmp(buf, "Info", 4) != 0))
     return false;

  const uint32_t flags = buf[7] & 0x7;
  if (unlikely(cur_frame.frame_size < offset + size_by_flag[flags]))
     return false;

  delete[] vbr_header.toc;
  memset(&vbr_header, 0, sizeof(vbr_header));

  buf += 8;
  if (likely(flags & XING_FLAG_FRAMES)) {
     vbr_header.frame_count = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) |
                               buf[3];
     buf += 4;
     }

  vbr_header.data_start = cur_frame.offset;
  if (likely(flags & XING_FLAG_SIZE)) {
     vbr_header.data_size = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) |
                               buf[3];
     buf += 4;
     }
  else {
     vbr_header.data_size = str->Filesize - cur_frame.offset;
     }

  if (likely(flags & XING_FLAG_TOC)) {
     vbr_header.toc = new uint32_t[100];
     if (unlikely(!vbr_header.toc)) {
        esyslog("MP3: No Memory!");
        return false;
        }
     vbr_header.frames_per_toc = vbr_header.frame_count / 100;
     vbr_header.valid_toc_entries = 100;
     for (int i = 0; i < 100; i++) {
         vbr_header.toc[i] = vbr_header.data_start +
            ((buf[i] * vbr_header.data_size + 128) >> 8);
         }
     }

  return (vbr_header.valid_toc_entries != 0) & (vbr_header.frame_count > 100);
}

bool cMP3FrameSyncer::parseVBRI(void)
{
  if (unlikely(cur_frame.frame_size < 36 + 26))
     return false;

  const uint8_t *buf = cur_frame.buffer + 36;

  if (memcmp(buf, "VBRI", 4) != 0)
     return false;

  delete[] vbr_header.toc;
  memset(&vbr_header, 0, sizeof(vbr_header));

  buf += 10;
  vbr_header.data_size = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8)| buf[3];
  buf += 4;
  vbr_header.frame_count = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
  buf += 4;
  const uint32_t toc_entries = ((buf[0] << 8) | buf[1]) + 1;
  buf += 2;
  const uint32_t toc_scale = (buf[0] << 8) | buf[1];
  buf += 2;
  const uint32_t toc_bytes = (buf[0] << 8) | buf[1];
  buf += 2;
  vbr_header.frames_per_toc = (buf[0] << 8) | buf[1];
  buf += 2;

  if (unlikely(cur_frame.frame_size < 36 + 26 + toc_entries * toc_bytes))
     return false;

  vbr_header.data_start = cur_frame.offset;

  vbr_header.toc = new uint32_t[toc_entries];
  if (unlikely(!vbr_header.toc)) {
     esyslog("MP3: No Memory!");
     return false;
     }

  uint32_t file_offset = cur_frame.offset;
  vbr_header.valid_toc_entries = toc_entries;
  for (uint32_t i = 0; i < toc_entries; i++) {
      uint32_t toc_ent = 0;
      for (uint32_t j = 0; j < toc_bytes; j++, buf++ ) {
          toc_ent = (toc_ent << 8) | buf[0];
          }
      toc_ent *= toc_scale;
      file_offset += toc_ent;
      vbr_header.toc[i] = file_offset;
      }

  return (vbr_header.frame_count != 0) & (vbr_header.frames_per_toc != 0);
}

const mp3_vbr_header *cMP3FrameSyncer::parseVBRHeader(void)
{
  if (parseXING() || parseVBRI())
     return &vbr_header;
  return NULL;
}

// --- cMP3Decoder -------------------------------------------------------------

cMP3Decoder::cMP3Decoder(const char *Filename, bool preinit)
:cDecoder(Filename), cThread("MP3 Decoder"), cRingBuffer(64)
{
  str=NULL;
  scan=NULL;
  isStream=false;
  if(preinit) {
    str=new cStream(filename);
    scan=new cScanID3();
    }
  pesBuffer = NULL;
  id3Reader = NULL;
  frameSyncer = NULL;
  on = false;
  SetTimeouts(50, 50);
}

cMP3Decoder::~cMP3Decoder()
{
  Activate(false);
  Clean();
  delete scan;
  delete str;
}

bool cMP3Decoder::Valid(void)
{
  return access(filename, R_OK) == 0;
}

cFileInfo *cMP3Decoder::FileInfo(void)
{
  cFileInfo *fi=NULL;
  if (str->HasInfo())
     fi=str;
  else {
     Lock();
     str->FileInfo();
     fi=str;
     Unlock();
     }
  return fi;
}

cSongInfo *cMP3Decoder::SongInfo(bool get)
{
  cSongInfo *si=NULL;
  if (scan->HasInfo())
     si=scan;
  else if(get) {
     Lock();
     if (scan->DoScan(filename))
        si=scan;
     Unlock();
     }
  return si;
}

void cMP3Decoder::InitCalc(void)
{
  dataOffset = cur_frame->offset;
  dataSize = str->Filesize - dataOffset;
  samples_per_frame = cur_frame->samples;
  sample_rate = cur_frame->samplerate;

  if (haveID3) {
     const mp3_vbr_header *vbr_header_id3;
     vbr_header_id3 = id3Reader->getVbrHeader(samples_per_frame, sample_rate);
     if (vbr_header_id3)
        vbr_header = vbr_header_id3;

     scan->SetFromID3Reader(id3Reader, str);
     dataSize = id3Reader->getDataEndOffset() - dataOffset;
     }

  totalFrames = 0;
  totalTime = 0;
  avgFrameSize = 0;
  if (vbr_header) {
     totalFrames = vbr_header->frame_count;
     uint64_t total_samples = totalFrames * samples_per_frame;
     totalTime = (total_samples +  sample_rate - 1) / sample_rate;
     }
  else {
     playedFrames = 1;
     playedFramesSizes = cur_frame->frame_size;
     CalcTimes();
     playedFrames = 0;
     playedFramesSizes = 0;
     }
}

bool cMP3Decoder::InitBuffer(void)
{
  static const uint8_t pes_header[9] = {
   0x00, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x80, 0x00, 0x00 };

  const int size = Size();
  pesBuffer = new PesBuffer[size];
  if (!pesBuffer) {
     esyslog("MP3: No Memory");
     return false;
     }

  for (int i = 0; i < size; i++) {
      pesBuffer[i].offset = 0;
      pesBuffer[i].pes_frame_length = 0;
      memcpy(pesBuffer[i].pes_frame_buffer, pes_header, 9);
      }
  return true;
}

bool cMP3Decoder::Init(void)
{
  Clean();
  if (!str->Open())
     return false;

  id3Reader = new cID3Reader();
  if (!id3Reader) {
     esyslog("MP3: No Memory");
     return false;
     }

  int id3 = 0;
  if (!isStream) {
     const uint8_t *buffer = NULL;
     uint32_t length = 0;
     id3 = id3Reader->ReadID3(str, buffer, length, true);
     if (id3 < 0)
        return false;

     haveID3 = (id3 == 1);
     int offset = 0;
     if (haveID3)
        offset = id3Reader->getDataOffset();
     str->Seek(offset);
     }
  else
     scan->DoScan(filename);

  frameSyncer = new cMP3FrameSyncer(str);
  if (!frameSyncer) {
     esyslog("MP3: No Memory");
     return false;
     }

  eMP3FrameSyncerState state;
  state = frameSyncer->syncToNextFrame(cur_frame, true);
  if (state != mfs_synced)
     return false;

  vbr_header = frameSyncer->parseVBRHeader();
  if (vbr_header) {
     state = frameSyncer->syncToNextFrame(cur_frame, true);
     if (state != mfs_synced)
        return false;
     }

  InitCalc();
  UpdateInfos();
  return InitBuffer();
}

void cMP3Decoder::CalcTimes(void)
{
  uint32_t PlayedFrames = playedFrames;
  uint64_t PlayedFramesSizes = playedFramesSizes;
  uint32_t TotalFrames = PlayedFrames;
  uint32_t AvgFrameSize = 0;
  if (!isStream) {
     AvgFrameSize = (PlayedFramesSizes + (PlayedFrames >> 1)) /
                              PlayedFrames;
     TotalFrames = (dataSize + AvgFrameSize - 1) / AvgFrameSize;
     }

  uint64_t totalSamples = TotalFrames * samples_per_frame + sample_rate - 1;
  uint32_t TotalTime = totalSamples / sample_rate;
  avgFrameSize = AvgFrameSize;
  totalFrames = TotalFrames;
  totalTime = TotalTime;
}

void cMP3Decoder::UpdateInfos(void)
{
  if (vbr_header) {
     scan->UpdateInfo(cur_frame, totalFrames, totalTime);
     scan->UpdateCache(str);
     return;
     }

  playedFrames++;
  playedFramesSizes += cur_frame->frame_size;
  scan->UpdateInfo(cur_frame, totalFrames, totalTime);
  scan->UpdateCache(str);
}

void cMP3Decoder::Clean(void)
{
  playing=false;
  delete[] pesBuffer;
  pesBuffer = NULL;
  delete id3Reader;
  id3Reader = NULL;
  delete frameSyncer;
  frameSyncer = NULL;

  putIndex = 0;
  getIndex = 0;
  seekOffset = 0;
  newSeek = 0;
  cur_frame = NULL;
  dataOffset = 0;
  dataSize = 0;
  playedFramesSizes = 0;
  playedFrames = 0;
  haveID3 = false;
  getDropInProgress = false;
  disableGet = false;

  cur_state = dsStop;
  scan->SetFromID3Reader(NULL, str);
}

bool cMP3Decoder::Activate(bool On)
{
  if (on == On)
     return true;
  if (On) {
     Lock();
     if (!Init()) {
        Clean();
        Unlock();
        return false;
        }
     playing = true;
     cur_state = dsOK;
     on = true;
     Start();
     Unlock();
     return true;
     }

  on = false;
  Cancel(10);
  Lock();
  str->Close();
  Clean();
  Unlock();

  return true;
}

void cMP3Decoder::Action(void)
{
  uint32_t lastSeek = 0;
  static const eDecodeStatus state_trans[5] =
      { dsOK, dsOK, dsOK, dsEof, dsError };

  bool ignoreLost = false;

  while (on & Running()) {
        const uint32_t NewSeek = newSeek;
        if (NewSeek != lastSeek) {
           if (!ClearSafe()) {
              cCondWait::SleepMs(10);
              continue;
              }
           const uint32_t newSeekOffset = seekOffset;
           cur_frame = NULL;
           lastSeek = NewSeek;
           str->Seek(newSeekOffset);
           frameSyncer->setBuffer(NULL, 0);
           ignoreLost = true;
           cur_state = dsOK;
           }

        if (cur_state != dsOK) {
           /* May seek around -- await stop */
           cCondWait::SleepMs(10);
           continue;
           }

        if (cur_frame) {
           if (!PutFrame())
              continue;
           }

        cur_frame = NULL;
        eMP3FrameSyncerState syncer_state;
        syncer_state = frameSyncer->syncToNextFrame(cur_frame, ignoreLost);

        if (likely(syncer_state == mfs_synced)) {
           UpdateInfos();
           ignoreLost = false;
           continue;
           }

        cur_frame = NULL;
        cur_state = state_trans[syncer_state];
        if (cur_state != dsOK)
           continue;

        /* Only lost sync possible */
        const uint8_t *buffer;
        uint32_t length;
        frameSyncer->getBuffer(buffer, length);
        int skipID3 = id3Reader->SkipID3Tag(str, buffer, length);
        if (skipID3 < 0) {
           cur_state = dsError;
           continue;
           }
        frameSyncer->setBuffer(buffer, length);
        ignoreLost = skipID3 != 1;
        }
}

bool cMP3Decoder::ClearSafe(void)
{
  disableGet = true;
  bool GetDropInProgress = getDropInProgress;
  if (GetDropInProgress)
     return false;
  Clear();
  disableGet = false;
  return true;
}

const struct DecoderFrame *cMP3Decoder::GetFrame(void)
{
  const int size = Size();
  const int GetIndex = getIndex;

  getDropInProgress = true;
  bool DisableGet = disableGet;
  int PutIndex = putIndex;
  int avail = PutIndex - GetIndex;
  if (avail < 0)
     avail += size;

  if (DisableGet | (avail <= 0)) {
     getDropInProgress = false;
     EnablePut();
     WaitForGet();

     getDropInProgress = true;
     DisableGet = disableGet;

     PutIndex = putIndex;
     avail = PutIndex - GetIndex;
     if (avail < 0)
        avail += size;
     if (DisableGet | (avail <= 0)) {
        getDropInProgress = false;
        decoderFrame.status = cur_state;
        decoderFrame.offset = 0;
        decoderFrame.pes_frame = NULL;
        decoderFrame.pes_frame_length = 0;
        EnablePut();
        return &decoderFrame;
        }
     }

  decoderFrame.status = dsPlay;
  decoderFrame.offset = pesBuffer[GetIndex].offset;
  decoderFrame.pes_frame = pesBuffer[GetIndex].pes_frame_buffer;
  decoderFrame.pes_frame_length = pesBuffer[GetIndex].pes_frame_length;
  decoderFrame.frame_param = pesBuffer[GetIndex].frame_param;
  return &decoderFrame;
}

void cMP3Decoder::DropFrame(void)
{
  if (unlikely(!getDropInProgress)) {
     esyslog("MP3: internal error DropFrame() without GetFrame()");
     return;
     }
  const int size = Size();
  int GetIndex = getIndex;
  GetIndex++;
  if (GetIndex >= size)
     GetIndex = 0;
  getIndex = GetIndex;
  getDropInProgress = false;
  EnablePut();
}

bool cMP3Decoder::PutFrame(void)
{
  if (cur_frame->frame_size + 9 > MAX_MP3_PES_FRAME_SIZE) {
     esyslog("MP3: Internal error frame size exceeds maximum");
     return true;
     }
  const int size = Size();
  int PutIndex = putIndex;
  int GetIndex = getIndex;

  int free = GetIndex - PutIndex;
  if (free <= 0)
     free += size;

  if (free <= 1) {
     EnableGet();
     WaitForPut();

     GetIndex = getIndex;
     free = GetIndex - PutIndex;
     if (free <= 0)
        free += size;

     if (free <= 1) {
        EnableGet();
        return false;
        }
     }

  pesBuffer[PutIndex].offset = cur_frame->offset;
  pesBuffer[PutIndex].pes_frame_length = cur_frame->frame_size + 9;
  memcpy(pesBuffer[PutIndex].pes_frame_buffer + 9,
          cur_frame->buffer, cur_frame->frame_size);
  uint16_t pes_size = cur_frame->frame_size + 3;
  pesBuffer[PutIndex].pes_frame_buffer[4] = (pes_size >> 8) & 0xFF;
  pesBuffer[PutIndex].pes_frame_buffer[5] = pes_size & 0xFF;
  pesBuffer[PutIndex].frame_param = cur_frame->mpeg_version | 
     (cur_frame->layer << 8) | (cur_frame->samplerate_i << 16) | 
	  (cur_frame->mode << 24);

  PutIndex++;
  if (PutIndex >= size)
     PutIndex = 0;
  putIndex = PutIndex;
  EnableGet();
  return true;
}

uint32_t cMP3Decoder::GetTimes(uint64_t offset, uint32_t &TotalTime)
{
  if (isStream | (!playing) | (!vbr_header)) {
     if (playing)
        CalcTimes();

     TotalTime = totalTime;
     if ((!playing) | isStream)
        return TotalTime;

     return ((offset - dataOffset) * samples_per_frame) / 
            (avgFrameSize * sample_rate);
     }

  TotalTime = totalTime;
  if (offset <= vbr_header->toc[0])
     return 0;

  int lowerLimit = 0;
  int upperLimit = vbr_header->valid_toc_entries - 1;
  while (upperLimit - lowerLimit > 1) {
        int testInd = lowerLimit + ((upperLimit - lowerLimit) >> 1);
        if (offset < vbr_header->toc[testInd])
           upperLimit = testInd;
        else
           lowerLimit = testInd;
        }

  uint32_t upper_offset = vbr_header->toc[upperLimit];
  uint32_t frames_in_toc = vbr_header->frames_per_toc;
  if (offset >= vbr_header->toc[upperLimit]) {
     lowerLimit = upperLimit;
     upper_offset = vbr_header->data_start + vbr_header->data_size;
     if (offset >= upper_offset)
        upper_offset = offset + 1;
     frames_in_toc = totalFrames - vbr_header->frames_per_toc * upperLimit;
     if ((int32_t) frames_in_toc < 0)
        frames_in_toc = 0;
     }

  const uint32_t lower_offset = vbr_header->toc[lowerLimit];
  uint64_t tmp = ((offset - lower_offset) * frames_in_toc) /
                       (upper_offset - lower_offset);
  tmp += (lowerLimit * vbr_header->frames_per_toc);
  tmp = (tmp * samples_per_frame ) / sample_rate;
  return tmp;
}

bool cMP3Decoder::Seek(int Seconds)
{
  if ((!playing) | (isStream) | (Seconds < 0))
     return false;

  const uint32_t NewSeek = newSeek + 1;
  if ((uint32_t)Seconds >= totalTime)
     Seconds = totalTime - 3;

  uint64_t tmp = Seconds;
  tmp = (tmp * sample_rate) / samples_per_frame;

  if (!vbr_header) {
     tmp = tmp * avgFrameSize + dataOffset;
     seekOffset = tmp;
     disableGet = true;
     newSeek = NewSeek;
     return true;
     }

  const uint32_t frame_pos = tmp;
  uint32_t fr_per_toc = vbr_header->frames_per_toc;
  uint32_t toc_ind = frame_pos / fr_per_toc;
  uint32_t frames_left = frame_pos % fr_per_toc;

  if (toc_ind >= vbr_header->valid_toc_entries)
     toc_ind = vbr_header->valid_toc_entries - 1;

  uint32_t end_offset;
  if (toc_ind + 1 >= vbr_header->valid_toc_entries) {
     end_offset = vbr_header->data_size + vbr_header->data_start;
     fr_per_toc *= toc_ind;
     frames_left = frame_pos - fr_per_toc;
     fr_per_toc = totalFrames - fr_per_toc;
     }
  else
     end_offset = vbr_header->toc[toc_ind + 1];

  if ((end_offset <= vbr_header->toc[toc_ind]) | (frames_left == 0) |
       (frames_left >= fr_per_toc)) {
     seekOffset = vbr_header->toc[toc_ind];
     disableGet = true;
     newSeek = NewSeek;
     return true;
     }

  uint32_t offset = vbr_header->toc[toc_ind];
  offset += ((end_offset - offset) * frames_left) / fr_per_toc;
  seekOffset = offset;
  disableGet = true;
  newSeek = NewSeek;
  return true;
}

const uint8_t *cMP3Decoder::GetImage(int &length)
{
  if (!id3Reader)
     return NULL;
  return id3Reader->getIFramePic(length);
}
// --- cScanID3 ----------------------------------------------------------------

cScanID3::cScanID3()
{
  id3Reader = NULL;
  modified = false;
  lastCacheUpdate = 0;
}

bool cScanID3::CheckInfoCache(cStream *str)
{
  cCacheData *dat=InfoCache.Search(str);
  if (dat) {
     Set(dat);
     dat->Unlock();
     if (!DecoderID) {
        DecoderID=DEC_MP3;
        InfoCache.Cache(this,str);
        }
     return true;
     }
  return false;
}

void cScanID3::SetFromID3Reader(cID3Reader *reader, cStream *str)
{
  id3Reader = reader;
  InfoDone();
  if (!reader) {
     if (!Title) {
        FakeTitle(str->Filename,".mp3");
        modified = true;
        }
     return;
     }

  const char *tmp;
  tmp = reader->getTitle();
  if (tmp) {
     free(Title);
     Title = strdup(tmp);
     modified = true;
     }
  else if (!Title) {
     FakeTitle(str->Filename, ".mp3");
     modified = true;
     }

  tmp = reader->getArtist();
  if (tmp) {
     free(Artist);
     Artist = strdup(tmp);
     modified = true;
     }

  tmp = reader->getAlbum();
  if (tmp) {
     free(Album);
     Album = strdup(tmp);
     modified = true;
     }

  tmp = reader->getYear();
  if (tmp) {
     Year = atoi(tmp);
     modified = true;
     }
}

bool cScanID3::DoScan(const char *filename)
{
  if (HasInfo())
     return true;

  cStream str(filename);
  if (!str.Open())
     return false;
  if (CheckInfoCache(&str))
     return true;

  Clear();

  cID3Reader reader;

  const uint8_t *buffer = NULL;
  uint32_t length = 0;
  int have_id3 = reader.ReadID3(&str, buffer, length, true);

  if (have_id3 < 0)
     return false;

  int64_t offset = 0;
  int64_t dataSize = str.Filesize;
  if (have_id3) {
     SetFromID3Reader(&reader, &str);
     offset = reader.getDataOffset();
     dataSize = reader.getDataEndOffset() - offset;
     }

  SetFromID3Reader(NULL, &str);

  str.Seek(offset);
  cMP3FrameSyncer syncer(&str);

  const mp3_frame *frame;
  eMP3FrameSyncerState state;
  state = syncer.syncToNextFrame(frame, true);
  if (state != mfs_synced)
     return false;

  const mp3_vbr_header *vbr_header;
  vbr_header = syncer.parseVBRHeader();
  if (vbr_header) {
     state = syncer.syncToNextFrame(frame, true);
     if (state != mfs_synced)
        return false;
     }

  if (have_id3) {
     const mp3_vbr_header *vbr_header_id3;
     vbr_header_id3 = reader.getVbrHeader(frame->samples, frame->samplerate);
     if (vbr_header_id3)
        vbr_header = vbr_header_id3;
     }

  if (vbr_header) {
     Frames = vbr_header->frame_count;
     }
  else {
     // Assume cbr
     dataSize += frame->frame_size - 1;
     Frames = dataSize / frame->frame_size;
     }

  Bitrate = frame->bitrate;
  SampleFreq = frame->samplerate;
  Channels = frame->channels;
  ChMode = frame->mode;
  DecoderID=DEC_MP3;
  uint64_t total_samples = Frames * frame->samples;
  Total = total_samples / SampleFreq;

  InfoDone();
  InfoCache.Cache(this, &str);
  lastCacheUpdate = time(NULL);
  modified = false;
  return true;
}

void cScanID3::UpdateInfo(const mp3_frame *frame, int newFrames, int newTotal)
{
  modified |= (SampleFreq != (int)frame->samplerate) |
             (Channels != (int)frame->channels) | (ChMode != (int)frame->mode) |
             (Frames != newFrames);

  SampleFreq = frame->samplerate;
  Channels = frame->channels;
  ChMode = frame->mode;
  Frames = newFrames;
  Total = newTotal;

  if (Bitrate < 0) {
     Bitrate = frame->bitrate;
     modified = true;
     }
  else if (Bitrate != (int)frame->bitrate) {
     if (MaxBitrate < 0) {
        modified = true;
        if (Bitrate < (int)frame->bitrate)
            MaxBitrate = frame->bitrate;
        else {
            MaxBitrate = Bitrate;
            Bitrate = frame->bitrate;
            }
        }
     else {
        if ((int)frame->bitrate > MaxBitrate) {
           MaxBitrate = frame->bitrate;
           modified = true;
           }
        if ((int)frame->bitrate < Bitrate) {
           Bitrate = frame->bitrate;
           modified = true;
           }
        }
     }
  InfoHook();
}
#define MAX_CACHE_UPDATE_INTERVAL 10
void cScanID3::UpdateCache(cStream *str)
{
  if (!modified)
     return;
  time_t now = time(NULL);
  if (lastCacheUpdate + MAX_CACHE_UPDATE_INTERVAL > now)
     return;
  InfoCache.Cache(this, str);
  modified = false;
  lastCacheUpdate = now;
}