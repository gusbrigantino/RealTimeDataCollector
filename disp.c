//
// COMP/GENG 422 - Gus Brigantino
//
// Display module
//

#include <stdarg.h>
#include <string.h>

#include "main.h"
#include "disp.h"
#include "fonts.h"


#define LCD_RST_DIS()	{GPIOC->BSRR = 0x00000002;}
#define LCD_RST_ENA()	{GPIOC->BSRR = 0x00020000;}

#define LCD_CS_DIS()	{GPIOC->BSRR = 0x00000004;}
#define LCD_CS_ENA()	{GPIOC->BSRR = 0x00040000;}

#define LCD_CMD_ENA()	{GPIOC->BSRR = 0x00080000;}
#define LCD_DAT_ENA()	{GPIOC->BSRR = 0x00000008;}

#define LCD_WR_CLR()	{GPIOC->BSRR = 0x00100000;}
#define LCD_WR_SET()	{GPIOC->BSRR = 0x00000010;}

#define LCD_RD_CLR()	{GPIOC->BSRR = 0x00200000;}
#define LCD_RD_SET()	{GPIOC->BSRR = 0x00000020;}

#define NUM_CHARS_MAX	32	// DispPrintf() maximums - %b:32, %d:10, %x:8


// These static vars are initialized in DispInit()
static uint16_t	CurBackColor;
static uint16_t	CurForeColor;
static int		CurFontType;
static sFONT	CurFontStruct;

// Static vars for DispPrintf() and its helper functions
static int		CurPosX;
static int		CurPosY;
static int		FieldWidth;
static Bool		IsLeftJustified = FALSE;
static Bool		IsMinWidth = FALSE;
static Bool		IsNegative;
static Bool		IsZeroFilled = FALSE;
static int		MinWidth;
static int		OutputCharCount;


static void LeftFill(void);
static void RightFill(void);
static void ColSet(uint16_t StartCol, uint16_t EndCol);
static void RowSet(uint16_t StartRow, uint16_t EndRow);
static void CmdWrite(uint8_t Data);
static void DataWrite(uint8_t Data);


void DispInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable the clocks for GPIO ports A,B,C,D
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	// Configure LCD control lines as outputs, deasserted
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	// LCD reset, active low - PC1
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);

	// LCD chip select, active low - PC2
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);

	// LCD command (low) / data (high) - PC3
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);

	// LCD write data latch, rising edge - PC4
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);

	// LCD read data latch, rising edge - PC5
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

	// Configure LCD data lines as outputs, low
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	//  D7 - PA4
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

	//  D6 - PB1
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

	//  D5 - PB4
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

	//  D4 - PA3
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

	//  D3 - PB0
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

	//  D2 - PD14
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

	//  D1 - PA15
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);

	//  D0 - PB2
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);

	// Reset the LCD
	LCD_RST_ENA();
	HAL_Delay(10);		// 10msec reset pulse width
	LCD_RST_DIS();
	HAL_Delay(120);		// 120msec delay to initialize

	// Assert the LCD chip select (forever)
	LCD_CS_ENA();

	// Data mode is the default, since commands are infrequent relative to data
	LCD_DAT_ENA();

	CmdWrite(0x11);		// sleep out
	HAL_Delay(100);		// 100msec delay to come out of sleep mode

	CmdWrite(0x3A);		// pixel format set
	DataWrite(0x55);	// 16-bit pixel mode (5:6:5)

	// Memory access control
	//
	// D7: MY	row addr order		0 - normal
	// D6: MX	col addr order		0 - normal
	// D5: MV	row/col exchange	1 - exchange row and col
	// D4: ML	vert refresh order	0 - normal
	// D3: BGR	RGB/BGR order		1 - BGR but results in RGB
	// D2: MH	hor refresh order	0 - normal
	// D1: --	reserved			0 - don't care
	// D0: --	reserved			0 - don't care
	CmdWrite(0x36);
	DataWrite(0x28);

	// Fill the entire display with the background color
	CurBackColor = DISP_BLK;		// background color initialized here
	CurForeColor = CurBackColor;	// for this rendering
	DispFilledRectRender(DISP_COL_MIN, DISP_ROW_MIN, DISP_RES_HOR, DISP_RES_VER);

	CurForeColor = DISP_WHT_MAX;	// foreground color initialized here
	CurFontType = DISP_FONT_20;		// font type initialized here
	CurFontStruct = Font20;			// font structure initialized here

	CmdWrite(0x29);		// display on
	HAL_Delay(100);		// 100msec delay to turn the display on
}


