#include "stm32f10x_conf.h"
#include <math.h>
#include "ili9341.h"
#include "font.h"

//#define TFT_USE_SPI	// SPI mode don't work ((


void TFT_sendByte(int data)
{
	int b, d;

#ifdef TFT_USE_SPI
    while(SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(TFT_SPI, data);
#else
    for(b=0; b<8; b++)
    {
    	d = (data >> (7 - b)) & 0x0001;
        if (d) {
			GPIO_WriteBit(TFT_PORT, PIN_MOSI, Bit_SET);
        }
        else {
			GPIO_WriteBit(TFT_PORT, PIN_MOSI, Bit_RESET);
        }

		GPIO_WriteBit(TFT_PORT, PIN_SCK, Bit_RESET);
        GPIO_WriteBit(TFT_PORT, PIN_SCK, Bit_SET);
    }
#endif
}

void TFT_sendWord(int data)
{
    TFT_DC_HIGH;
    TFT_CS_LOW;

    TFT_sendByte(data >> 8);
    TFT_sendByte(data & 0x00ff);

    TFT_CS_HIGH;
}

void TFT_sendCMD(int index)
{
    TFT_DC_LOW;
    TFT_CS_LOW;

    TFT_sendByte(index);

    TFT_CS_HIGH;
}

void TFT_sendDATA(int data)
{
    TFT_DC_HIGH;
    TFT_CS_LOW;

    TFT_sendByte(data);

    TFT_CS_HIGH;
}

int TFT_Read_Register(int Addr, int xParameter)
{
	int data = 0;

    TFT_sendCMD(0xD9);                                                     // ext command
    TFT_sendByte(0x10+xParameter);                           // 0x11 is the first Parameter
    TFT_DC_LOW;
    TFT_CS_LOW;
    TFT_sendByte(Addr);
    TFT_DC_HIGH;
    while(SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_RXNE)==RESET);
    data = SPI_I2S_ReceiveData(TFT_SPI);
    TFT_CS_HIGH;

    return data;
}

int TFT_readID(void)
{
	int i = 0;
	int byte;
	int data[3] = {0x00, 0x00, 0x00};
	int ID[3] = {0x00, 0x93, 0x41};
	int ToF = 1;

	for(i=0; i<3; i++)
    {
		byte = TFT_Read_Register(0xD3, i+1);
        data[i] = byte;

        if(data[i] != ID[i])
        {
            ToF = 0;
        }
    }

    return ToF;
}

