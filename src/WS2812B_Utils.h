#include <stdint.h>
#include <math.h>

void WS2812B_HSV2RGB(float h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b);
float WS2812B_Interpolation(float x1, float x2, float fx1, float fx2, float x);