#ifndef VDR_FREECELL_CARDS_H
#define VDR_FREECELL_CARDS_H

#if VDRVERSNUM < 10307
#include <vdr/osdbase.h>
#else /* VDRVERSNUM < 10307 */
#include <vdr/osd.h>
#endif /* VDRVERSNUM < 10307 */
#include "tools/list.h"

#define SHAPE_HERZ  0
#define SHAPE_KARO  1
#define SHAPE_PIK   2
#define SHAPE_KREUZ 3
#define SHAPE_COUNT 4

#define SHAPE_RED(s)   ((s) == SHAPE_HERZ || (s) == SHAPE_KARO)
#define SHAPE_BLACK(s) ((s) == SHAPE_PIK || (s) == SHAPE_KREUZ)

#define VALUE_ACE    0
#define VALUE_2      1
#define VALUE_3      2 
#define VALUE_4      3
#define VALUE_5      4
#define VALUE_6      5
#define VALUE_7      6
#define VALUE_8      7
#define VALUE_9      8
#define VALUE_10     9
#define VALUE_JACK  10
#define VALUE_QUEEN 11
#define VALUE_KING  12
#define VALUE_COUNT 13

class cCardStack;

class cCard: public cBitmap {
	friend class cCardStack;

private:
	int mShape;
	int mValue;

public:
	cCard(int Shape, int Value);
	virtual ~cCard();

	static int Width() { return 54; }
	static int Height() { return 72; }

	bool Accepts(const cCard *Card);
};

class cCardStack: public cTBList<cCard*> {
private:
	int mX, mY;
	bool mStacked;
	uint mSeed;

public:
	cCardStack(int X = -1, int Y = -1, bool Stacked = false);
	virtual ~cCardStack();

	void Clear(void);
	void Refill(void);
	void Shuffle(void);
	void Move(int x, int y) { mX = x; mY = y; }
	bool Accepts(const cCard *Card);
	bool Solves(const cCard *Card);
	bool Solved(void);

	void GetCursorPos(int &x, int &y);
	void GetFloatingPos(int &x, int &y);

#if VDRVERSNUM < 10307
	void Paint(cOsdBase *Osd);
#else /* VDRVERSNUM < 10307 */
	void Paint(cOsd *Osd);
#endif /* VDRVERSNUM < 10307 */
};

#endif // VDR_FREECELL_CARDS_H
