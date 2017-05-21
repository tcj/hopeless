//------------------------------------------
/*		Hopeless
**		(c)2000 Waterade Software
**		(c)2006 Salmon Standard Software
**
**		todo:

			- allocate pens correctly
			- high scores
			- good score display
			- appwindow for image display
			+ testing on native gfx
			- add different game modes
			- add gameover check
			- requester to pick game #
**
*/
//------------------------------------------

#define INTUI_V36_NAMES_ONLY
#define __USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/gadtools.h>
#include <clib/alib_protos.h>
#include <clib/alib_stdio_protos.h>

#include <graphics/gfxmacros.h>
#include <libraries/gadtools.h>
#include <exec/memory.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

// long __stack = 250000L;

// #define SM_NTILES_X	20
// #define SM_NTILES_Y	10
#define WINDOW_TITLE_LEN 78

static char ver[] = "$VER: Hopeless 0.80 (31 Dec 2010)";
static char progName[] = "Hopeless";

// extern UWORD RangeRand (unsigned long maxValue);		// function from amiga.lib

char myboard[21][11];
int TileX, TileY;
int SameLastTime = 0;
int score = 0;

/************************************[Declarations, etc]*******/

#ifdef HL_DEBUG
	struct EasyStruct myES =
	{
		sizeof(struct EasyStruct),
		0,
		"Mouse Clicked!",
		"The mouse was clicked\nx value = %ld\ny value = %ld",
		"OK",
	};
#endif

struct EasyStruct aboutES =
{
	sizeof(struct EasyStruct),
	0,
	"About Hopeless...",
	"Hopeless v0.76\n(c)2000 Waterade\nEmail: tim@adkg.com",
	"OK",
};

struct NewMenu mynewmenu[] =
{
	{ NM_TITLE,		"Game",			0 , 0, 0, 0,},
	{  NM_ITEM,		"New",		  "N", 0, 0, 0,},
	{  NM_ITEM,		"Hi-Scores",  "H", 0, 0, 0,},
	{  NM_ITEM,		NM_BARLABEL,	0 , 0, 0, 0,},
	{  NM_ITEM,		"About...",   "?", 0, 0, 0,},
	{  NM_ITEM,		"Quit",		  "Q", 0, 0, 0,},
	{   NM_END,		NULL,				0 , 0, 0, 0,},
};


/*****************************************************************[ Functions ]******/


/* 
void setupRand(int r[20]) {
   int i, j, k;
   i=time(NULL);
   srand(i);
   for (j=1; j < 21; j++) {
       r[j]=(rand() % 20 + 1);
   }
}
*/

VOID newGame (void)
{
	BYTE x, y;		
	
	score = 0;
	SameLastTime = 0;
	
	for (x = 0; x <= 21; x++)
	{
		myboard[x][0] = 0;
	}
	
	for (y = 0; y <= 11; y++)
	{
		myboard[0][y] = 0;	
	}
	
	for (x=1; x<=20; x++)
	{
		for (y=1; y<=10; y++)	
		{
			myboard[x][y] = RangeRand(6) + 7;							
		}
	}
}

 
VOID checkMatch(int xcood, int ycood, char colr)
{
	int SameThisTime = 1;
	
	if (xcood < 1 || xcood > 20 || ycood < 1 || ycood > 10)
		return;

//	if (myboard[xcood][ycood] = -1)
//		return;
		
  if (myboard[xcood][ycood] != 0) {
    if (xcood < 20) {										// to the right
      if (myboard[xcood+1][ycood] == colr) {
        myboard[xcood+1][ycood] = -1;
        SameThisTime++;
        checkMatch(xcood+1,ycood,colr);
      }
    }
    if (xcood > 1) {											// to the left
      if (myboard[xcood-1][ycood] == colr) {
        myboard[xcood-1][ycood] = -1;
        SameThisTime++;
        checkMatch(xcood-1,ycood,colr);
      }
    }
    if (ycood < 10) {											// going up
      if (myboard[xcood][ycood+1] == colr) {
        myboard[xcood][ycood+1] = -1;
        SameThisTime++;
        checkMatch(xcood,ycood+1,colr);
      }
    }
    if (ycood > 1) {											// going down
      if (myboard[xcood][ycood-1] == colr) {
        myboard[xcood][ycood-1] = -1;
        SameThisTime++;
        checkMatch(xcood,ycood-1,colr);
      }
    }
    if (SameThisTime > 1)
		 SameLastTime += (SameThisTime - 1);
  }
}


