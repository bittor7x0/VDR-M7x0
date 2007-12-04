#ifndef VDR_FREECELL_CURSOR_H
#define VDR_FREECELL_CURSOR_H

#if VDRVERSNUM < 10307
#include <vdr/osdbase.h>
#else /* VDRVERSNUM < 10307 */
#include <vdr/osd.h>
#endif /* VDRVERSNUM < 10307 */

class cCursor: public cBitmap {
private:
	int mX, mY;

public:
	cCursor(void);
	virtual ~cCursor();

	static int Width(void) { return 16; }
	static int Height(void) { return 22; }

	void Move(int x, int y) { mX = x; mY = y; }
#if VDRVERSNUM < 10307
	void Paint(cOsdBase *Osd, eDvbColor Color);
#else /* VDRVERSNUM < 10307 */
	void Paint(cOsd *Osd, tColor Color);
#endif /* VDRVERSNUM < 10307 */
};

#endif // VDR_FREECELL_CURSOR_H