void TFT_init()
{
    int i, TFTDriver = 0;
	GPIO_InitTypeDef gpio;

#ifdef TFT_USE_SPI
	SPI_InitTypeDef spi;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	gpio.GPIO_Pin = PIN_SCK | PIN_MOSI;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(TFT_PORT, &gpio);

	gpio.GPIO_Pin = PIN_MISO;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(TFT_PORT, &gpio);

    gpio.GPIO_Pin = PIN_LED | PIN_DC | PIN_RST | PIN_CS;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(TFT_PORT, &gpio);

	//SPI_StructInit(&spi);
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_NSS = SPI_NSS_Soft;
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; // θημενθλ ρ 128
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_CRCPolynomial = 7;
	SPI_Init(TFT_SPI, &spi);

	SPI_Cmd(TFT_SPI, ENABLE);
	SPI_CalculateCRC(TFT_SPI, DISABLE);
#else
	gpio.GPIO_Pin = PIN_LED | PIN_DC | PIN_RST | PIN_CS | PIN_SCK | PIN_MOSI;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(TFT_PORT, &gpio);

	gpio.GPIO_Pin = PIN_MISO;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(TFT_PORT, &gpio);
#endif

    TFT_CS_HIGH;
    TFT_DC_HIGH;

	TFT_RST_LOW;
	for(i=0; i<0x0FFFF; i++);
	TFT_RST_HIGH;

/*
    for(i=0; i<3; i++)
    {
        TFTDriver = TFT_readID();
    }
*/

    TFT_sendCMD(0xCB);
	TFT_sendDATA(0x39);
	TFT_sendDATA(0x2C);
	TFT_sendDATA(0x00);
	TFT_sendDATA(0x34);
	TFT_sendDATA(0x02);

	TFT_sendCMD(0xCF);
	TFT_sendDATA(0x00);
	TFT_sendDATA(0xC1);
	TFT_sendDATA(0x30);

	TFT_sendCMD(0xE8);
	TFT_sendDATA(0x85);
	TFT_sendDATA(0x00);
	TFT_sendDATA(0x78);

	TFT_sendCMD(0xEA);
	TFT_sendDATA(0x00);
	TFT_sendDATA(0x00);

	TFT_sendCMD(0xED);
	TFT_sendDATA(0x64);
	TFT_sendDATA(0x03);
	TFT_sendDATA(0x12);
	TFT_sendDATA(0x81);

	TFT_sendCMD(0xF7);
	TFT_sendDATA(0x20);

	TFT_sendCMD(0xC0);    	//Power control
	TFT_sendDATA(0x23);   	//VRH[5:0]

	TFT_sendCMD(0xC1);    	//Power control
	TFT_sendDATA(0x10);   	//SAP[2:0];BT[3:0]

	TFT_sendCMD(0xC5);    	//VCM control
	TFT_sendDATA(0x3e);   	//Contrast
	TFT_sendDATA(0x28);

	TFT_sendCMD(0xC7);    	//VCM control2
	TFT_sendDATA(0x86);  	 //--

	TFT_sendCMD(0x36);    	// Memory Access Control
	TFT_sendDATA(0x48);  	//C8	   //48 68η»”ζ §η†//28 E8 ε¦―ξ„η†

	TFT_sendCMD(0x3A);
	TFT_sendDATA(0x55);

	TFT_sendCMD(0xB1);
	TFT_sendDATA(0x00);
	TFT_sendDATA(0x18);

	TFT_sendCMD(0xB6);    	// Display Function Control
	TFT_sendDATA(0x08);
	TFT_sendDATA(0x82);
	TFT_sendDATA(0x27);

	TFT_sendCMD(0xF2);    	// 3Gamma Function Disable
	TFT_sendDATA(0x00);

	TFT_sendCMD(0x26);    	//Gamma curve selected
	TFT_sendDATA(0x01);

	TFT_sendCMD(0xE0);    	//Set Gamma
	TFT_sendDATA(0x0F);
	TFT_sendDATA(0x31);
	TFT_sendDATA(0x2B);
	TFT_sendDATA(0x0C);
	TFT_sendDATA(0x0E);
	TFT_sendDATA(0x08);
	TFT_sendDATA(0x4E);
	TFT_sendDATA(0xF1);
	TFT_sendDATA(0x37);
	TFT_sendDATA(0x07);
	TFT_sendDATA(0x10);
	TFT_sendDATA(0x03);
	TFT_sendDATA(0x0E);
	TFT_sendDATA(0x09);
	TFT_sendDATA(0x00);

	TFT_sendCMD(0xE1);    	//Set Gamma
	TFT_sendDATA(0x00);
	TFT_sendDATA(0x0E);
	TFT_sendDATA(0x14);
	TFT_sendDATA(0x03);
	TFT_sendDATA(0x11);
	TFT_sendDATA(0x07);
	TFT_sendDATA(0x31);
	TFT_sendDATA(0xC1);
	TFT_sendDATA(0x48);
	TFT_sendDATA(0x08);
	TFT_sendDATA(0x0F);
	TFT_sendDATA(0x0C);
	TFT_sendDATA(0x31);
	TFT_sendDATA(0x36);
	TFT_sendDATA(0x0F);

	TFT_sendCMD(0x11);    	//Exit Sleep
	for(i=0; i<0x00FFF; i++);

	TFT_sendCMD(0x29);    //Display on
	TFT_sendCMD(0x2c);

//	TFT_clearScreen();
//	TFT_fillScreen(TFT_MIN_X, TFT_MIN_Y, TFT_MAX_X, TFT_MAX_Y, WHITE);
}

void TFT_led(int state)
{
	if (state)
		GPIO_WriteBit(TFT_PORT, PIN_LED, Bit_SET);
	else
		GPIO_WriteBit(TFT_PORT, PIN_LED, Bit_RESET);
}

void TFT_setCol(int StartCol, int EndCol)
{
	TFT_sendCMD(0x2A);                                                      // Column Command address
	TFT_sendWord(StartCol);
	TFT_sendWord(EndCol);
}

void TFT_setPage(int StartPage, int EndPage)
{
	TFT_sendCMD(0x2B);                                                      // Column Command address
	TFT_sendWord(StartPage);
	TFT_sendWord(EndPage);
}

void TFT_setXY(int poX, int poY)
{
	TFT_setCol(poX, poX);
	TFT_setPage(poY, poY);
	TFT_sendCMD(0x2c);
}

int TFT_rgb2color(int R, int G, int B)
{
	return ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3);
}

void TFT_setPixel(int poX, int poY, int color)
{
	TFT_setXY(poX, poY);
	TFT_sendWord(color);
}

void TFT_clearScreen(void)
{
	int i, max = ((TFT_MAX_X + 1) * (TFT_MAX_Y + 1));

	TFT_setCol(0, TFT_MAX_X);
    TFT_setPage(0, TFT_MAX_Y);
    TFT_sendCMD(0x2c);		// start to write to display ram

    TFT_DC_HIGH;
    TFT_CS_LOW;
    for(i=0; i<max; i++)
    {
    	TFT_sendWord(0);
    }
    TFT_CS_HIGH;
}

