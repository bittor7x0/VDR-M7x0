#ifndef VDR_FREECELL_FIELD_H
#define VDR_FREECELL_FIELD_H

#include <vdr/osdbase.h>
#include <vdr/osd.h>

class cCardStack;
class cCursor;

class cFreecellGame: public cOsdObject {
private:
	int mX, mY, mWidth, mHeight;
	int mPosition;
	bool mFirstGame;
	bool mShowInfo;
	bool mShowMessage;
	bool mShowHelp;
	bool mSelectStack;
	bool mGameOver;
#if VDRVERSNUM < 10307
	cOsdBase *mOsd;
#else /* VDRVERSNUM < 10307 */
	cOsd *mOsd;
#endif /* VDRVERSNUM < 10307 */
	cCursor *mCursor;
	cBitmap *mInfo;
	cBitmap *mMessage;
	cBitmap *mHelp;
	cCardStack *mStacks[8];
	cCardStack *mSlots[4];
	cCardStack *mTargets[4];
	cCardStack *mFloating;

protected:
	virtual void Show(void);
	virtual eOSState ProcessKey(eKeys Key);
	void Paint(void);

	void WriteInfo(const char *Text);
	void WriteMessage(const char *Text);
	void WriteHelp(void);
	void ShowInfo(bool Show) { mShowInfo = Show; }
	void ShowMessage(bool Show) { mShowMessage = Show; }
	void ShowHelp(bool Show) { mShowHelp = Show; }
	void AutoSolve(void);
	bool CanMoveTo(int Position);

public:
	cFreecellGame(void);
	virtual ~cFreecellGame();
	
	void Start(void);
};

#endif // VDR_FREECELL_FIELD_H
