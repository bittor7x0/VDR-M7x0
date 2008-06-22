// originally from freecell by Sascha Volkenandt (sascha@akv-soft.de)

#include "game.h"
#include "cards.h"
#include "osd.h"
#include "cursor.h"
#include "i18n.h"
#include <math.h>

cSolitaireGame::cSolitaireGame(void): cOsdObject(true) {
	mWidth = 504;
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
	mClosedPile = NULL;
	mOpenPile = NULL;
	mInfo = NULL;
	mMessage = NULL;
	mHelp = NULL;
	memset(mStacks, 0, sizeof(mStacks));
	memset(mTargets, 0, sizeof(mTargets));
	printf("CreatedObject\n");
}

cSolitaireGame::~cSolitaireGame() {
	delete mHelp;
	delete mMessage;
	delete mInfo;
	delete mFloating;
	delete mClosedPile;
	delete mOpenPile;
	for (int it = 0; it < 7; ++it)
		delete mStacks[it];
	for (int it = 0; it < 4; ++it)
		delete mTargets[it];
	delete mCursor;
	delete mOsd;
	printf("DeletedObject\n");
}

void cSolitaireGame::Show(void) {
	printf("Show\n");
#if VDRVERSNUM < 10307
	mOsd = cOsd::OpenRaw(0, 0);
	if (mOsd) {	
		mOsd->Create(mX, mY, mWidth, mHeight, 4, true);
#else /* VDRVERSNUM < 10307 */
	mOsd = cOsdProvider::NewOsd(0, 0);
	if (mOsd) {
		tArea Area[] = { { mX, mY, mX + mWidth - 1, mY + mHeight - 1, 4 } };
		printf("OSD: %d\n",mOsd->CanHandleAreas(Area,1));
		mOsd->SetAreas(Area, sizeof(Area) / sizeof(tArea));
#endif /* VDRVERSNUM < 10307 */
		Start();
		Paint();
	}
}

void cSolitaireGame::Paint(void) {
	ASSERT(mOsd == NULL);
	ASSERT(mPosition < -6 || mPosition > 6);

	int x, y;
	cCardStack *stack = mStacks[0];
	if (mPosition == -1)
		stack = mClosedPile;
	else if (mPosition == -2)
		stack = mOpenPile;
	else if (mPosition < -2)
		stack = mTargets[-mPosition - 3];
	else
		stack = mStacks[mPosition];

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
	mClosedPile->Paint(mOsd);
	mOpenPile->Paint(mOsd);
	for (int it = 0; it < 7; ++it)
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

void cSolitaireGame::Start(void) {
	if (mFirstGame) {
#if VDRVERSNUM >= 10307
		const cFont *font = cFont::GetFont(fontOsd);
#endif /* VDRVERSNUM < 10307 */
		for (int it = 0; it < 7; ++it)
			mStacks[it] = new cCardStack(mX + (cCard::Width() + 1) * it, mY + 98, true);
		mClosedPile = new cCardStack(mX , mY + 1);
		mOpenPile = new cCardStack(mX + cCard::Width() + 1, mY + 1);
		for (int it = 0; it < 4; ++it)
			mTargets[it] = new cCardStack(mX + (cCard::Width() + 1) * (it + 3), mY + 1);
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
		for (int it = 0; it < 7; ++it)
			mStacks[it]->Clear();
		mClosedPile->Clear();
		mOpenPile->Clear();
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

	mClosedPile->Refill();
	mClosedPile->Shuffle();
	int cards = mClosedPile->Count();
	for (int it = 0; it < cards; ++it)
		mClosedPile->At(it)->CloseCard();

	for (int it = 0; it < 7; ++it) {
		for (int it2 = 0; it2 <= it; ++it2) {
			mStacks[it]->Append(mClosedPile->TakeLast());
			mStacks[it]->Last()->CloseCard();
		}
		mStacks[it]->Last()->OpenCard();
	}
}

void cSolitaireGame::WriteInfo(const char *Text) {
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

void cSolitaireGame::WriteMessage(const char *Text) {
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

void cSolitaireGame::WriteHelp(void) {
#if VDRVERSNUM >= 10307
	const cFont *font = cFont::GetFont(fontOsd);
#endif /* VDRVERSNUM < 10307 */
	static const char *HelpText = 
			"This is Solitaire, a card-game for the Video Disk Recorder.\n\n"
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

void cSolitaireGame::AutoSolve(void) {
	bool solved;
	for (int it = 0; it < 7; ++it) {
		if (mStacks[it]->Count() > 0 && mStacks[it]->Last()->Closed())
			mStacks[it]->Last()->OpenCard();
	}

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

bool cSolitaireGame::CanMoveTo(int Position) {
	ASSERT(mFloating->Count() == 0);

	return mStacks[Position]->Accepts(mFloating->First());
}

eOSState cSolitaireGame::ProcessKey(eKeys Key) {
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
					if (mPosition < 0 && mPosition >= -6) {
						int delta = (Key == kLeft ? 1 : -1);
						int newpos = mPosition + delta;
						if (mFloating->Count() == 1) {
							while (newpos <= -3 && newpos >= -6) {
								if (mTargets[-newpos-3]->Solves(mFloating->Last()) || mTargets[-newpos-3]->IsSource()) {
									mPosition = newpos;
									break;
								}
								newpos += delta;
							}
						} else {
							mPosition = newpos;
							if (mPosition < -6)
								mPosition = -6;
							if (mPosition >= 0)
								mPosition = -1;
						}
					} else if (mPosition >= 0 && mPosition <= 6) {
						int delta = (Key == kLeft ? -1 : 1);
						int newpos = mPosition + delta;
						if (mFloating->Count() > 0) {
							while (newpos >= 0 && newpos <= 6) {
								if (mStacks[newpos]->Accepts(mFloating->First()) || mStacks[newpos]->IsSource()) {
									mPosition = newpos;
									break;
								}
								newpos += delta;
							}
							break;
						}
						mPosition = newpos;
						if (mPosition > 6)
							mPosition = 6;
						if (mPosition < 0)
							mPosition = 0;
					}
					break;
				}

				case kUp:
				case kUp|k_Repeat:
				case kDown:
				case kDown|k_Repeat: {
					Key = (eKeys)(Key & ~k_Repeat);
					printf("Pos: %d\n",mPosition);
					if (mSelectStack && Key == kUp) {
						if (mStacks[mPosition]->Count() > 0 
								&& mStacks[mPosition]->Last()->Accepts(mFloating->First()))
							mFloating->Prepend(mStacks[mPosition]->TakeLast());
					} else if (mSelectStack && Key == kDown) {
						if (mFloating->Count() > 1)
							mStacks[mPosition]->Append(mFloating->TakeFirst());
					} else if (Key == kUp) {
						if (mPosition >= 0 && mFloating->Count() == 1) {
							if (mOpenPile->IsSource()) {
								mPosition = -2;
							} else {
								for (int it = -3; it >= -6; --it) {
									if (mTargets[-it-3]->Solves(mFloating->Last())) {
										mPosition = it;
										break;
									}
								}
							}
						} else if (mPosition >= 0 && mFloating->Count() == 0) {
							if (mPosition < 2)
								mPosition = -mPosition - 1;
							else if (mPosition == 2)
								mPosition = -2;
							else
								mPosition = -mPosition;
						}
					} else if (Key == kDown) {
						if (mPosition < 0) {
							if (mFloating->Count() == 0) {
								if (mPosition > -3)
									mPosition = -mPosition - 1;
								else
									mPosition = -mPosition;
							} else {
								for (int it = 0; it < 7; ++it) {
									if (mStacks[it]->Accepts(mFloating->First())) {
										mPosition = it;
										break;
									}
								}
							}
						}
					}
					printf("Pos: %d\n",mPosition);
					break;
				}

				case kOk: {
					if (mSelectStack) {
						mSelectStack = false;
						ShowInfo(false);
					} else if (mPosition == -1 && mFloating->Count() == 0) {
						// leere hand über kartenvorrat (neue karte ziehen)
						if (mClosedPile->Count() == 0) {
							while (mOpenPile->Count() > 0) {
								mClosedPile->Append(mOpenPile->TakeLast());
								mClosedPile->Last()->CloseCard();
							}
						} else {
							int cards;
/*							if (Setup->3Karten) {
								if (mClosedPile->Count() >= 3)
									cards = 3;
								else
									cards = mClosedPile->Count();
							} else
*/								cards = 1;
							for (int it = 0; it < cards; ++it) {
								mOpenPile->Append(mClosedPile->TakeLast());
								mOpenPile->Last()->OpenCard();
							}
						}
					} else if (mPosition == -2 && mFloating->Count() == 0) {
						// leere hand über offenen kartenstapel
						if (mOpenPile->Count() > 0) {
							mFloating->Append(mOpenPile->TakeLast());
							mOpenPile->SetAsSource();
						}
					} else if (mPosition == -2 && mFloating->Count() == 1) {
						// hand mit karte ueber offenen Kartenstapel
						if (mOpenPile->IsSource()) {
							mOpenPile->Append(mFloating->TakeLast());
							ResetSource();
						}
					} else if (mPosition <= -3 && mFloating->Count() == 0) {
						// leere hand über ablageplatz
						if (mTargets[-mPosition-3]->Count() > 0) {
							mFloating->Append(mTargets[-mPosition-3]->TakeLast());
							mTargets[-mPosition-3]->SetAsSource();
						}
					} else if (mPosition <= -3 && mFloating->Count() == 1) {
						// hand mit karte auf ablageplatz
						if (mTargets[-mPosition-3]->Solves(mFloating->Last())) {
							mTargets[-mPosition-3]->Append(mFloating->TakeLast());
							ResetSource();
						}
					} else if (mPosition >= 0 && mFloating->Count() == 0) {
						// leere hand über stapel
						if (mStacks[mPosition]->Count() > 0) {
							mFloating->Append(mStacks[mPosition]->TakeLast());
							mStacks[mPosition]->SetAsSource();
							if (mStacks[mPosition]->Count() > 0 
									&& mStacks[mPosition]->Last()->Accepts(mFloating->First()) && !(mStacks[mPosition]->Last()->Closed())) {
								WriteInfo(tr("Select stack"));
								ShowInfo(true);
								mSelectStack = true;
							}
						}
					} else {
						// hand mit karte oder stapel über stapel
						while (mFloating->Count() > 0) {
							mStacks[mPosition]->Append(mFloating->TakeFirst());
							ResetSource();
						}
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

void cSolitaireGame::ResetSource() {
	mClosedPile->ResetSource();
	mOpenPile->ResetSource();
	for (int it = 0; it < 4; ++it)
		mTargets[it]->ResetSource();
	for (int it = 0; it < 7; ++it)
		mStacks[it]->ResetSource();
}
