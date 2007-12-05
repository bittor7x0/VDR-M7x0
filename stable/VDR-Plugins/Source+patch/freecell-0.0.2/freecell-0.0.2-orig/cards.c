#include <vdr/tools.h>
#include <vdr/plugin.h>
#include <stdlib.h>
#include "cards.h"
#include "cursor.h"
#include "osd.h"

const char *ShapeNames[SHAPE_COUNT] = {
	"herz",
	"karo",
	"pik",
	"kreuz"
};

const char *ValueNames[VALUE_COUNT] = {
	"ace",
	"two",
	"three",
	"four",
	"five",
	"six",
	"seven",
	"eight",
	"nine",
	"ten",
	"jack",
	"queen",
	"king"
};

cCard::cCard(int Shape, int Value): cBitmap(Width(), Height(), 4) {
	char *filename;

	mShape = Shape;
	mValue = Value;
	
	asprintf(&filename, "%s/%s_%s.xpm", cPlugin::ConfigDirectory("freecell"), 
			ShapeNames[mShape], ValueNames[mValue]);
	::LoadXpm(this, filename);
	free(filename);
}

cCard::~cCard() {
}

bool cCard::Accepts(const cCard *Card) {	
	return (mValue > VALUE_ACE && mValue == Card->mValue + 1 
			&& SHAPE_RED(mShape) != SHAPE_RED(Card->mShape));
}

cCardStack::cCardStack(int X, int Y, bool Stacked) {
	mX = X;
	mY = Y;
	mStacked = Stacked;
	mSeed = time_ms();
}

cCardStack::~cCardStack() {
	Clear();
}

void cCardStack::Clear(void) {
	while (Count() > 0)
		delete TakeFirst();
}

void cCardStack::Refill(void) {
	printf("\nRefill\n\n");
	Clear();
	for (int it = 0; it < VALUE_COUNT; ++it) {
		Append(new cCard(SHAPE_HERZ,  it));
		Append(new cCard(SHAPE_KARO,  it));
		Append(new cCard(SHAPE_PIK,   it));
		Append(new cCard(SHAPE_KREUZ, it));
	}
}

void cCardStack::Shuffle(void) {
	cTBList<cCard*> cards = *this;

	cTBList<cCard*>::Clear();
	while (cards.Count()) {
		int idx = (int)((double)cards.Count() * rand_r(&mSeed) / (RAND_MAX + 1.0));
		Append(cards.Take(idx));
	}
}

bool cCardStack::Accepts(const cCard *Card) {	
	if (Count() == 0)
		return true;
	return Last()->Accepts(Card);
}

bool cCardStack::Solves(const cCard *Card) {
	if (Count() == 0)
		return Card->mValue == VALUE_ACE;
	return Last()->mShape == Card->mShape && Last()->mValue + 1 == Card->mValue;
}

bool cCardStack::Solved(void) {
	bool solved = Count() > 0 && Last()->mValue == VALUE_KING;
	int shape = -1;
	if (solved) {
		for (uint it = 0; it < Count(); ++it) {
			if (shape == -1)
				shape = At(it)->mShape;
			else if (shape != At(it)->mShape) {
				solved = false;
				break;
			}
			if (At(it)->mValue != (int)it)	{
				solved = false;
				break;
			}
		}
	}
	return solved;
}

void cCardStack::GetCursorPos(int &x, int &y) {
	x = mX + cCard::Width() / 2 - cCursor::Width() / 2;
	y = mY + (Count() - 1) * cCard::Height() / 3 + cCard::Height() / 2 
			- cCursor::Height() / 2;
}

void cCardStack::GetFloatingPos(int &x, int &y) {
	x = mX;
	y = mY;
	if (mStacked)
		y += (Count() + 1) * cCard::Height() / 3;
}

#if VDRVERSNUM < 10307
void cCardStack::Paint(cOsdBase *Osd) {
#else /* VDRVERSNUM < 10307 */
void cCardStack::Paint(cOsd *Osd) {
#endif /* VDRVERSNUM < 10307 */
	ASSERT(mX == -1 || mY == -1);

	Frame(Osd, mX, mY, mX + cCard::Width() - 1, mY + cCard::Height() - 1, 
			clrWhite);
	if (mStacked)
		for (uint it = 0; it < Count(); ++it)
#if VDRVERSNUM < 10307
			Osd->SetBitmap(mX, mY + it * cCard::Height() / 3, *At(it));
#else /* VDRVERSNUM < 10307 */
			Osd->DrawBitmap(mX, mY + it * cCard::Height() / 3, *At(it));
#endif /* VDRVERSNUM < 10307 */
	else if (Count() > 0)
#if VDRVERSNUM < 10307
		Osd->SetBitmap(mX, mY, *Last());
#else /* VDRVERSNUM < 10307 */
		Osd->DrawBitmap(mX, mY, *Last());
#endif /* VDRVERSNUM < 10307 */
}
