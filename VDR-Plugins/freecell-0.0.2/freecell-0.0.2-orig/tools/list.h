#ifndef TOOLBOX_LIST_H
#define TOOLBOX_LIST_H

#include "tools/tools.h"
#include "tools/collection.h"
#include "tools/string.h"

template <class T>
class cTBList: public cTBCollection {
protected:
	struct cItem {
		cItem *Next;
		cItem *Prev;
		T      Data;
	};

	cItem *NewItem (cItem *i1, cItem *i2) const;
	void DelItem (cItem *i) const;

private:
	cItem *m_First;
	cItem *m_Last;
	cItem *m_Current;
	int    m_CurPos;
	uint   m_Count;
	bool   m_Delete;

	cItem *Locate (uint idx);
	cItem *Unlink ();
	cItem *GetCurrent () const { return m_Current; }

	void Initialize ();

public:
	cTBList ();
	cTBList (const cTBList<T> &src);
	virtual ~cTBList ();

	void SetAutoDelete(bool on) { m_Delete = on; }

	cTBList<T> &operator= (const cTBList<T> &src);

	uint Count () const { return m_Count; }

	void Append   (const T &obj);
	void Prepend  (const T &obj);
	bool Insert (uint pos, const T &obj);

	T &Append  ();
	T &Prepend ();

	void Append   (const cTBList<T> &src);
	void Prepend  (const cTBList<T> &src);
	bool Insert (uint pos, const cTBList<T> &src);

	bool Remove   (const T &obj);
	bool Remove   ();
	bool Remove (uint idx);

	bool RemoveFirst () { SetFirstCurrent(); return Remove(); }
	bool RemoveLast  () { SetLastCurrent();  return Remove(); }

	T Take      ();
	T Take    (uint idx);

	T TakeFirst () { SetFirstCurrent(); return Take(); }
	T TakeLast  () { SetLastCurrent();  return Take(); }

	void Clear ();

	int Find     (const T &obj);
	int Contains (const T &obj) const;
	
	int Current  () const { return m_CurPos; }

	T  At (uint idx) const;
	T &At (uint idx);

	T  operator[] (uint idx) const { return At(idx); }
	T &operator[] (uint idx) { return At(idx); }

	const T &Get   () const;
	const T &First () const;
	const T &Last  () const;
	const T &Next  () const;
	const T &Prev  () const;
	
	T &SetFirstCurrent ();
	T &SetLastCurrent  ();
	T &SetNextCurrent  ();
	T &SetPrevCurrent  ();

	friend cTBList<T> &operator+= <> (cTBList<T> &list1, const cTBList<T> &list2);
	friend cTBList<T> &operator+= <> (cTBList<T> &list, const T &element);

	friend cTBList<T> operator+ <> (const cTBList<T> &list1, const cTBList<T> &list2);
	friend cTBList<T> operator+ <> (const T &element, const cTBList<T> &list);
	friend cTBList<T> operator+ <> (const cTBList<T> &list, const T &element);

	/*friend cSource &operator<< <> (cSource &dest, const cTBList<T> &list);
	friend cSource &operator>> <> (cSource &dest, cTBList<T> &list);*/
};

// Template Implementation (inline funcs)

template<class T>
inline int cTBList<T>::Find (const T & i) {
	cTBList<T>::cItem *item = m_First;
	int index = 0;

	while (item && (item->Data != i)) {
		item = item->Next;
		index++;
	}

	m_Current = item;
	return (m_CurPos = item ? index : -1);
}

template<class T>
inline int cTBList<T>::Contains (const T &i) const {
	cItem *item = m_First;
	uint Count = 0;

	while (item) {
		if (item->Data == i)
			Count++;
		item = item->Next;
	}
	return Count;
}

template<class T>
inline T &cTBList<T>::At (uint idx) {
	ASSERT(idx >= m_Count);
	return Locate(idx)->Data;
}

template<class T>
inline T cTBList<T>::At (uint idx) const {
	ASSERT(idx >= m_Count);
	return Locate(idx)->Data;
}

template<class T>
inline const T &cTBList<T>::Get () const {
	ASSERT(m_Current == NULL);
	return m_Current->Data;
}

template<class T>
inline const T &cTBList<T>::First () const {
	ASSERT(m_First == NULL);
	return m_First->Data;
}

template<class T>
inline const T &cTBList<T>::Last () const {
	ASSERT(m_Last == NULL);
	return m_Last->Data;
}

template<class T>
inline const T &cTBList<T>::Next () const {
	ASSERT(m_Current == NULL);
	ASSERT(m_Current->Next == NULL);
	return m_Current->Next->Data;
}

template<class T>
inline const T &cTBList<T>::Prev () const {
	ASSERT(m_Current == NULL);
	ASSERT(m_Current->Prev == NULL);
	return m_Current->Prev->Data;
}