void DispBackColorSet(uint16_t Color)
{
	CurBackColor = Color;
}


uint16_t DispBackColorGet(void)
{
	return CurBackColor;
}


void DispForeColorSet(uint16_t Color)
{
	CurForeColor = Color;
}


uint16_t DispForeColorGet(void)
{
	return CurForeColor;
}


void DispFontSet(int Font)
{
	CurFontType = Font;
	switch (Font)
	{
	case DISP_FONT_8:
		CurFontStruct = Font8;
		break;
	case DISP_FONT_12:
		CurFontStruct = Font12;
		break;
	case DISP_FONT_16:
		CurFontStruct = Font16;
		break;
	case DISP_FONT_20:
		CurFontStruct = Font20;
		break;
	case DISP_FONT_24:
		CurFontStruct = Font24;
		break;
	default:
		CurFontType = DISP_FONT_20;
		CurFontStruct = Font20;
		break;
	}
}


int DispFontGet(void)
{
	return CurFontType;
}


uint16_t DispFontHeightGet(void)
{
	return CurFontStruct.Height;
}


uint16_t DispFontWidthGet(void)
{
	return CurFontStruct.Width;
}


int DispFilledRectRender(int PosX, int PosY, int Width, int Height)
{
	int		StartPosX;
	int		EndPosX;
	int		StartPosY;
	int		EndPosY;
	int		PixelCount;
	int		RetVal = DISP_RENDER_RESULT_FULL;

	StartPosX = (PosX > DISP_COL_MIN) ? PosX : DISP_COL_MIN;
	EndPosX = ((PosX + Width - 1) < DISP_COL_MAX) ? (PosX + Width - 1) : DISP_COL_MAX;
	StartPosY = (PosY > DISP_ROW_MIN) ? PosY : DISP_ROW_MIN;
	EndPosY = ((PosY + Height - 1) < DISP_ROW_MAX) ? (PosY + Height - 1) : DISP_ROW_MAX;

	if ((EndPosX < DISP_COL_MIN) || (StartPosX > DISP_COL_MAX) ||
		(EndPosY < DISP_ROW_MIN) || (StartPosY > DISP_ROW_MAX))
	{
		return DISP_RENDER_RESULT_NONE;
	}

	PixelCount = (EndPosX - StartPosX + 1) * (EndPosY - StartPosY + 1);
	if (PixelCount < (Width * Height))
	{
		RetVal = DISP_RENDER_RESULT_PART;
	}

	// Render the rectangle
	ColSet(StartPosX, EndPosX);
	RowSet(StartPosY, EndPosY);
	CmdWrite(0x2C);		// memory write
	while (PixelCount--)
	{
		DataWrite(CurForeColor >> 8);
		DataWrite(CurForeColor);
	}

	return RetVal;
}


