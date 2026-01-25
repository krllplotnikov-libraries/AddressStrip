#ifndef WS2812B_H_
#define WS2812B_H_

#include <stdio.h>
#include <stdint.h>

#define WS2812B_MAX_PIXELS 720

typedef struct WS2812B WS2812B_st;
typedef struct WS2812B_UserFunctions WS2812B_UserFunctions_st;
typedef struct WS2812B_PixelColor WS2812B_PixelColor_st;

typedef void (*WS2812B_SendData)(WS2812B_st* ws2812b, WS2812B_PixelColor_st* pixelColor, uint16_t count);
typedef uint32_t (*WS2812B_GetTick)();

struct WS2812B_PixelColor{
	uint8_t g;
	uint8_t r;
	uint8_t b;
};

struct WS2812B_UserFunctions{
	WS2812B_SendData sendData;
	WS2812B_GetTick getTick;
};

struct WS2812B{
	WS2812B_PixelColor_st pixelColor[WS2812B_MAX_PIXELS]; 
	WS2812B_UserFunctions_st userFunctions;
};

void WS2812B_Init(WS2812B_st* ws2812b, WS2812B_UserFunctions_st* userFunctions);
void WS2812B_Run(WS2812B_st* ws2812b);
void WS2812B_SetColor(WS2812B_st* ws2812b, uint8_t r, uint8_t g, uint8_t b,  uint16_t index);
void WS2812B_GetColor(WS2812B_st* ws2812b, uint8_t* r, uint8_t* g, uint8_t* b,  uint16_t index);

#endif