// Template implementation (out-of-line)

template<class T>
cTBList<T>::cTBList () : cTBCollection () {
	Initialize();
}

template<class T>
cTBList<T>::cTBList (const cTBList<T> &src) : cTBCollection () {
	Initialize();
	Append(src);
}

template<class T>
cTBList<T>::~cTBList () {
	if (m_Count)
		Clear();
}

template<class T>
void cTBList<T>::Initialize () {
	m_First = m_Last = m_Current = NULL;
	m_Count = 0;
	m_CurPos = -1;
	m_Delete = false;
}

template<class T>
typename cTBList<T>::cItem *cTBList<T>::NewItem (cTBList<T>::cItem *pBefore, cTBList<T>::cItem *pAfter) const {
	cTBList<T>::cItem *item = new cTBList<T>::cItem;

	item->Next = pAfter;
	item->Prev = pBefore;

	if (pBefore)
		pBefore->Next = item;

	if (pAfter)
		pAfter->Prev  = item;

	CreateElements<T>(&item->Data, 1);

	return item;
}

template<class T>
void cTBList<T>::DelItem (cTBList<T>::cItem *item) const {
	ASSERT(item == NULL);
	if (m_Delete)
		delete item->Data;
	DeleteElements<T>(&item->Data, 1);
	delete item;
}

template<class T>
typename cTBList<T>::cItem *cTBList<T>::Locate (uint idx) {
	ASSERT(idx >= m_Count);

	if ((int)idx == m_CurPos)
		return m_Current;

	cTBList<T>::cItem *item;
	int dist = idx - m_CurPos;
	bool direct;

	if (idx >= m_Count)
		return 0;

	if (dist < 0)
		dist = -dist;

	if (((uint)dist < idx) && 
			((uint)dist < m_Count - idx)) {
		item = m_Current;
		direct = idx > (uint)m_CurPos;
	} else if (idx < m_Count - idx) {
		item = m_First;
		dist = idx;
		direct = true;
	} else {
		item = m_Last;
		dist = m_Count - idx - 1;
		if (dist < 0)
			dist = 0;
		direct = false;
	}

	while (dist--)
		item = direct ? item->Next : item->Prev;

	m_Current = item;
	m_CurPos = idx;
	return item;
}

template<class T>
T &cTBList<T>::Prepend () {
	cTBList<T>::cItem *item = NewItem(NULL, m_First);

	m_First = m_Current = item;
	if (!m_Last)
		m_Last = m_First;

	m_Count++;
	m_CurPos = 0;

	return item->Data;
}

template<class T>
T &cTBList<T>::Append () {
	cTBList<T>::cItem *item = NewItem(m_Last, NULL);

	m_Last = m_Current = item;
	if (!m_First)
		m_First = m_Last;

	m_CurPos = m_Count;
	m_Count++;

	return item->Data;
}

template<class T>
void cTBList<T>::Prepend (const T &i) {
	Prepend() = i;
}

template<class T>
void cTBList<T>::Append (const T & i) {
	Append() = i;
}

template<class T>
bool cTBList<T>::Insert (uint idx, const T & i) {
	if (!idx) {
		Prepend(i);
		return true;
	} else if (idx == m_Count) {
		Append(i);
		return true;
	}

	cTBList<T>::cItem *nextItem = Locate(idx);
	if (!nextItem)
		return false;

	cTBList<T>::cItem *prevItem = nextItem->Prev;

	cTBList<T>::cItem *item = NewItem(prevItem, nextItem);
	item->Data = i;

	m_CurPos = idx;
	m_Current = item;
	m_Count++;

	return true;
}

template<class T>
typename cTBList<T>::cItem *cTBList<T>::Unlink () {
	if (!m_Current)
		return NULL;

	cTBList<T>::cItem *item = m_Current;
	if (item == m_First) {
		if (m_First = item->Next)
			m_First->Prev = NULL;
		else {
			m_Last = m_Current = NULL;
			m_CurPos = -1;
		}
	} else if (item == m_Last) {
		m_Last = item->Prev;
		m_Last->Next = NULL;
	} else {
		item->Prev->Next = item->Next;
		item->Next->Prev = item->Prev;
	}

	if (item->Next)
		m_Current = item->Next;
	else if (item->Prev) {
		m_Current = item->Prev;
		m_CurPos--;
	}
	m_Count--;
	return item;
}

template<class T>
bool cTBList<T>::Remove (const T & i) {
	if (i)
		if (Find(i) == -1)
			return false;

	cTBList<T>::cItem *item = Unlink();
	if (!item)
		return false;

	DelItem(item);
	return true;
}