int DispCharRender(int PosX, int PosY, char Char)
{
	uint8_t		BitMask;
	int			CurByte;
	uint16_t	CurCol;
	uint16_t	CurRow;
	int			FontColStart;
	int			FontColEnd;
	int			FontRowStart;
	int			FontRowEnd;
	int			FontRowBytes;
	uint8_t *	FontBytePtr;

	// Determine the bounding rectangle for the complete character
	FontColStart = PosX;
	FontColEnd = PosX + CurFontStruct.Width - 1;
	FontRowStart = PosY;
	FontRowEnd = PosY + CurFontStruct.Height - 1;

	// Check if the character is completely outside of the display
	if ((FontColEnd < DISP_COL_MIN) || (FontColStart > DISP_COL_MAX) ||
		(FontRowEnd < DISP_ROW_MIN) || (FontRowStart > DISP_ROW_MAX))
	{
		return DISP_RENDER_RESULT_NONE;
	}

	// If the character is non-printable, convert it to 0x7f
	Char = (Char < 0x20) ? 0x7f : Char;
	Char = (Char > 0x7f) ? 0x7f : Char;

	// Offset the character value by 0x20 to index into the font table
	Char -= 0x20;

	// Constrain the portion of the character to be rendered within the display
	FontColStart = (FontColStart < DISP_COL_MIN) ? DISP_COL_MIN : FontColStart;
	FontColEnd   = (FontColEnd   > DISP_COL_MAX) ? DISP_COL_MAX : FontColEnd;
	FontRowStart = (FontRowStart < DISP_ROW_MIN) ? DISP_ROW_MIN : FontRowStart;
	FontRowEnd   = (FontRowEnd   > DISP_ROW_MAX) ? DISP_ROW_MAX : FontRowEnd;

	// Determine the number of bytes per row and the first byte in the table for the char
	FontRowBytes = (CurFontStruct.Width / 8) + 1;
	FontBytePtr = (uint8_t *) CurFontStruct.table +					// start of table
				  (Char * CurFontStruct.Height * FontRowBytes) +	// start of char
				  ((FontRowStart - PosY) * FontRowBytes);			// first displayed row

	ColSet(FontColStart, FontColEnd);
	RowSet(FontRowStart, FontRowEnd);

	CmdWrite(0x2C);		// memory write

	for (CurRow = FontRowStart; CurRow <= FontRowEnd; CurRow++)
	{
		CurCol = PosX;
		for (CurByte = 0; CurByte < FontRowBytes; CurByte++)
		{
			for (BitMask = 0x80; BitMask; BitMask >>= 1, CurCol++)
			{
				if ((CurCol >= FontColStart) && (CurCol <= FontColEnd))
				{
					DataWrite(((*FontBytePtr) & BitMask ? CurForeColor : CurBackColor) >> 8);
					DataWrite(((*FontBytePtr) & BitMask ? CurForeColor : CurBackColor)     );
				}
			}
			FontBytePtr++;
		}
	}

	// Determine if a partial character was rendered
	if (((FontColEnd - FontColStart + 1) < CurFontStruct.Width) ||
		((FontRowEnd - FontRowStart + 1) < CurFontStruct.Height))
	{
		return DISP_RENDER_RESULT_PART;
	}

	return DISP_RENDER_RESULT_FULL;
}


