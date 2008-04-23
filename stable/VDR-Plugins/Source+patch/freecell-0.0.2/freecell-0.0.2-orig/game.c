#include "game.h"
#include "cards.h"
#include "osd.h"
#include "cursor.h"
#include "i18n.h"
#include <math.h>

cFreecellGame::cFreecellGame(void): cOsdObject(true) {
	mWidth = 480;
	mHeight = 360;
	mX = (720 - mWidth) / 2;
	mY = (576 - mHeight) / 2;
	mPosition = 0;
	mFirstGame = true;
	mShowInfo = false;
	mShowMessage = false;
	mShowHelp = false;
	mSelectStack = false;
	mGameOver = false;
	mOsd = NULL;
	mCursor = NULL;
	mFloating = NULL;
	mInfo = NULL;
	mMessage = NULL;
	mHelp = NULL;
	memset(mStacks, 0, sizeof(mStacks));
	memset(mSlots, 0, sizeof(mSlots));
	memset(mTargets, 0, sizeof(mTargets));
}

cFreecellGame::~cFreecellGame() {
	delete mHelp;
	delete mMessage;
	delete mInfo;
	delete mFloating;
	for (int it = 0; it < 8; ++it)
		delete mStacks[it];
	for (int it = 0; it < 4; ++it)
		delete mSlots[it];
	for (int it = 0; it < 4; ++it)
		delete mTargets[it];
	delete mCursor;
	delete mOsd;
}

