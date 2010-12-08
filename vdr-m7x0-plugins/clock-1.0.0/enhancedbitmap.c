/*
 * Clock Plugin for VDR. See http://vdr.aistleitner.info for download.
 * Copyright (C) 2007  Mario Aistleitner <vdr@aistleitner.info>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "enhancedbitmap.h"

#include <string.h>


///////////////////////////////////////////////////////////////////


cEnhancedBitmap::cEnhancedBitmap(int Width, int Height, int Bpp, bool ClearWithBackground) : cBitmap(Width,Height,Bpp,ClearWithBackground) {
	bitmap=(char *)(((cBitmap *)this)->Data(0,0));
}


///////////////////////////////////////////////////////////////////


bool
cEnhancedBitmap::onBitmap(int x, int y)
{
	return (x>=0) && (x<Width()) && (y>=0) && (y<Height());
}


///////////////////////////////////////////////////////////////////


void
cEnhancedBitmap::DrawLine(int x1, int y1, int x2, int y2, tColor Color,bool commit)
{
  if ((onBitmap(x1,y1)) && (onBitmap(x2,y2)))
  {
	char col=Index(Color);
  	int xv;
  	int yv;
	int dirtyX1, dirtyX2, dirtyY1, dirtyY2;

  	xv=x2-x1;
  	yv=y2-y1;
	Dirty(dirtyX1, dirtyX2, dirtyY1, dirtyY2);

  	if (xv*xv>yv*yv) {
		double y,uprise;
		if (x1>x2) {
			int swap;
			swap=x1;x1=x2;x2=swap;
			xv=-xv;
			uprise=-((double)yv)/xv;
			y=y2;
		} else {
			uprise=((double)yv)/xv;
			y=y1;
		}

		for (int x=x1;x<=x2;x++) {
   			bitmap[Width() * ((int)y) + x] = col;
   			y+=uprise;
   		}

  	} else { //  if (abs(xv)>abs(yv))
		double x,uprise,y2width;
		if (y1>y2) {
			int swap;
			swap=y1;y1=y2;y2=swap;
			yv=-yv;
			uprise=-((double)xv)/yv;
			x=x2;
		} else {
			uprise=((double)xv)/yv;
			x=x1;
		}

		y2width=y2*Width();
		for (int y=y1*Width();y<=y2width;y+=Width()) {
	   		bitmap[y + (int)x] = col;
   			x+=uprise;
   		}
  	} //  if (abs(xv)>abs(yv))

  	if (commit) {
		if (x1<x2) {
			if (x1<dirtyX1) dirtyX1=x1;
			if (x2>dirtyX2) dirtyX2=x2;
		} else {
			if (x2<dirtyX1) dirtyX1=x2;
			if (x1>dirtyX2) dirtyX2=x1;
		}
		if (y1<y2) {
			if (y1<dirtyY1) dirtyY1=y1;
			if (y2>dirtyY2) dirtyY2=y2;
		} else {
			if (y2<dirtyY1) dirtyY1=y2;
			if (y1>dirtyY2) dirtyY2=y1;
		}
  	}
  } //if ((onBitmap(x1,y1)) && (onBitmap(x2,y2)))

}


///////////////////////////////////////////////////////////////////


void
cEnhancedBitmap::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3,tColor Color,bool commit)
{
  if ((onBitmap(x1,y1)) && (onBitmap(x2,y2)) && (onBitmap(x3,y3)))
  {
  	char col=Index(Color);
  	int y;	   //y coordinate for points
  	int yend1,yend2; //y coordinates of phase ends
  	double xs,xe;    //start and end for xcoordinates of y
  	double dxs1,dxs2;//change for xs in the two phases
  	double dxe1,dxe2;//change for xs in the two phases
  	int diff;	   //for calculating xe-xs;
        int dirtyX1, dirtyX2, dirtyY1, dirtyY2;
	Dirty(dirtyX1, dirtyX2, dirtyY1, dirtyY2);

	if (y1<y2) {
	  	int swap;
	  	swap=x1;x1=x2;x2=swap;
	  	swap=y1;y1=y2;y2=swap;
  	}
  	if (y3<y2) {
	  	int swap;
	  	swap=x3;x3=x2;x2=swap;
	  	swap=y3;y3=y2;y2=swap;
  	}
  	if (x3<x1) {
	  	int swap;
	  	swap=x3;x3=x1;x1=swap;
	  	swap=y3;y3=y1;y1=swap;
  	}

  	y=y2;
  	if (y1==y2) {
	  	if (y2==y3) {
		  	if (x2<x1) {xs=x2;xe=x1;}
			else       {xs=x1;xe=x2;}
			if (x3<xs) xs=x3;
			else if (x3>xe) xe=x3;
	  		DrawLine((int)xs,y1,(int)xe,y1,Color,commit);
	  		return;
	  	} else {
		  	xs=x1;
	  		xe=x2;
	  		dxs1=0;
	  		dxe1=0;
	  		yend1=0;
	  		dxs2=(double)(x3-x1)/(y3-y1);
	  		dxe2=(double)(x3-x2)/(y3-y2);
	  		yend2=y3;
	  	}
  	} else {
	  	if (y2==y3) {
		  	xs=x2;
	  		xe=x3;
	  		dxs1=0;
	  		dxe1=0;
	  		yend1=0;
	  		dxs2=(double)(x1-x2)/(y1-y2);
	  		dxe2=(double)(x1-x3)/(y1-y3);
	  		yend2=y1;
	  	} else {
	  	  	if (y1>y3) {
			  	xs=x2;
		  		xe=x2;
		  		dxs1=(double)(x1-x2)/(y1-y2);
		  		dxe1=(double)(x3-x2)/(y3-y2);
		  		yend1=y3;
		  		dxs2=dxs1;
		  		dxe2=(double)(x1-x3)/(y1-y3);
		  		yend2=y1;
		  	} else {
			  	xs=x2;
		  		xe=x2;
		  		dxs1=(double)(x1-x2)/(y1-y2);
		  		dxe1=(double)(x3-x2)/(y3-y2);
		  		yend1=y1;
		  		dxs2=(double)(x3-x1)/(y3-y1);
		  		dxe2=dxe1;
		  		yend2=y3;
		  	}
	  	}
  	}

  	//phase 1
  	while (y<yend1) {
	  	diff=((int)xe)-((int)xs) + 1;
	  	memset(bitmap+Width()*y+(int)xs,col, diff<1 ? 1 : diff );
		xs+=dxs1;
		xe+=dxe1;
		y++;
  	}

  	//phase 2
  	while (y<=yend2) {
	  	diff=((int)xe)-((int)xs) + 1;
	  	memset(bitmap+Width()*y+(int)xs,col, diff<1 ? 1 : diff );
		xs+=dxs2;
		xe+=dxe2;
		y++;
  	}

  	if (commit) {
		if (x1>x2) {
			if (x2<dirtyX1) dirtyX1=x2;
		}  else {
			if (x1<dirtyX1) dirtyX1=x1;  	
		}
	       
		if (y2<dirtyY1) dirtyY1=y2;  	
		
		if (x2>x3) {
			if (x2>dirtyX2) dirtyX2=x2;  	
		} else {
			if (x3>dirtyX2) dirtyX2=x3;  	
		}
		
		if (y1>y3) {
			if (y1>dirtyY2) dirtyY2=y1;  	
		} else {
			if (y3>dirtyY2) dirtyY2=y3;  	
		}
		
  	}
  } //if ((onBitmap(x1,y1) && (onBitmap(x2,y2)) && (onBitmap(x3,y3))) 	
}


///////////////////////////////////////////////////////////////////


void
cEnhancedBitmap::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3,tColor Color,bool commit)
{ 
  if ((onBitmap(x1,y1)) && (onBitmap(x2,y2)) && (onBitmap(x3,y3))) 
  {

        int dirtyX1, dirtyX2, dirtyY1, dirtyY2;
	Dirty(dirtyX1, dirtyX2, dirtyY1, dirtyY2);

	DrawLine(x1,y1,x2,y2,Color,false);
  	DrawLine(x1,y1,x3,y3,Color,false);
  	DrawLine(x2,y2,x3,y3,Color,false);
	
  	if (commit) {
	  	int xmin,xmax,ymin,ymax;
	  	xmin=x1;
	  	xmax=x1;
	  	ymin=y1;
	  	ymax=y1;
	  	if (x2<xmin) xmin=x2;
	  	if (x2>xmax) xmax=x2;
	  	if (y2<ymin) ymin=y2;
	  	if (y2>ymax) ymax=y2;
	  	
	  	if (x3<xmin) xmin=x3;
	  	if (x3>xmax) xmax=x3;
	  	if (y3<ymin) ymin=y3;
	  	if (y3>ymax) ymax=y3;
	  	
		if (xmin<dirtyX1) xmin=dirtyX1;
		if (xmax>dirtyX2) xmax=dirtyX2;
	
		if (ymin<dirtyY1) ymin=dirtyY1;
		if (ymax>dirtyY2) ymax=dirtyY2;
  	}
  } //if ((onBitmap(x1,y1) && (onBitmap(x2,y2)) && (onBitmap(x3,y3))) 
}

	
///////////////////////////////////////////////////////////////////


