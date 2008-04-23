#ifndef VDR_FREECELL_OSD_H
#define VDR_FREECELL_OSD_H

#if VDRVERSNUM < 10307
#include <vdr/osdbase.h>
#else /* VDRVERSNUM < 10307 */
#include <vdr/osd.h>
#endif /* VDRVERSNUM < 10307 */

#if VDRVERSNUM < 10307
void Frame(cOsdBase *Osd, int x1, int y1, int x2, int y2, eDvbColor Color);
void Frame(cBitmap *Bitmap, int x1, int y1, int x2, int y2, eDvbColor Color);
#else /* VDRVERSNUM < 10307 */
void Frame(cOsd *Osd, int x1, int y1, int x2, int y2, tColor Color);
void Frame(cBitmap *Bitmap, int x1, int y1, int x2, int y2, tColor Color);
#endif /* VDRVERSNUM < 10307 */
bool LoadXpm(cBitmap *Bitmap, const char *FileName, 
#if VDRVERSNUM < 10307
		eDvbColor NoneColor = clrTransparent);
char *WrapText(cBitmap *Bitmap, const char *Text, int Width, 
#else /* VDRVERSNUM < 10307 */
		tColor NoneColor = clrTransparent);
char *WrapText(cBitmap *Bitmap, const char *Text, int Width, const cFont *Font,
#endif /* VDRVERSNUM < 10307 */
		int *Height = NULL);

#endif // VDR_FREECELL_OSD_H