void cFreecellGame::Show(void) {
	printf("Show\n");
#if VDRVERSNUM < 10307
	mOsd = cOsd::OpenRaw(0, 0);
	if (mOsd) {	
		mOsd->Create(mX, mY, mWidth, mHeight, 4, true);
#else /* VDRVERSNUM < 10307 */
	mOsd = cOsdProvider::NewOsd(0, 0);
	if (mOsd) {
		tArea Areas[] = { { mX, mY, mX + mWidth - 1, mY + mHeight - 1, 4 } };
		mOsd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
#endif /* VDRVERSNUM < 10307 */
		Start();
		Paint();
	}
}

void cFreecellGame::Paint(void) {
	ASSERT(mOsd == NULL);
	ASSERT(mPosition < -4 || mPosition > 7);

	int x, y;
	cCardStack *stack = mPosition < 0 ? mSlots[~mPosition] : mStacks[mPosition];
	if (mFloating->Count() == 0) {
		stack->GetCursorPos(x, y);
		mCursor->Move(x, y);
	} else {
		stack->GetFloatingPos(x, y);
		mFloating->Move(x, y);
		mFloating->GetCursorPos(x, y);
		mCursor->Move(x, y);
	}
		
#if VDRVERSNUM < 10307
	mOsd->Clear();
#else /* VDRVERSNUM < 10307 */
	mOsd->DrawRectangle(mX, mY, mX + mWidth - 1, mY + mHeight - 1, clrGray50);
#endif /* VDRVERSNUM < 10307 */

	for (int it = 0; it < 4; ++it)
		mTargets[it]->Paint(mOsd);
	for (int it = 0; it < 4; ++it)
		mSlots[it]->Paint(mOsd);
	for (int it = 0; it < 8; ++it)
		mStacks[it]->Paint(mOsd);
	if (mFloating->Count() > 0)
		mFloating->Paint(mOsd);
	mCursor->Paint(mOsd, mFloating->Count() > 0 ? clrYellow : clrBlue);
	if (mShowInfo) 
#if VDRVERSNUM < 10307
		mOsd->SetBitmap(mX + (mWidth - mInfo->Width()) / 2, mY + 10, *mInfo);
#else /* VDRVERSNUM < 10307 */
		mOsd->DrawBitmap(mX + (mWidth - mInfo->Width()) / 2, mY + 10, *mInfo);
#endif /* VDRVERSNUM < 10307 */
	if (mShowMessage) 
#if VDRVERSNUM < 10307
		mOsd->SetBitmap(mX + (mWidth - mMessage->Width()) / 2, mY + mHeight 
#else /* VDRVERSNUM < 10307 */
		mOsd->DrawBitmap(mX + (mWidth - mMessage->Width()) / 2, mY + mHeight 
#endif /* VDRVERSNUM < 10307 */
				- mMessage->Height() - 10, *mMessage);
	if (mShowHelp) 
#if VDRVERSNUM < 10307
		mOsd->SetBitmap(mX + (mWidth - mHelp->Width()) / 2, mY + (mHeight 
#else /* VDRVERSNUM < 10307 */
		mOsd->DrawBitmap(mX + (mWidth - mHelp->Width()) / 2, mY + (mHeight 
#endif /* VDRVERSNUM < 10307 */
				- mHelp->Height()) / 2, *mHelp);
	
	Frame(mOsd, mX, mY, mX + mWidth - 1, mY + mHeight - 1, clrRed);

	mOsd->Flush();
}

void cFreecellGame::Start(void) {
	cCardStack pile;
	int stack;

	if (mFirstGame) {
#if VDRVERSNUM >= 10307
		const cFont *font = cFont::GetFont(fontOsd);
#endif /* VDRVERSNUM < 10307 */
		for (int it = 0; it < 8; ++it)
			mStacks[it] = new cCardStack(mX + 5 + (cCard::Width() + 5) * it, mY + 82, 
					true);
		for (int it = 0; it < 4; ++it)
			mSlots[it] = new cCardStack(mX + 5 + (cCard::Width() + 5) * it, mY + 5);
		for (int it = 0; it < 4; ++it)
			mTargets[it] = new cCardStack(mX + 5 + (cCard::Width() + 5) * (it + 4), 
					mY + 5);
		mFloating = new cCardStack(-1, -1, true);
		mCursor = new cCursor;
#if VDRVERSNUM < 10307
		mInfo = new cBitmap(mWidth / 2, cOsd::LineHeight() + 6, 4);
		mInfo->SetFont(fontOsd);
		mMessage = new cBitmap(mWidth * 3/4, cOsd::LineHeight() + 6, 4);
		mMessage->SetFont(fontOsd);
#else /* VDRVERSNUM < 10307 */
		mInfo = new cBitmap(mWidth / 2, font->Height() + 6, 4);
		mMessage = new cBitmap(mWidth * 3/4, font->Height() + 6, 4);
#endif /* VDRVERSNUM < 10307 */
		mHelp = new cBitmap(mWidth * 6/7, mHeight * 6/7, 4);
#if VDRVERSNUM < 10307
		mHelp->SetFont(fontOsd);
#endif /* ! VDRVERSNUM < 10307 */
		WriteHelp(); // needs to be done only once
		mFirstGame = false;
	} else {
		for (int it = 0; it < 8; ++it)
			mStacks[it]->Clear();
		for (int it = 0; it < 4; ++it)
			mSlots[it]->Clear();
		for (int it = 0; it < 4; ++it)
			mTargets[it]->Clear();
		mFloating->Clear();
		mCursor->Move(-1, -1);
	}

	ShowInfo(false);
	ShowMessage(false);
	ShowHelp(false);
	mGameOver = false;
	mPosition = 0;

	pile.Refill();
	pile.Shuffle();
	stack = 0;
	while (pile.Count() > 0) {
		mStacks[stack]->Append(pile.TakeFirst());
		if (++stack == 8) stack = 0;
	}
}

void cFreecellGame::WriteInfo(const char *Text) {
#if VDRVERSNUM < 10307
	int x = max((mInfo->Width() - mInfo->Width(Text)) / 2, 0);
	mInfo->Fill(0, 0, mInfo->Width() - 1, mInfo->Height() - 1, clrWhite);
#else /* VDRVERSNUM < 10307 */
	const cFont *font = cFont::GetFont(fontOsd);
	int x = max((mInfo->Width() - font->Width(Text)) / 2, 0);
	mInfo->DrawRectangle(0, 0, mInfo->Width() - 1, mInfo->Height() - 1, clrWhite);
#endif /* VDRVERSNUM < 10307 */
	Frame(mInfo, 0, 0, mInfo->Width() - 1, mInfo->Height() - 1, clrRed);
#if VDRVERSNUM < 10307
	mInfo->Text(x, 3, Text, clrBlack, clrWhite);
#else /* VDRVERSNUM < 10307 */
	mInfo->DrawText(x, 3, Text, clrBlack, clrWhite, font);
#endif /* VDRVERSNUM < 10307 */
}

void cFreecellGame::WriteMessage(const char *Text) {
#if VDRVERSNUM < 10307
	int x = max((mMessage->Width() - mMessage->Width(Text)) / 2, 0);
	mMessage->Fill(0, 0, mMessage->Width() - 1, mMessage->Height() - 1, clrWhite);
#else /* VDRVERSNUM < 10307 */
	const cFont *font = cFont::GetFont(fontOsd);
	int x = max((mMessage->Width() - font->Width(Text)) / 2, 0);
	mMessage->DrawRectangle(0, 0, mMessage->Width() - 1, mMessage->Height() - 1, clrWhite);
#endif /* VDRVERSNUM < 10307 */
	Frame(mMessage, 0, 0, mMessage->Width() - 1, mMessage->Height() - 1, clrRed);
#if VDRVERSNUM < 10307
	mMessage->Text(x, 3, Text, clrBlack, clrWhite);
#else /* VDRVERSNUM < 10307 */
	mMessage->DrawText(x, 3, Text, clrBlack, clrWhite, font);
#endif /* VDRVERSNUM < 10307 */
}

void cFreecellGame::WriteHelp(void) {
#if VDRVERSNUM >= 10307
	const cFont *font = cFont::GetFont(fontOsd);
#endif /* VDRVERSNUM < 10307 */
	static const char *HelpText = 
			"This is Freecell, a card-game for the Video Disk Recorder.\n\n"
			"Up/Down: Jump cells/stacks\n"
			"Left/Right: Move hand/cards\n"
			"OK: Grab card or stack\n"
			"Up/Down: Select/Deselect cards\n"
			"OK: Confirm stack selection\n"
			"Blue: Start new game\n\n"
			"Press OK to resume playing";

	int line = 0;
	char *text, *ptr, *first;

#if VDRVERSNUM < 10307
	mHelp->Fill(0, 0, mHelp->Width() - 1, mHelp->Height() - 1, clrWhite);
#else /* VDRVERSNUM < 10307 */
	mHelp->DrawRectangle(0, 0, mHelp->Width() - 1, mHelp->Height() - 1, clrWhite);
#endif /* VDRVERSNUM < 10307 */
	Frame(mHelp, 0, 0, mHelp->Width() - 1, mHelp->Height() - 1, clrRed);
	
#if VDRVERSNUM < 10307
	first = text = WrapText(mHelp, tr(HelpText), mHelp->Width() - 10);
#else /* VDRVERSNUM < 10307 */
	first = text = WrapText(mHelp, tr(HelpText), mHelp->Width() - 10, font);
#endif /* VDRVERSNUM < 10307 */
	while ((ptr = strchr(first, '\n')) != NULL) {
		*(ptr++) = '\0';
#if VDRVERSNUM < 10307
		mHelp->Text(5, 5 + line * cOsd::LineHeight(), first, clrBlack, clrWhite);
#else /* VDRVERSNUM < 10307 */
		mHelp->DrawText(5, 5 + line * font->Height(), first, clrBlack, clrWhite, font);
#endif /* VDRVERSNUM < 10307 */
		first = ptr;
		++line;
	}
	// print last line, too
#if VDRVERSNUM < 10307
	mHelp->Text(5, 5 + line * cOsd::LineHeight(), first, clrBlack, clrWhite);
#else /* VDRVERSNUM < 10307 */
	mHelp->DrawText(5, 5 + line * font->Height(), first, clrBlack, clrWhite, font);
#endif /* VDRVERSNUM < 10307 */
	free(text);
}

void cFreecellGame::AutoSolve(void) {
	bool found, solved;
	do {
		found = false;
		for (int it = 0; it < 8; ++it) {
			if (mStacks[it]->Count() > 0) {
				for (int it2 = 0; it2 < 4; ++it2) {
					if (mTargets[it2]->Solves(mStacks[it]->Last())) {
						mTargets[it2]->Append(mStacks[it]->TakeLast());
						found = true;
						break;
					}
				}
				if (found) 
					break;
			}
		}
		if (!found) {
			for (int it = 0; it < 4; ++it) {
				if (mSlots[it]->Count() > 0) {
					for (int it2 = 0; it2 < 4; ++it2) {
						if (mTargets[it2]->Solves(mSlots[it]->Last())) {
							mTargets[it2]->Append(mSlots[it]->TakeLast());
							found = true;
							break;
						}
					}
					if (found) 
						break;
				}
			}
		}
	} while (found);
	
	solved = true;
	for (int it = 0; it < 4; ++it) {
		if (!mTargets[it]->Solved()) {
			solved = false;
			break;
		}
	}
	if (solved) {
		WriteInfo(tr("You won!"));
		ShowInfo(true);
		WriteMessage(tr("Press OK to start a new game"));
		ShowMessage(true);
		mGameOver = true;
	}
}

bool cFreecellGame::CanMoveTo(int Position) {
	ASSERT(mFloating->Count() == 0);

	if (mStacks[Position]->Accepts(mFloating->First())) {
		int slots = 0, stacks = 0;
		for (int it = 0; it < 4; ++it)
			if (mSlots[it]->Count() == 0)
				++slots;
		for (int it = 0; it < 8; ++it)
			if (it != Position && mStacks[it]->Count() == 0)
				++stacks;
		return (int)mFloating->Count() <= (slots + 1) * (int)pow(2, stacks);
	}
	return false;
}

eOSState cFreecellGame::ProcessKey(eKeys Key) {
	eOSState state = cOsdObject::ProcessKey(Key);
	if (state == osUnknown) {
		if (Key == kBack)
			return osEnd;
		else if (Key == kBlue || (mGameOver && Key == kOk))
			Start();
		else if (mShowHelp && Key == kOk)
			ShowHelp(false);
		else if (!mShowHelp) {
			switch (Key) {
				case kBack:  return osEnd;

				case kLeft:
				case kLeft|k_Repeat:
				case kRight: 
				case kRight|k_Repeat: {
					Key = (eKeys)(Key & ~k_Repeat);
					if (mPosition < 0 && mFloating->Count() == 0) {
						int delta = (Key == kLeft ? 1 : -1);
						int newpos = mPosition + delta;
						while (newpos >= -4 && newpos <= -1) {
							if (mSlots[~newpos]->Count() > 0) {
								mPosition = newpos;
								break;
							}
							newpos += delta;
						}
					} else if (mPosition >= 0) {
						int delta = (Key == kLeft ? -1 : 1);
						int newpos = mPosition + delta;
						while (newpos >= 0 && newpos <= 7) {
							if ((mFloating->Count() == 0 && mStacks[newpos]->Count() > 0)
									|| (mFloating->Count() > 0 && CanMoveTo(newpos))) {
								if (mFloating->Count() == 0) {
									// check if acceptable
								}
								mPosition = newpos;
								mSelectStack = false;
								ShowInfo(false);
								break;
							}
							newpos += delta;
						}
					}
					break;
				}

				case kUp:
				case kUp|k_Repeat:
				case kDown:
				case kDown|k_Repeat: {
					Key = (eKeys)(Key & ~k_Repeat);
					if (mSelectStack && Key == kUp) {
						if (mStacks[mPosition]->Count() > 0 
								&& mStacks[mPosition]->Last()->Accepts(mFloating->First()))
							mFloating->Prepend(mStacks[mPosition]->TakeLast());
					} else if (mSelectStack && Key == kDown) {
						if (mFloating->Count() > 1)
							mStacks[mPosition]->Append(mFloating->TakeFirst());
					} else if (Key == kUp) {
						if (mPosition >= 0 && mFloating->Count() <= 1) {
							for (int it = 0; it < 4; ++it) {
								if (mSlots[it]->Count() != mFloating->Count()) {
									mPosition = ~it;
									break;
								}
							}
						}
					} else if (Key == kDown) {
						if (mPosition < 0) {
							if (mFloating->Count() == 0)
								mPosition = 0;
							else {
								for (int it = 0; it < 8; ++it) {
									if (mStacks[it]->Accepts(mFloating->First())) {
										mPosition = it;
										break;
									}
								}
							}
						}
					}
					break;
				}

				case kOk: {
					if (mSelectStack) {
						mSelectStack = false;
						ShowInfo(false);
					} else if (mPosition < 0 && mFloating->Count() == 0) {
						// leere hand über abgelegter karte
						mFloating->Append(mSlots[~mPosition]->TakeLast());
					} else if (mPosition < 0) {
						// hand mit karte auf leerem ablageplatz
						mSlots[~mPosition]->Append(mFloating->TakeLast());
					} else if (mPosition >= 0 && mFloating->Count() == 0) {
						// leere hand über stapel
						if (mStacks[mPosition]->Count() > 0) {
							mFloating->Append(mStacks[mPosition]->TakeLast());
							if (mStacks[mPosition]->Count() > 0 
									&& mStacks[mPosition]->Last()->Accepts(mFloating->First())) {
								WriteInfo(tr("Select stack"));
								ShowInfo(true);
								mSelectStack = true;
							}
						}
					} else {
						// hand mit karte oder stapel über stapel
						while (mFloating->Count() > 0)
							mStacks[mPosition]->Append(mFloating->TakeFirst());
					}
					break;
				}

				case kRed ... kBlue:
				case k0 ... k9: {
					ShowHelp(true);
					break;
				}

				default: break;
			}
		}
		state = osContinue;
		if (mFloating->Count() == 0)
			AutoSolve();
		Paint();
	}
	return state;
}