int DispPrintf(int PosX, int PosY, char * FmtStrPtr, ...)
{
	va_list		ArgPtr;
	Bool		IsHexLower;
	char		NumCharArray[NUM_CHARS_MAX + 1];	// allow for null terminator
	int			NumCharArrayIdx;
	int			NumValCur;
	int			NumValNext;
	char *		StrPtr;

	//Variable argument list start-up
	va_start(ArgPtr, FmtStrPtr);

	// Initialization
	OutputCharCount = 0;
	NumCharArray[NUM_CHARS_MAX] = '\0';		// array will be filled right-to-left
	CurPosX = PosX;
	CurPosY = PosY;

	while(*FmtStrPtr)
	{
		// Print the format string verbatim until the start of a format specifier
		if(*FmtStrPtr != '%')
		{
			DispCharRender(CurPosX, CurPosY, *FmtStrPtr++);
			CurPosX += CurFontStruct.Width;
			OutputCharCount++;
			continue;
		}

		//
		// Format specifier handling
		//

		// Advance to the next char in the format string, reset flags
		FmtStrPtr++;
		MinWidth = 0;
		IsMinWidth = FALSE;
		IsLeftJustified = FALSE;
		IsNegative = FALSE;
		IsZeroFilled = FALSE;
		IsHexLower = FALSE;

		// Left justification
		if(*FmtStrPtr == '-')
		{
			IsLeftJustified = TRUE;
			FmtStrPtr++;
		}

		// Zero fill
		if(*FmtStrPtr == '0')
		{
			IsZeroFilled = TRUE;
			FmtStrPtr++;
		}

		// Minimum field width, in decimal
		while((*FmtStrPtr >= '0') && (*FmtStrPtr <= '9'))
		{
			IsMinWidth = TRUE;
			MinWidth = (MinWidth * 10) + (*FmtStrPtr - '0');
			FmtStrPtr++;
		}

		switch (*FmtStrPtr)
		{
		case 'b':
			NumCharArrayIdx = NUM_CHARS_MAX;
			NumValCur = va_arg(ArgPtr, int);

			do
			{
				NumValNext = (NumValCur >> 1) & 0x7FFFFFFF;	// mask stops propagation of msb=1 in a signed int
				NumValCur &= 0x00000001;					// mask off the least significant bit
				NumCharArray[--NumCharArrayIdx] = '0' + NumValCur;
				NumValCur = NumValNext;
			} while (NumValCur);

			FieldWidth = NUM_CHARS_MAX - NumCharArrayIdx;
			LeftFill();
			while (NumCharArray[NumCharArrayIdx])
			{
				DispCharRender(CurPosX, CurPosY, NumCharArray[NumCharArrayIdx++]);
				CurPosX += CurFontStruct.Width;
				OutputCharCount++;
			}
			RightFill();
			break;

		case 'c':
			FieldWidth = 1;
			LeftFill();
			DispCharRender(CurPosX, CurPosY, va_arg(ArgPtr, int));
			CurPosX += CurFontStruct.Width;
			OutputCharCount++;
			RightFill();
			break;

		case 'd':
			NumCharArrayIdx = NUM_CHARS_MAX;
			NumValCur = va_arg(ArgPtr, int);
			if (NumValCur < 0)
			{
				NumValCur = -NumValCur;
				IsNegative = TRUE;
			}

			do
			{
				NumValNext = NumValCur / 10;
				NumCharArray[--NumCharArrayIdx] = '0' + (NumValCur - (10 * NumValNext));
				NumValCur = NumValNext;
			} while (NumValCur);

			FieldWidth = (NUM_CHARS_MAX - NumCharArrayIdx) + (IsNegative ? 1 : 0);
			LeftFill();
			while (NumCharArray[NumCharArrayIdx])
			{
				DispCharRender(CurPosX, CurPosY, NumCharArray[NumCharArrayIdx++]);
				CurPosX += CurFontStruct.Width;
				OutputCharCount++;
			}
			RightFill();
			break;

		case 's':
			StrPtr = va_arg(ArgPtr, char *);
			FieldWidth = strlen(StrPtr);
			LeftFill();
			while (*StrPtr)
			{
				DispCharRender(CurPosX, CurPosY, *StrPtr++);
				CurPosX += CurFontStruct.Width;
				OutputCharCount++;
			}
			RightFill();
			break;

		case 'x':
			IsHexLower = TRUE;
			// NOTE: lack of break is intentional, flow to next case

		case 'X':
			NumCharArrayIdx = NUM_CHARS_MAX;
			NumValCur = va_arg(ArgPtr, int);

			do
			{
				NumValNext = (NumValCur >> 4) & 0x0FFFFFFF;	// mask stops propagation of msb=1 in a signed int
				NumValCur &= 0x0000000F;					// mask off the least significant hex digit
				if (NumValCur < 10)
				{
					NumCharArray[--NumCharArrayIdx] = '0' + NumValCur;
				}
				else
				{
					NumCharArray[--NumCharArrayIdx] = (IsHexLower ? 'a' : 'A') + (NumValCur - 10);
				}
				NumValCur = NumValNext;
			} while (NumValCur);

			FieldWidth = NUM_CHARS_MAX - NumCharArrayIdx;
			LeftFill();
			while (NumCharArray[NumCharArrayIdx])
			{
				DispCharRender(CurPosX, CurPosY, NumCharArray[NumCharArrayIdx++]);
				CurPosX += CurFontStruct.Width;
				OutputCharCount++;
			}
			RightFill();
			break;

		case '%':
			FieldWidth = 1;
			LeftFill();
			DispCharRender(CurPosX, CurPosY, '%');
			CurPosX += CurFontStruct.Width;
			OutputCharCount++;
			RightFill();
			break;

		default:
			DispCharRender(CurPosX, CurPosY, '%');
			CurPosX += CurFontStruct.Width;
			DispCharRender(CurPosX, CurPosY, *FmtStrPtr++);
			CurPosX += CurFontStruct.Width;
			OutputCharCount += 2;
			break;
		}
		FmtStrPtr++;
	}

	// Variable argument list clean-up
	va_end(ArgPtr);

	return OutputCharCount;
}


