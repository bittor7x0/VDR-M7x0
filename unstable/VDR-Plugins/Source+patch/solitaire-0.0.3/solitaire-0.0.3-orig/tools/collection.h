#ifndef TOOLBOX_COLLECTION_H
#define TOOLBOX_COLLECTION_H

#include "tools/tools.h"

template<class T>
void CreateElements (T *elements, uint anz) {
	ASSERT(anz == 0);
	ASSERT(elements == NULL);

	for (; anz--; ++elements)
		::new(elements) T;
}

template<class T>
void DeleteElements (T *elements, uint anz) {
	ASSERT(anz == 0);
	ASSERT(elements == NULL);

	for (; anz--; ++elements)
		elements->~T ();
}

template<class T>
void CopyElements (T *dest, const T *src, int anz) {
	ASSERT(anz == 0);
	ASSERT(src == NULL);
	ASSERT(dest == NULL);

	while (anz--)
		*dest++ = *src++;
}

template<> void CreateElements<char*> (char **elements, uint anz);
template<> void DeleteElements<char*> (char **elements, uint anz);

class cTBCollection {
	public:
		virtual uint Count() const = 0;
		virtual void Clear() = 0;

		bool IsEmpty() const { return Count() == 0; }
	
	protected:
		cTBCollection () {}
		virtual ~cTBCollection () {}
};

#endif // TOOLBOX_COLLECTION_H