/*											blitz basic game-over check   *********************
**  TilesLeft=0
**  GameOverCheck=True
**  For x=0 To 19
**    For y=0 To 9
**      If board(x+1,y+1)>0
**        TilesLeft+1
**      End If
**      If GameOverCheck
**        gx=x+1
**        gy=y+1
**        gb=board(gx,gy)
**        If gb>0
**          If gx < 20
**            If board(gx+1,gy)=gb Then GameOverCheck=False
**          End If
**          If gy <> 10
**            If board(gx,gy+1)=gb Then GameOverCheck=False
**          End If
**        End If
**      End If
**      tmpx=x*TileX
**      tmpy=y*TileY
**      WBox tmpx, tmpy, tmpx+TileX-1, tmpy+TileY-1, board(x+1,y+1)
**    Next
**  Next
....

BOOL gameOverCheck (void) {
	UWORD TilesLeft = 0, 

*/


VOID drawBoard (struct RastPort *rp)
{
	UWORD x,y,x2,y2;

	for (x = 1; x <= 20; x++)
	{
		for (y = 1; y <= 10; y++)
		{
			x2 = (x-1) * TileX;
			y2 = (y-1) * TileY;
			SetAPen(rp, myboard[x][y]);
			RectFill(rp, x2, y2, x2+TileX, y2+TileY);
		}
	}
	
}

//VOID tileDimensions ()
//{
//}

VOID compressBoard (VOID)
{
	char x,y,xii,h,v;

	for (x = 1; x <= 20; x++) {            //  x=1 To 20
		for (y = 1; y <= 10; y++) {          //  y=1 To 10
	      if (myboard[x][y] == -1) {
				myboard[x][y] = 0;
            for (v=y; v >= 1; v--) {      // To 1 Step -1
			      myboard[x][v] = myboard[x][v-1];
				} 
			} 
		} 
	} 

   for (xii = 1; xii <= 19; xii++) {            // xi = 1 To 19
      for (h=19; h >= 1; h--) {              // h=19 To 1 Step -1
         if (myboard[h][10] == 0) {
           for (y=1; y <= 10; y++) {             // y = 1 To 10
              myboard[h][y] = myboard[h+1][y];
              myboard[h+1][y] = 0;
           }
         }
      }
   }
}


VOID processIDCMP(struct Window *win, struct Menu *menuStrip)
{
	WORD done;
	struct IntuiMessage	*msg;
	UWORD msgCode;
	ULONG	msgClass;
	LONG answer = 0L;
	UWORD menuNumber, menuNum, itemNum /*, subNum */;
	struct MenuItem *item;
	int ClickX, ClickY;
	char *windowTitle = NULL;

	done = FALSE;

	windowTitle = (char *)AllocMem( WINDOW_TITLE_LEN, MEMF_PUBLIC | MEMF_CLEAR);
	if (!windowTitle)
		done = TRUE;
		
	while (!done)
	{
		Wait(1L << win->UserPort->mp_SigBit);
		
		while  ( (!done) && 
			(NULL != (msg = (struct IntuiMessage *)GetMsg(win->UserPort))) )
		{
			msgClass = msg->Class;
			msgCode  = msg->Code;

			ReplyMsg((struct Message *)msg);
						
			switch (msgClass)
			{
				case IDCMP_CLOSEWINDOW:
					done = TRUE;
					break;
					
				case IDCMP_MOUSEBUTTONS:
					if (msgCode == SELECTUP) { 
						SameLastTime = 0;

						ClickX = ((msg->MouseX - win->BorderLeft) / TileX) + 1;
						ClickY = ((msg->MouseY - win->BorderTop) / TileY) + 1;
#ifdef HL_DEBUG
	 					answer = EasyRequest(NULL, &myES, NULL, ClickX, ClickY);
#endif
						checkMatch(ClickX, ClickY, myboard[ClickX][ClickY]);
					   if (SameLastTime > 1) {
							compressBoard();
							score += (SameLastTime * SameLastTime);
							sprintf(windowTitle, "Score: %d", score);
							SetWindowTitles(win, windowTitle, (UBYTE *)&progName);
							drawBoard(win->RPort);
						}  
//						if (gameOverCheck) {};
					}
					break;
					
				case IDCMP_REFRESHWINDOW:
					BeginRefresh(win);
//					drawBoard(win->RPort);
					EndRefresh(win, TRUE);
					break;
					
				case IDCMP_NEWSIZE:
					TileX = (win->GZZWidth / 20);
					TileY = (win->GZZHeight / 10);
					drawBoard(win->RPort);
					break;
					
				case IDCMP_MENUPICK:
					menuNumber = msgCode;
					while ((menuNumber != MENUNULL && (!done)))
					{
						item = ItemAddress(menuStrip, menuNumber);
						
						/* process the item here! */
						menuNum = MENUNUM(menuNumber);
						itemNum = ITEMNUM(menuNumber);
//						subNum = SUBNUM(menuNumber);
						

						if ((menuNum == 0) && (itemNum == 0))                    // New Game
							{								
								SetWindowTitles(win, "Hopeless", "Hopeless");								
								newGame();
								drawBoard(win->RPort);								
							};
							
						if ((menuNum == 0) && (itemNum == 1))					// High Scores
							{
							};

						if ((menuNum == 0) && (itemNum == 3))					// About... 
							answer = EasyRequest(NULL, &aboutES, NULL, NULL);
							switch (answer)
							{
								case 0:
									break;
								default:
									break;
							}								

						if ((menuNum == 0) && (itemNum == 4))                    // Quit
							done = TRUE;
							
						menuNumber = item->NextSelect;				
					}
					break;					
			}
		}
	}
	if(windowTitle)
		FreeMem(windowTitle, WINDOW_TITLE_LEN);
}


