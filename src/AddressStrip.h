#ifndef ADDRESS_STRIP_H_
#define ADDRESS_STRIP_H_

#include "WS2812B.h"
#include "WS2812B_Utils.h"

#define ADDRESS_STRIP_DEFAULT_NUMDER_LEDS         60
#define ADDRESS_STRIP_DEFAULT_STATE               ADDRESS_STRIP_STATE_ON
#define ADDRESS_STRIP_DEFAULT_MODE                ADDRESS_STRIP_MODE_COLOR
#define ADDRESS_STRIP_DEFAULT_BRIGHTNESS          100
#define ADDRESS_STRIP_DEFAULT_HUE                 0
#define ADDRESS_STRIP_DEFAULT_SATURATION          100

typedef enum AddressStrip_State AddressStrip_State_et;
typedef enum AddressStrip_Mode AddressStrip_Mode_et;
typedef enum AddressStrip_Effect AddressStrip_Effect_et;

typedef struct AddressStrip AddressStrip_st;

enum AddressStrip_State{
	ADDRESS_STRIP_STATE_OFF,
	ADDRESS_STRIP_STATE_ON
};

enum AddressStrip_Mode{
	ADDRESS_STRIP_MODE_NONE,
	ADDRESS_STRIP_MODE_COLOR,
	ADDRESS_STRIP_MODE_EFFECT,
	ADDRESS_STRIP_MODE_COLOR_MUSIC
};

enum AddressStrip_Effect{
	ADDRESS_STRIP_EFFECT_NONE,
	ADDRESS_STRIP_EFFECT_RAINBOW,
};

struct AddressStrip{
	WS2812B_st* ws2812b;
	
	uint16_t numberLeds;
	AddressStrip_State_et state;
	AddressStrip_Mode_et mode;
	uint8_t brightness;
	
	uint16_t hue;
	uint8_t saturation;
	
	AddressStrip_Effect_et effect;
	uint8_t effectSpeed;
	
	uint8_t colorMusic;
	uint8_t colorMusicSpeed;
};

void AddressStrip_Init(AddressStrip_st* strip, WS2812B_st* ws2812b);
void AddressStrip_Run(AddressStrip_st* strip);

void AddressStrip_SetNumberLeds(AddressStrip_st* strip, uint16_t numberLeds);
void AddressStrip_SetState(AddressStrip_st* strip, AddressStrip_State_et state);
void AddressStrip_SetMode(AddressStrip_st* strip, uint8_t mode);
void AddressStrip_SetBrightness(AddressStrip_st* strip, uint8_t brightness);

void AddressStrip_SetHue(AddressStrip_st* strip, uint16_t hue);
void AddressStrip_SetSaturation(AddressStrip_st* strip, uint8_t saturation);

void AddressStrip_SetEffect(AddressStrip_st* strip, uint8_t effect);

#endif