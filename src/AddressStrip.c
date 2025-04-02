#include "AddressStrip.h"

static void SetPixelHue(AddressStrip_st* strip, uint16_t hue,  uint16_t index);
static void SetPixelSaturation(AddressStrip_st* strip, uint8_t saturation,  uint16_t index);
static void SetPixelValue(AddressStrip_st* strip, uint8_t value,  uint16_t index);

static void RunColor(AddressStrip_st* strip);
static void RunEffect(AddressStrip_st* strip);
static void RunState(AddressStrip_st* strip);

void AddressStrip_Init(AddressStrip_st* strip, WS2812B_st* ws2812b){
	strip->ws2812b = ws2812b;
	
	strip->numberLeds = ADDRESS_STRIP_DEFAULT_NUMDER_LEDS;
	strip->state = ADDRESS_STRIP_DEFAULT_STATE;
	strip->mode = ADDRESS_STRIP_DEFAULT_MODE;
	strip->brightness = ADDRESS_STRIP_DEFAULT_BRIGHTNESS;
	strip->hue = ADDRESS_STRIP_DEFAULT_HUE;
	strip->saturation = ADDRESS_STRIP_DEFAULT_SATURATION;
	
	RunColor(strip);
}

void AddressStrip_Run(AddressStrip_st* strip){
	if(strip->state == ADDRESS_STRIP_STATE_ON){
		if(strip->mode == ADDRESS_STRIP_MODE_COLOR){
			RunColor(strip);
		}	
		if(strip->mode == ADDRESS_STRIP_MODE_EFFECT){
			RunEffect(strip);
		}		
	}
	else if(strip->state == ADDRESS_STRIP_STATE_OFF){
		if(strip->mode == 0){
			RunState(strip);
		}		
	}
	WS2812B_Run(strip->ws2812b);
}

void AddressStrip_SetNumberLeds(AddressStrip_st* strip, uint16_t numberLeds){
	strip->numberLeds = numberLeds;
	for(int i = 0; i < WS2812B_MAX_PIXELS; i++){
		WS2812B_SetColor(strip->ws2812b, 0, 0, 0, i);
	}	
}

void AddressStrip_SetState(AddressStrip_st* strip, AddressStrip_State_et state){
	strip->state = state;
}

void AddressStrip_SetMode(AddressStrip_st* strip, uint8_t mode){
	strip->mode = mode;
}

void AddressStrip_SetBrightness(AddressStrip_st* strip, uint8_t brightness){
	strip->brightness = brightness;
}



void AddressStrip_SetHue(AddressStrip_st* strip, uint16_t hue){
	strip->hue = hue;
}

void AddressStrip_SetSaturation(AddressStrip_st* strip, uint8_t saturation){
	strip->saturation = saturation;
}




void AddressStrip_SetEffect(AddressStrip_st* strip, uint8_t effect){
	strip->effect = effect;
}



static void RunColor(AddressStrip_st* strip){
	uint8_t r, g, b;
	WS2812B_HSV2RGB((float)strip->hue, (float)strip->saturation / 100, (float)strip->brightness / 100, &r, &g, &b);
	for(int i = 0; i < strip->numberLeds; i++){
		WS2812B_SetColor(strip->ws2812b, r, g, b, i);
	}	
}

static void RunEffect(AddressStrip_st* strip){
	
}

static void RunState(AddressStrip_st* strip){
	for(int i = 0; i < strip->numberLeds; i++){
		WS2812B_SetColor(strip->ws2812b, 0, 0, 0, i);
	}	
}