VOID main(int argc, char **argv)
{
	LONG returnValue;
	struct Window  *win;
	ULONG          *my_VisualInfo;
	struct Menu    *menuStrip;
	
	returnValue=RETURN_FAIL;
	
	if (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 37))
	{
		if (GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 37L))
		{
			if (GadToolsBase = OpenLibrary("gadtools.library", 37))
			{
				if (win=OpenWindowTags(NULL,	
									WA_IDCMP, 			IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_MENUPICK |
															IDCMP_NEWSIZE,      //    | IDCMP_REFRESHWINDOW,
									WA_Title,			"Welcome to Hopeless!",
									WA_InnerWidth,	 	320,		WA_InnerHeight,	180,
									WA_CloseGadget,	TRUE,		WA_Activate,	TRUE,
									WA_DragBar,			TRUE,		WA_AutoAdjust,	TRUE,
									WA_DepthGadget,	TRUE,		WA_SizeGadget,	TRUE,
									WA_SmartRefresh,  FALSE,	WA_SizeBBottom,   TRUE,
									WA_GimmeZeroZero, TRUE,		WA_NewLookMenus,	TRUE,
									WA_MaxWidth,		~0,		WA_MaxHeight,	~0,
									TAG_END))
				{
					if (NULL != (my_VisualInfo = GetVisualInfo(win->WScreen, TAG_END)))
					{
						if (NULL != (menuStrip = CreateMenus(mynewmenu, TAG_END)))
						{
							if (LayoutMenus(menuStrip, my_VisualInfo, GTMN_NewLookMenus, TRUE, TAG_END))
							{
								if (SetMenuStrip(win, menuStrip))
								{		
									TileX = (win->GZZWidth)  / 20;
									TileY = (win->GZZHeight) / 10;
									newGame();
									drawBoard(win->RPort);
									processIDCMP(win, menuStrip);								
									ClearMenuStrip(win);
								}
								FreeMenus(menuStrip);
							}						
						}
						FreeVisualInfo(my_VisualInfo);
					}
					CloseWindow(win);
				}
				CloseLibrary((struct Library *)GadToolsBase);
			}
			CloseLibrary((struct Library *)GfxBase);		
		}
		CloseLibrary((struct Library *)IntuitionBase);
	}
	exit(returnValue);
}

/*      If SameLastTime>1
          For x=1 To 20
            For y=1 To 10
              If board(x,y)=-1
                board(x,y)=0
                For v=y To 1 Step -1
                  board(x,v)=board(x,v-1)
                Next
              End If
            Next
          Next
          score + (SameLastTime*SameLastTime)
          For xi=1 To 19
            For h=19 To 1 Step -1
              If board(h,10)=0
                For y=1 To 10
                  board(h,y)=board(h+1,y)
                  board(h+1,y)=0
                Next
              End If
            Next
          Next
          JSR drawBoard
        End If   */
