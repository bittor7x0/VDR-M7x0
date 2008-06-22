// originally from freecell by Sascha Volkenandt (sascha@akv-soft.de)

#include <vdr/plugin.h>
#include "tools/tools.h"
#include "cursor.h"
#include "osd.h"

cCursor::cCursor(void): cBitmap(Width(), Height(), 4) {
	char *filename;

	mX = mY = -1;

	asprintf(&filename, "%s/cursor.xpm", cPlugin::ConfigDirectory("solitaire"));
	::LoadXpm(this, filename, clrTransparent);
	free(filename);
}

cCursor::~cCursor() {
}

#if VDRVERSNUM < 10307
void cCursor::Paint(cOsdBase *Osd, eDvbColor Color) {
#else /* VDRVERSNUM < 10307 */
void cCursor::Paint(cOsd *Osd, tColor Color) {
#endif /* VDRVERSNUM < 10307 */
	ASSERT(mX == -1 || mY == -1);

	for (int x = 0; x < Width(); ++x) {
		for (int y = 0; y < Height(); ++y) {
			char index = *Data(x, y);
#if VDRVERSNUM < 10307
			eDvbColor col = GetColor(index);
#else /* VDRVERSNUM < 10307 */
			tColor col = cPalette::Color(index);
#endif /* VDRVERSNUM < 10307 */
			if (col != clrTransparent) 
#if VDRVERSNUM < 10307
				Osd->Fill(mX + x, mY + y, mX + x, mY + y, Color);
#else /* VDRVERSNUM < 10307 */
				Osd->DrawRectangle(mX + x, mY + y, mX + x, mY + y, Color);
#endif /* VDRVERSNUM < 10307 */
		}
	}
}
	
