#include "WS2812B_Utils.h"

void WS2812B_HSV2RGB(float h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b){
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60, 2) - 1));
    float m = v - c;
    
    float rs = 0;
    float gs = 0;
    float bs = 0;
    
    if(h >= 0 && h < 60){
        rs = c; gs = x; bs = 0;        
    }
    if(h >= 60 && h < 120){
        rs = x; gs = c; bs = 0;        
    }
    if(h >= 120 && h < 180){
        rs = 0; gs = c; bs = x;        
    }
    if(h >= 180 && h < 240){
        rs = 0; gs = x; bs = c;        
    }
    if(h >= 240 && h < 300){
        rs = x; gs = 0; bs = c;        
    }
    if(h >= 300 && h <= 360){
        rs = c; gs = 0; bs = x;        
    }
    
    *r = (rs + m) * 255;
    *g = (gs + m) * 255;
    *b = (bs + m) * 255;
}

float WS2812B_Interpolation(float x1, float x2, float fx1, float fx2, float x){
    return x1 + (x - x1) * ((fx2 - fx1) / (x2 - x1));
}