void TFT_fillScreen(int XL, int YU, int XR, int YD, int color)
{
	int i;
	int XY = 0;

    if(XL > XR)
    {
    	i = XL;
        XL = XR;
        XR = i;
    }

    if(YU > YD)
    {
    	i = YU;
        YU = YD;
        YD = i;
    }

/*    XL = constrain(XL, TFT_MIN_X, TFT_MAX_X);
    XR = constrain(XR, TFT_MIN_X, TFT_MAX_X);
    YU = constrain(YU, TFT_MIN_Y, TFT_MAX_Y);
    YD = constrain(YD, TFT_MIN_Y, TFT_MAX_Y);
*/
    XY = (XR-XL+1);
    XY = XY*(YD-YU+1);

    TFT_setCol(XL,XR);
    TFT_setPage(YU, YD);
    TFT_sendCMD(0x2c);                                                  /* start to write to display ra */
                                                                        /* m                            */

    TFT_DC_HIGH;
    TFT_CS_LOW;

    int Hcolor = color>>8;
    int Lcolor = color&0xff;
    for(i=0; i < XY; i++)
    {
    	TFT_sendByte(Hcolor);
    	TFT_sendByte(Lcolor);
    }

    TFT_CS_HIGH;
}

void TFT_fillRectangle(int x0, int y0, int x1, int y1, int color)
{
	TFT_fillScreen(x0, y0, x1, y1, color);
}

void TFT_drawLine(int x0, int y0, int x1, int y1, int color)
{
    int x = x1-x0;
    int y = y1-y0;
    int dx = abs(x), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2;                                                // error value e_xy

    for (;;){                                                           // loop
    	TFT_setPixel(x0, y0, color);
        e2 = 2*err;
        if (e2 >= dy) {                                                 // e_xy+e_x > 0
            if (x0 == x1) break;
            err += dy; x0 += sx;
        }
        if (e2 <= dx) {                                                 // e_xy+e_y < 0
            if (y0 == y1) break;
            err += dx; y0 += sy;
        }
    }
}

void TFT_drawHorizontalLine(int poX, int poY, int length, int color)
{
	int i;

	TFT_setCol(poX, poX + length);
	TFT_setPage(poY, poY);
    TFT_sendCMD(0x2c);
    for(i=0; i<length; i++)
    TFT_sendWord(color);
}

void TFT_drawVerticalLine(int poX, int poY, int length, int color)
{
	int i;

	TFT_setCol(poX, poX);
	TFT_setPage(poY, poY+length);
	TFT_sendCMD(0x2c);
    for(i=0; i<length; i++) {
    	TFT_sendWord(color);
    }
}

void TFT_drawCircle(int poX, int poY, int r, int color)
{
    int x = -r, y = 0, err = 2-2*r, e2;

    do {
    	TFT_setPixel(poX-x, poY+y, color);
    	TFT_setPixel(poX+x, poY+y, color);
    	TFT_setPixel(poX+x, poY-y, color);
    	TFT_setPixel(poX-x, poY-y, color);
        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);
}

void TFT_fillCircle(int poX, int poY, int r, int color)
{
    int x = -r, y = 0, err = 2-2*r, e2;

    do {

    	TFT_drawVerticalLine(poX-x, poY-y, 2*y, color);
    	TFT_drawVerticalLine(poX+x, poY-y, 2*y, color);

        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);

}
void TFT_drawTraingle(int poX1, int poY1, int poX2, int poY2, int poX3, int poY3, int color)
{
	TFT_drawLine(poX1, poY1, poX2, poY2, color);
	TFT_drawLine(poX1, poY1, poX3, poY3, color);
	TFT_drawLine(poX2, poY2, poX3, poY3, color);
}

void TFT_drawChar(int ascii, int poX, int poY, int size, int fgcolor)
{
	int i, f;

    if ((ascii>=32) && (ascii<=127))
    {
        ;
    }
    else
    {
        ascii = '?'-32;
    }

    for (i =0; i<TFT_FONT_X; i++ ) {
    	unsigned char temp = Font8x8[ascii-0x20][i];

        for(f=0; f<TFT_FONT_Y; f++)
        {
            if((temp>>f) & 0x01)
            {
                TFT_fillRectangle(poX+i*size, poY+f*size, poX+i*size+size, poY+f*size+size, fgcolor);
            }
        }
    }
}

void TFT_drawString(char *string, int poX, int poY, int size, int fgcolor)
{
    while(*string)
    {
        TFT_drawChar(*string, poX, poY, size, fgcolor);
        *string++;

        if(poX < TFT_MAX_X)
        {
            poX += TFT_FONT_SPACE * size;
        }
    }
}

