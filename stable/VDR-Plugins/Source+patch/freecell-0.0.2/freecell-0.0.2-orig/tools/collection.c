#include "tools/tools.h"
#include "tools/collection.h"

// Special implementations for String-Pointer
template<> void CreateElements<char*> (char **elements, uint anz) {}
template<> void DeleteElements<char*> (char **elements, uint anz) {}
