#include "WS2812B.h"

void WS2812B_Init(WS2812B_st* ws2812b, WS2812B_UserFunctions_st* userFunctions){
	ws2812b->userFunctions.sendData = userFunctions->sendData;
	ws2812b->userFunctions.getTick = userFunctions->getTick;
}

void WS2812B_Run(WS2812B_st* ws2812b){
	ws2812b->userFunctions.sendData(ws2812b, ws2812b->pixelColor, WS2812B_MAX_PIXELS);
}

void WS2812B_SetColor(WS2812B_st* ws2812b, uint8_t r, uint8_t g, uint8_t b,  uint16_t index){
	ws2812b->pixelColor[index].r = r;
	ws2812b->pixelColor[index].g = g;
	ws2812b->pixelColor[index].b = b;
}

void WS2812B_GetColor(WS2812B_st* ws2812b, uint8_t* r, uint8_t* g, uint8_t* b,  uint16_t index){
	*r = ws2812b->pixelColor[index].r;
	*g = ws2812b->pixelColor[index].g;
	*b = ws2812b->pixelColor[index].b;
}