static void LeftFill(void)
{
	int	FillCharCount = MinWidth - FieldWidth;

	if (!IsMinWidth || IsLeftJustified || (FillCharCount < 0))
	{
		if (IsNegative)
		{
			DispCharRender(CurPosX, CurPosY, '-');
			CurPosX += CurFontStruct.Width;
			OutputCharCount++;
		}
		return;
	}

	if (IsNegative && IsZeroFilled)
	{
		DispCharRender(CurPosX, CurPosY, '-');
		CurPosX += CurFontStruct.Width;
		OutputCharCount++;
	}

	while (FillCharCount > 0)
	{
		DispCharRender(CurPosX, CurPosY, IsZeroFilled ? '0' : ' ');
		CurPosX += CurFontStruct.Width;
		OutputCharCount++;
		FillCharCount--;
	}

	if (IsNegative && !IsZeroFilled)
	{
		DispCharRender(CurPosX, CurPosY, '-');
		CurPosX += CurFontStruct.Width;
		OutputCharCount++;
	}
}


static void RightFill(void)
{
	int	FillCharCount = MinWidth - FieldWidth;

	if (!IsMinWidth || !IsLeftJustified || (FillCharCount < 0))
	{
		return;
	}

	while (FillCharCount > 0)
	{
		DispCharRender(CurPosX, CurPosY, ' ');
		CurPosX += CurFontStruct.Width;
		OutputCharCount++;
		FillCharCount--;
	}
}


static void ColSet(uint16_t StartCol, uint16_t EndCol)
{
	CmdWrite(0x2A);
	DataWrite(StartCol >> 8);
	DataWrite(StartCol);
	DataWrite(EndCol >> 8);
	DataWrite(EndCol);
}


static void RowSet(uint16_t StartRow, uint16_t EndRow)
{
	CmdWrite(0x2B);
	DataWrite(StartRow >> 8);
	DataWrite(StartRow);
	DataWrite(EndRow >> 8);
	DataWrite(EndRow);
}


static void CmdWrite(uint8_t Data)
{
	LCD_CMD_ENA();	// enable command mode

	LCD_WR_CLR();

	GPIOA->BSRR = (Data & 0x0080) ? 0x00000010 : 0x00100000;
	GPIOB->BSRR = (Data & 0x0040) ? 0x00000002 : 0x00020000;
	GPIOB->BSRR = (Data & 0x0020) ? 0x00000010 : 0x00100000;
	GPIOA->BSRR = (Data & 0x0010) ? 0x00000008 : 0x00080000;
	GPIOB->BSRR = (Data & 0x0008) ? 0x00000001 : 0x00010000;
	GPIOD->BSRR = (Data & 0x0004) ? 0x00004000 : 0x40000000;
	GPIOA->BSRR = (Data & 0x0002) ? 0x00008000 : 0x80000000;
	GPIOB->BSRR = (Data & 0x0001) ? 0x00000004 : 0x00040000;

	LCD_WR_SET();

	LCD_DAT_ENA();	// enable data mode (default)
}


static void DataWrite(uint8_t Data)
{
	// Data mode is the default, no need to enable it
	LCD_WR_CLR();

	GPIOA->BSRR = (Data & 0x0080) ? 0x00000010 : 0x00100000;
	GPIOB->BSRR = (Data & 0x0040) ? 0x00000002 : 0x00020000;
	GPIOB->BSRR = (Data & 0x0020) ? 0x00000010 : 0x00100000;
	GPIOA->BSRR = (Data & 0x0010) ? 0x00000008 : 0x00080000;
	GPIOB->BSRR = (Data & 0x0008) ? 0x00000001 : 0x00010000;
	GPIOD->BSRR = (Data & 0x0004) ? 0x00004000 : 0x40000000;
	GPIOA->BSRR = (Data & 0x0002) ? 0x00008000 : 0x80000000;
	GPIOB->BSRR = (Data & 0x0001) ? 0x00000004 : 0x00040000;

	LCD_WR_SET();
	// Stay in data mode (default)
}
