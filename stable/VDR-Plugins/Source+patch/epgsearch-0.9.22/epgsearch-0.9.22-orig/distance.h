//---------------------------------------------------
// Levenshtein Distance
// by Michael Gilleland, Merriam Park Software
//
// source:
// http://www.merriampark.com/ld.htm#CPLUSPLUS
//
//---------------------------------------------------

#ifndef _DISTANCE_INC_
#define _DISTANCE_INC_

class Distance
{
  public:
    int LD (char const *s, char const *t, int maxLength);
  private:
    int Minimum (int a, int b, int c);
    int *GetCellPointer (int *pOrigin, int col, int row, int nCols);
    int GetAt (int *pOrigin, int col, int row, int nCols);
    void PutAt (int *pOrigin, int col, int row, int nCols, int x);
}; 

#endif