template<class T>
bool cTBList<T>::Remove (uint idx) {
	if (!Locate(idx))
		return false;

	cTBList<T>::cItem *item = Unlink();
	if (!item)
		return false;

	DelItem(item);
	return true;
}

template<class T>
T cTBList<T>::Take () {
	ASSERT(m_Current == NULL);

	cItem *item = Unlink();
	T data = item->Data;
	DelItem(item);
	return data;
}

template<class T>
T cTBList<T>::Take (uint idx) {
	ASSERT(idx >= m_Count);

	Locate(idx);

	return Take();
}

template<class T>
void cTBList<T>::Clear () {
	cTBList<T>::cItem *item = m_First;

	m_First = m_Last = m_Current = NULL;
	m_Count = 0;
	m_CurPos = -1;

	cTBList<T>::cItem *Prev;
	while (item) {
		Prev = item;
		item = item->Next;
		DelItem(Prev);
	}
}

template<class T>
T &cTBList<T>::SetFirstCurrent () {
	ASSERT(m_First == NULL);
	m_CurPos = 0;
	return (m_Current = m_First)->Data;
}

template<class T>
T &cTBList<T>::SetLastCurrent () {
	ASSERT(m_Last == NULL);
	m_CurPos = m_Count - 1;
	return (m_Current = m_Last)->Data;
}

template<class T>
T &cTBList<T>::SetNextCurrent () {
	ASSERT(m_Current == NULL);
	ASSERT(m_Current->Next == NULL);
	m_CurPos++;
	return (m_Current = m_Current->Next)->Data;
}

template<class T>
T & cTBList<T>::SetPrevCurrent () {
	ASSERT(m_Current == NULL);
	ASSERT(m_Current->Prev == NULL);
	m_CurPos--;
	return (m_Current = m_Current->Prev)->Data;
}

template<class T>
void cTBList<T>::Append (const cTBList<T> &src) {
	ASSERT(src.m_First == NULL);
	for (cItem *Next = src.m_First; Next; Next = Next->Next)
		Append(Next->Data);
}

template<class T>
void cTBList<T>::Prepend (const cTBList<T> &src) {
	ASSERT(src.m_First == NULL);
	for (cItem *Next = src.m_First; Next; Next = Next->Next)
		Prepend(Next->Data);
}

template<class T>
bool cTBList<T>::Insert (uint idx, const cTBList<T> &src) {
	ASSERT(src.m_First == NULL);

	if (idx >= m_Count)
		return false;

	for (cItem *Next = src.m_First; Next; Next = Next->Next)
		Insert(idx, Next->Data);
}

// Operators (all inline)

template<class T>
inline cTBList<T> &cTBList<T>::operator= (const cTBList<T> &src) {
	Clear();
	Append(src);
	return *this;
}

template<class T>
inline cTBList<T> &operator+= (cTBList<T> &list1, const cTBList<T> &list2) {
	list1.Append(list2);
	return list1;
}

template<class T>
inline cTBList<T> &operator+= (cTBList<T> &list, const T &element) {
	list.Append(element);
	return list;
}

template<class T>
inline cTBList<T> operator+ (const cTBList<T> &list1, const cTBList<T> &list2) {
	cTBList<T> neu = list1;
	neu.Append(list2);
	return neu;
}

template<class T>
inline cTBList<T> operator+ (const T &element, const cTBList<T> &list) {
	cTBList<T> neu = list;
	neu.Prepend(element);
	return neu;
}

template<class T>
inline cTBList<T> operator+ (const cTBList<T> &list, const T &element) {
	cTBList<T> neu = list;
	neu.Append(element);
	return neu;
}

typedef class cTBList<cTBString> cStringList;
typedef class cTBList<void*> cPtrList;

// Special Overrides for String lists
template<class T>
inline cTBList<T> &operator+= (cTBList<T> &src, const char *item); // Normally forbidden

template<>
inline cTBList<cTBString> &operator+= <cTBString> (cTBList<cTBString> &src, const char *item) {
	src.Append((cTBString)item);
	return src;
}

// Special Overrides for Lists of Lists
template<class T>
inline cTBList<cTBList<T> > &operator+= (cTBList<cTBList<T> > &src, const T &item) {
	src.m_Last->Append(item);
	return src;
}

template<class T>
inline cTBList<cTBList<T> > operator+ (const cTBList<cTBList<T> > &src, const T &item) {
	cTBList<cTBList<T> > neu = src;
	neu.m_Last->Append(item);
	return neu;
}

template<class T>
inline cTBList<cTBList<T> > operator+ (const T &item, const cTBList<cTBList<T> > &list) {
	cTBList<cTBList<T> > neu = list;
	list.m_Last->Append(item);
	return neu;
}

#endif // TOOLBOX_LIST_H
