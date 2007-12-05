/*
 * See the README file for copyright information and how to reach the author.
 */

#include "patchfont.h"

// most of this function is from the epgsearch-plugin
void PatchFont(eDvbFont DvbFont)
{
 const cFont *font = cFont::GetFont(DvbFont);
 int num_rows_global = font->Height();
 int num_rows = num_rows_global+2;
 int i,j,num_char;
 
 cFont::tPixelData* font_data = new cFont::tPixelData[225*(num_rows)];
 
 for (i = 0; i < 225; i++)
  for (j = 0; j < num_rows; j++)
   font_data[(i*num_rows)+j]=0x0000000000000000;
   
 font_data[0+0]=6;//font->CharData(0)->width;
 font_data[0+1]=num_rows_global;
 
 for(num_char=33, i = 1; num_char<256; i++, num_char++)
 {
  const cFont::tCharData* char_data = font->CharData(num_char);
  font_data[i*num_rows+0] = (num_char>=127 && num_char<=129)?6:char_data->width;
  font_data[i*num_rows+1] = char_data->height;

  // original icons are from the wareagle-icon-patch, modified by myself;
  // the small icons are created from scratch by myself
  if(num_char==128)
  {
   if(DvbFont==fontOsd)
   {
    font_data[i*num_rows+ 0]=16;
    font_data[i*num_rows+ 1]=26;
    font_data[i*num_rows+ 2]=0;      // .... .... .... ....
    font_data[i*num_rows+ 3]=0x7c00; // .### ##.. .... ....
    font_data[i*num_rows+ 4]=0x7e00; // .### ###. .... ....
    font_data[i*num_rows+ 5]=0x6600; // .##. .##. .... ....
    font_data[i*num_rows+ 6]=0x7e00; // .### ###. .... ....
    font_data[i*num_rows+ 7]=0x7800; // .### #... .... ....
    font_data[i*num_rows+ 8]=0x7c00; // .### ##.. .... ....
    font_data[i*num_rows+ 9]=0x7e00; // .### ###. .... ....
    font_data[i*num_rows+10]=0x6400; // .##. ###. .... ....
    font_data[i*num_rows+11]=0x0;    // .... .... .... ....
    font_data[i*num_rows+12]=0x0fc0; // .... #### ##.. ....
    font_data[i*num_rows+13]=0x0fc0; // .... #### ##.. ....
    font_data[i*num_rows+14]=0x0c00; // .... ##.. .... ....
    font_data[i*num_rows+15]=0x0f00; // .... #### .... ....
    font_data[i*num_rows+16]=0x0c00; // .... ##.. .... ....
    font_data[i*num_rows+17]=0x0fc0; // .... #### ##.. ....
    font_data[i*num_rows+18]=0x0fc0; // .... #### ##.. ....
    font_data[i*num_rows+19]=0x0;    // .... .... .... ....
    font_data[i*num_rows+20]=0x0078; // .... .... .### #...
    font_data[i*num_rows+21]=0x00fc; // .... .... #### ##..
    font_data[i*num_rows+22]=0x01cc; // .... ...# ##.. ##..
    font_data[i*num_rows+23]=0x0180; // .... ...# #... ....
    font_data[i*num_rows+24]=0x0180; // .... ...# #... ....
    font_data[i*num_rows+25]=0x01cc; // .... ...# ##.. ##..
    font_data[i*num_rows+26]=0x00fc; // .... .... #### ##..
    font_data[i*num_rows+27]=0x0078; // .... .... .### #...
   }
   else
   {
    font_data[i*num_rows+ 0]=16;
    font_data[i*num_rows+ 1]=20;
    font_data[i*num_rows+ 5]=0x3c00; //..## ##.. .... ....
    font_data[i*num_rows+ 6]=0x3200; //..## ..#. .... ....
    font_data[i*num_rows+ 7]=0x3c00; //..## ##.. .... ....
    font_data[i*num_rows+ 8]=0x3400; //..## .#.. .... ....
    font_data[i*num_rows+ 9]=0x3200; //..## ..#. .... ....
    font_data[i*num_rows+10]=0x0000; //.... .... .... ....
    font_data[i*num_rows+11]=0x07c0; //.... .### ##.. ....
    font_data[i*num_rows+12]=0x0600; //.... .##. .... ....
    font_data[i*num_rows+13]=0x0700; //.... .### .... ....
    font_data[i*num_rows+14]=0x0600; //.... .##. .... ....
    font_data[i*num_rows+15]=0x07c0; //.... .### ##.. ....
    font_data[i*num_rows+16]=0x0000; //.... .... .... ....
    font_data[i*num_rows+17]=0x0078; //.... .... .### #...
    font_data[i*num_rows+18]=0x00c0; //.... .... ##.. ....
    font_data[i*num_rows+19]=0x00c0; //.... .... ##.. ....
    font_data[i*num_rows+20]=0x00c0; //.... .... ##.. ....
    font_data[i*num_rows+21]=0x0078; //.... .... .### #...

   }
  }
  else if(num_char==129)
  {
   if(DvbFont==fontOsd)
   {
    font_data[i*num_rows+ 0]=16;
    font_data[i*num_rows+ 1]=23;
    font_data[i*num_rows+ 8]=0x300c; // ..## .... .... ##..
    font_data[i*num_rows+ 9]=0x73ce; // .### ..## ##.. ###.
    font_data[i*num_rows+10]=0x77ee; // .### .### ###. ###.
    font_data[i*num_rows+11]=0x3c3c; // ..## ##.. ..## ##..
    font_data[i*num_rows+12]=0x1998; // ...# #..# #..# #...
    font_data[i*num_rows+13]=0x318c; // ..## ...# #... ##..
    font_data[i*num_rows+14]=0x318c; // ..## ...# #... ##..
    font_data[i*num_rows+15]=0x30cc; // ..## .... ##.. ##..
    font_data[i*num_rows+16]=0x38dc; // ..## #... ##.# ##..
    font_data[i*num_rows+17]=0x1c38; // ...# ##.. ..## #...
    font_data[i*num_rows+18]=0xff0;  // .... #### #### ....
    font_data[i*num_rows+19]=0xff0;  // .... #### #### ....
    font_data[i*num_rows+20]=0xc30;  // .... ##.. ..## ....
    font_data[i*num_rows+21]=0x1818; // ...# #... ...# #...
    font_data[i*num_rows+22]=0x381c; // ..## #... ...# ##..
    font_data[i*num_rows+23]=0x7c3e; // .### ##.. ..## ###.
    font_data[i*num_rows+24]=0x7c3e; // .### ##.. ..## ###.
   }
   else
   {
    font_data[i*num_rows+ 0]=16;
    font_data[i*num_rows+ 1]=19;
    font_data[i*num_rows+ 6]=0x300c, // ..## .... .... ##..
    font_data[i*num_rows+ 7]=0x73ce, // .### ..## ##.. ###.
    font_data[i*num_rows+ 8]=0x77ee, // .### .### ###. ###.
    font_data[i*num_rows+ 9]=0x3c3c, // ..## ##.. ..## ##..
    font_data[i*num_rows+10]=0x1998, // ...# #..# #..# #...
    font_data[i*num_rows+11]=0x318c; // ..## ...# #... ##..
    font_data[i*num_rows+12]=0x318c; // ..## ...# #... ##..
    font_data[i*num_rows+13]=0x30cc; // ..## .... ##.. ##..
    font_data[i*num_rows+14]=0x38dc; // ..## #... ##.# ##..
    font_data[i*num_rows+15]=0x1c38; // ...# ##.. ..## #...
    font_data[i*num_rows+16]=0xff0;  // .... #### #### ....
    font_data[i*num_rows+17]=0xff0;  // .... #### #### ....
    font_data[i*num_rows+18]=0x1818; // ...# #... ...# #...
    font_data[i*num_rows+19]=0x381c; // ..## #... ...# ##..
    font_data[i*num_rows+20]=0x7c3e; // .### ##.. ..## ###.
   }
  }
  else if(num_char==130)
  {
   if(DvbFont==fontOsd)
   {
    font_data[i*num_rows+ 0]=6;
    font_data[i*num_rows+ 1]=23;
    font_data[i*num_rows+ 8]=0x3f; // ## ####
    font_data[i*num_rows+ 9]=0x3f; // ## ####
    font_data[i*num_rows+10]=0x30; // ## ....
    font_data[i*num_rows+11]=0x30; // ## ....
    font_data[i*num_rows+12]=0x30; // ## ....
    font_data[i*num_rows+13]=0x30; // ## ....
    font_data[i*num_rows+14]=0x30; // ## ....
    font_data[i*num_rows+15]=0x30; // ## ....
    font_data[i*num_rows+16]=0x30; // ## ....
    font_data[i*num_rows+17]=0x30; // ## ....
    font_data[i*num_rows+18]=0x30; // ## ....
    font_data[i*num_rows+19]=0x30; // ## ....
    font_data[i*num_rows+20]=0x30; // ## ....
    font_data[i*num_rows+21]=0x30; // ## ....
    font_data[i*num_rows+22]=0x30; // ## ....
    font_data[i*num_rows+23]=0x3f; // ## ####
    font_data[i*num_rows+24]=0x3f; // ## ####
   }
   else
   {
    font_data[i*num_rows+ 0]=5;
    font_data[i*num_rows+ 1]=19;
    font_data[i*num_rows+ 6]=0x1f; // # ####
    font_data[i*num_rows+ 7]=0x1f; // # ####
    font_data[i*num_rows+ 8]=0x18; // # #...
    font_data[i*num_rows+ 9]=0x18; // # #...
    font_data[i*num_rows+10]=0x18; // # #...
    font_data[i*num_rows+11]=0x18; // # #...
    font_data[i*num_rows+12]=0x18; // # #...
    font_data[i*num_rows+13]=0x18; // # #...
    font_data[i*num_rows+14]=0x18; // # #...
    font_data[i*num_rows+15]=0x18; // # #...
    font_data[i*num_rows+16]=0x18; // # #...
    font_data[i*num_rows+17]=0x18; // # #...
    font_data[i*num_rows+18]=0x18; // # #...
    font_data[i*num_rows+19]=0x1f; // # #...
    font_data[i*num_rows+20]=0x1f; // # ####
   }
  }
  else if(num_char==131)
  {
   if(DvbFont==fontOsd)
   {
    font_data[i*num_rows+ 0]=6;
    font_data[i*num_rows+ 1]=23;
    font_data[i*num_rows+ 8]=0x3f; // ## ####
    font_data[i*num_rows+ 9]=0x3f; // ## ####
    font_data[i*num_rows+10]=0x0;  // .. ....
    font_data[i*num_rows+11]=0x0;  // .. ....
    font_data[i*num_rows+12]=0x0;  // .. ....
    font_data[i*num_rows+13]=0x0;  // .. ....
    font_data[i*num_rows+14]=0x0;  // .. ....
    font_data[i*num_rows+15]=0x0;  // .. ....
    font_data[i*num_rows+16]=0x0;  // .. ....
    font_data[i*num_rows+17]=0x0;  // .. ....
    font_data[i*num_rows+18]=0x0;  // .. ....
    font_data[i*num_rows+19]=0x0;  // .. ....
    font_data[i*num_rows+20]=0x0;  // .. ....
    font_data[i*num_rows+21]=0x0;  // .. ....
    font_data[i*num_rows+22]=0x0;  // .. ....
    font_data[i*num_rows+23]=0x3f; // ## ####
    font_data[i*num_rows+24]=0x3f; // ## ####
   }
   else
   {
    font_data[i*num_rows+ 0]=5;
    font_data[i*num_rows+ 1]=19;
    font_data[i*num_rows+ 6]=0x1f; // # ####
    font_data[i*num_rows+ 7]=0x1f; // # ####
    font_data[i*num_rows+ 8]=0x0;  // . ....
    font_data[i*num_rows+ 9]=0x0;  // . ....
    font_data[i*num_rows+10]=0x0;  // . ....
    font_data[i*num_rows+11]=0x0;  // . ....
    font_data[i*num_rows+12]=0x0;  // . ....
    font_data[i*num_rows+13]=0x0;  // . ....
    font_data[i*num_rows+14]=0x0;  // . ....
    font_data[i*num_rows+15]=0x0;  // . ....
    font_data[i*num_rows+16]=0x0;  // . ....
    font_data[i*num_rows+17]=0x0;  // . ....
    font_data[i*num_rows+18]=0x0;  // . ....
    font_data[i*num_rows+19]=0x1f; // # ####
    font_data[i*num_rows+20]=0x1f; // # ####
   }
  }
  else if(num_char==132)
  {
   if(DvbFont==fontOsd)
   {
    font_data[i*num_rows+ 0]=6;
    font_data[i*num_rows+ 1]=23;
    font_data[i*num_rows+ 8]=0x3f; // ## ####
    font_data[i*num_rows+ 9]=0x3f; // ## ####
    font_data[i*num_rows+10]=0x03; // .. ..##
    font_data[i*num_rows+11]=0x03; // .. ..##
    font_data[i*num_rows+12]=0x03; // .. ..##
    font_data[i*num_rows+13]=0x03; // .. ..##
    font_data[i*num_rows+14]=0x03; // .. ..##
    font_data[i*num_rows+15]=0x03; // .. ..##
    font_data[i*num_rows+16]=0x03; // .. ..##
    font_data[i*num_rows+17]=0x03; // .. ..##
    font_data[i*num_rows+18]=0x03; // .. ..##
    font_data[i*num_rows+19]=0x03; // .. ..##
    font_data[i*num_rows+20]=0x03; // .. ..##
    font_data[i*num_rows+21]=0x03; // .. ..##
    font_data[i*num_rows+22]=0x03; // .. ..##
    font_data[i*num_rows+23]=0x3f; // ## ####
    font_data[i*num_rows+24]=0x3f; // ## ####
   }
   else
   {
    font_data[i*num_rows+ 0]=5;
    font_data[i*num_rows+ 1]=19;
    font_data[i*num_rows+ 6]=0x1f; // # ####
    font_data[i*num_rows+ 7]=0x1f; // # ####
    font_data[i*num_rows+ 8]=0x03; // . ..##
    font_data[i*num_rows+ 9]=0x03; // . ..##
    font_data[i*num_rows+10]=0x03; // . ..##
    font_data[i*num_rows+11]=0x03; // . ..##
    font_data[i*num_rows+12]=0x03; // . ..##
    font_data[i*num_rows+13]=0x03; // . ..##
    font_data[i*num_rows+14]=0x03; // . ..##
    font_data[i*num_rows+15]=0x03; // . ..##
    font_data[i*num_rows+16]=0x03; // . ..##
    font_data[i*num_rows+17]=0x03; // . ..##
    font_data[i*num_rows+18]=0x03; // . ..##
    font_data[i*num_rows+19]=0x1f; // # ####
    font_data[i*num_rows+20]=0x1f; // # ####
   }
  }
  else if(num_char==127)
  {
   if(DvbFont==fontOsd)
   {
    font_data[i*num_rows+ 0]=6;
    font_data[i*num_rows+ 1]=23;
    font_data[i*num_rows+ 8]=0x3f; // ## ####
    font_data[i*num_rows+ 9]=0x3f; // ## ####
    font_data[i*num_rows+10]=0x3f; // ## ####
    font_data[i*num_rows+11]=0x3f; // ## ####
    font_data[i*num_rows+12]=0x3f; // ## ####
    font_data[i*num_rows+13]=0x3f; // ## ####
    font_data[i*num_rows+14]=0x3f; // ## ####
    font_data[i*num_rows+15]=0x3f; // ## ####
    font_data[i*num_rows+16]=0x3f; // ## ####
    font_data[i*num_rows+17]=0x3f; // ## ####
    font_data[i*num_rows+18]=0x3f; // ## ####
    font_data[i*num_rows+19]=0x3f; // ## ####
    font_data[i*num_rows+20]=0x3f; // ## ####
    font_data[i*num_rows+21]=0x3f; // ## ####
    font_data[i*num_rows+22]=0x3f; // ## ####
    font_data[i*num_rows+23]=0x3f; // ## ####
    font_data[i*num_rows+24]=0x3f; // ## ####
   }
   else
   {
    font_data[i*num_rows+ 0]=5;
    font_data[i*num_rows+ 1]=19;
    font_data[i*num_rows+ 6]=0x1f; // # ####
    font_data[i*num_rows+ 7]=0x1f; // # ####
    font_data[i*num_rows+ 8]=0x1f; // # ####
    font_data[i*num_rows+ 9]=0x1f; // # ####
    font_data[i*num_rows+10]=0x1f; // # ####
    font_data[i*num_rows+11]=0x1f; // # ####
    font_data[i*num_rows+12]=0x1f; // # ####
    font_data[i*num_rows+13]=0x1f; // # ####
    font_data[i*num_rows+14]=0x1f; // # ####
    font_data[i*num_rows+15]=0x1f; // # ####
    font_data[i*num_rows+16]=0x1f; // # ####
    font_data[i*num_rows+17]=0x1f; // # ####
    font_data[i*num_rows+18]=0x1f; // # ####
    font_data[i*num_rows+19]=0x1f; // # ####
    font_data[i*num_rows+20]=0x1f; // # ####
   }
  }
  else
  {
   for(j = 0; j<(int)char_data->height; j++)
   {
    cFont::tPixelData Data = (cFont::tPixelData)char_data->lines[j];
    font_data[(i*num_rows)+2+j] = Data;
   }
  }
 }
 
 cFont::SetFont(DvbFont, font_data);
}
