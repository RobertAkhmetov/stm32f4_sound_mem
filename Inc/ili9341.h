#define TFT_PORT	GPIOB
#define TFT_SPI		SPI2

#define PIN_LED		GPIO_Pin_0
#define PIN_RST		GPIO_Pin_10
#define PIN_DC		GPIO_Pin_11
#define PIN_CS		GPIO_Pin_12
#define PIN_SCK		GPIO_Pin_13
#define PIN_MISO	GPIO_Pin_14
#define PIN_MOSI	GPIO_Pin_15

// Colors
#define RED			0xf800
#define GREEN		0x07e0
#define BLUE		0x001f
#define BLACK		0x0000
#define YELLOW		0xffe0
#define WHITE		0xffff
#define CYAN		0x07ff
#define BRIGHT_RED	0xf810
#define GRAY1		0x8410
#define GRAY2		0x4208

//TFT resolution 240*320
#define TFT_MIN_X	0
#define TFT_MIN_Y	0
#define TFT_MAX_X	239
#define TFT_MAX_Y	319

#define TFT_FONT_SPACE 6
#define TFT_FONT_X 8
#define TFT_FONT_Y 8


#define TFT_CS_LOW		GPIO_WriteBit(TFT_PORT, PIN_CS, Bit_RESET);
#define TFT_CS_HIGH		GPIO_WriteBit(TFT_PORT, PIN_CS, Bit_SET);
#define TFT_DC_LOW		GPIO_WriteBit(TFT_PORT, PIN_DC, Bit_RESET);
#define TFT_DC_HIGH		GPIO_WriteBit(TFT_PORT, PIN_DC, Bit_SET);
#define TFT_RST_LOW		GPIO_WriteBit(TFT_PORT, PIN_RST, Bit_RESET);
#define TFT_RST_HIGH	GPIO_WriteBit(TFT_PORT, PIN_RST, Bit_SET);


void TFT_sendByte(int data);
void TFT_sendCMD(int index);
void TFT_sendDATA(int data);
void TFT_sendWord(int data);
int TFT_Read_Register(int Addr, int xParameter);
int TFT_readID(void);

void TFT_init();
void TFT_led(int state);
void TFT_setCol(int StartCol, int EndCol);
void TFT_setPage(int StartPage, int EndPage);
void TFT_setXY(int poX, int poY);
int TFT_rgb2color(int R, int G, int B);
void TFT_setPixel(int poX, int poY, int color);

void TFT_clearScreen(void);
void TFT_fillScreen(int XL, int YU, int XR, int YD, int color);
void TFT_fillRectangle(int x0, int y0, int x1, int y1, int color);

void TFT_drawLine(int x0, int y0, int x1, int y1, int color);
void TFT_drawHorizontalLine(int poX, int poY, int length, int color);
void TFT_drawVerticalLine(int poX, int poY, int length, int color);

void TFT_drawCircle(int poX, int poY, int r, int color);
void TFT_fillCircle(int poX, int poY, int r, int color);
void TFT_drawTraingle(int poX1, int poY1, int poX2, int poY2, int poX3, int poY3, int color);

void TFT_drawChar(int ascii, int poX, int poY, int size, int fgcolor);
void TFT_drawString(char *string, int poX, int poY, int size, int fgcolor);

