//
// COMP/GENG 422 - Gus Brigantino
//
// Display module header
//

#ifndef DISP_H_
#define DISP_H_

#define DISP_COL_MIN	0
#define DISP_COL_MAX	479

#define DISP_ROW_MIN	0
#define DISP_ROW_MAX	319

#define DISP_RES_HOR	(DISP_COL_MAX + 1)
#define DISP_RES_VER	(DISP_ROW_MAX + 1)

#define DISP_PIX_TOT	(DISP_RES_HOR * DISP_RES_VER)

#define DISP_BLK		0x0000

#define DISP_RED_MIN	0x0800
#define DISP_RED_MAX	0xf800

#define DISP_GRN_MIN	0x0020
#define DISP_GRN_MAX	0x07e0

#define DISP_BLU_MIN	0x0001
#define DISP_BLU_MAX	0x001f

#define DISP_CYA_MIN	(DISP_BLU_MIN | DISP_GRN_MIN)
#define DISP_CYA_MAX	(DISP_BLU_MAX | DISP_GRN_MAX)

#define DISP_MAG_MIN	(DISP_BLU_MIN | DISP_RED_MIN)
#define DISP_MAG_MAX	(DISP_BLU_MAX | DISP_RED_MAX)

#define DISP_YEL_MIN	(DISP_GRN_MIN | DISP_RED_MIN)
#define DISP_YEL_MAX	(DISP_GRN_MAX | DISP_RED_MAX)

#define DISP_WHT_MIN	(DISP_BLU_MIN | DISP_GRN_MIN | DISP_RED_MIN)
#define DISP_WHT_MAX	(DISP_BLU_MAX | DISP_GRN_MAX | DISP_RED_MAX)

#define DISP_FONT_8		0
#define DISP_FONT_12	1
#define DISP_FONT_16	2
#define DISP_FONT_20	3
#define DISP_FONT_24	4

#define DISP_RENDER_RESULT_FULL	0
#define DISP_RENDER_RESULT_PART	1
#define DISP_RENDER_RESULT_NONE	2

void		DispInit(void);

void		DispBackColorSet(uint16_t Color);
uint16_t	DispBackColorGet(void);
void		DispForeColorSet(uint16_t Color);
uint16_t	DispForeColorGet(void);
void		DispFontSet(int Font);
int			DispFontGet(void);
uint16_t	DispFontHeightGet(void);
uint16_t	DispFontWidthGet(void);

int			DispFilledRectRender(int PosX, int PosY, int Width, int Height);
int 		DispCharRender(int PosX, int PosY, char Char);
int			DispPrintf(int PosX, int PosY, char * FmtStrPtr, ...);

#endif // DISP_H_
