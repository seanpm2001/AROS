/*
    (C) 1997-98 AROS - The Amiga Research OS
    $Id$

    Desc: Support functions for AROSCycleClass.
    Lang: english
*/
#include <strings.h>
#include <exec/types.h>
#include <proto/intuition.h>
#include <intuition/screens.h>
#include <proto/graphics.h>
#include <graphics/rastport.h>
#include <graphics/text.h>
#include <libraries/gadtools.h>

#include "aroscycle_intern.h"


UWORD disabledpattern[2] = {0x4444, 0x1111};

/* draws a disabled pattern */
void drawdisabledpattern(struct CycleBase_intern *AROSCycleBase,
			 struct RastPort *rport, UWORD pen,
			 WORD left, WORD top, UWORD width, UWORD height)
{
    SetABPenDrMd(rport, pen, 0, JAM1);
    rport->AreaPtrn = disabledpattern;
    rport->AreaPtSz = 1;
    RectFill(rport, left, top, left+width-1, top+height-1);
}

#define CYCLEIMAGEWIDTH 19

void renderlabel(struct CycleBase_intern *AROSCycleBase,
                 struct Gadget *gad,
                 STRPTR string,
                 struct RastPort *rport,
                 struct GadgetInfo *ginfo)
{
    UWORD *pens = ginfo->gi_DrInfo->dri_Pens;
    WORD x,y,h;
    int len = strlen(string);

    SetABPenDrMd(rport, pens[TEXTPEN], pens[BACKGROUNDPEN], JAM1);
    Move(rport,
         gad->LeftEdge + (gad->Width - CYCLEIMAGEWIDTH - TextLength(rport, string, len)) / 2,
         gad->TopEdge + (gad->Height - rport->Font->tf_YSize) / 2 + rport->Font->tf_Baseline);
    Text(rport, string, len);
    
    x = gad->LeftEdge + gad->Width - CYCLEIMAGEWIDTH;
    
    /* separator bar */
    
    SetAPen(rport, pens[SHINEPEN]);
    RectFill(rport, x + 1, gad->TopEdge + 2, x + 1, gad->TopEdge + gad->Height - 1 - 2);
    SetAPen(rport, pens[SHADOWPEN]);
    RectFill(rport, x, gad->TopEdge + 2, x, gad->TopEdge + gad->Height - 1 - 2);

    /* cycle image */
    
    h = gad->Height / 2;
    
    x += 6;

    for(y = 0; y < 4; y++)
    {
    	RectFill(rport,x + y,
		       gad->TopEdge + gad->Height - 1 - h - y - 1,
		       x + 6 - y, 
		       gad->TopEdge + gad->Height - 1 - h - y - 1); 
		       
	RectFill(rport,x + y,
		       gad->TopEdge + h + y + 1,
		       x + 6 - y,
		       gad->TopEdge + h + y + 1); 
    }
}
