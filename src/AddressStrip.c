#include "AddressStrip.h"
#include "WS2812B.h"
#include <stdint.h>
#include <string.h>

//static void SetPixelHue(AddressStrip_st* strip, uint16_t hue,  uint16_t index);
//static void SetPixelSaturation(AddressStrip_st* strip, uint8_t saturation,  uint16_t index);
//static void SetPixelValue(AddressStrip_st* strip, uint8_t value,  uint16_t index);

/*--------------------------------------------------------------------------------
                        Local Function Prototype
--------------------------------------------------------------------------------*/
//Run
static void RunColor(AddressStrip_st* strip);
static void RunEffect(AddressStrip_st* strip);
static void RunColorMusic(AddressStrip_st* strip);
static void RunStateOff(AddressStrip_st* strip);

//Preset
static void PresetRainbow(AddressStrip_st* strip, uint16_t start, uint16_t stop);
static void PresetHue(AddressStrip_st* strip, uint16_t start, uint16_t stop, uint16_t hue);
static void PresetRaindrop(AddressStrip_st* strip, uint16_t start, uint16_t stop);

//Animation
static void AnimationTrain(AddressStrip_st* strip, uint16_t start, uint16_t stop);
static void AnimationLoop(AddressStrip_st* strip, uint16_t start, uint16_t stop);

//Effect
static void EffectRainbowLoop(AddressStrip_st* strip);

//ColorMusic
static void ColorMusicSpectre(AddressStrip_st* strip);
static void ColorMusicRainbowTrain(AddressStrip_st* strip);

static void ColorMusicCalculateMagnitudes(AddressStrip_st* strip);



/*--------------------------------------------------------------------------------
                        Global Function Definition
--------------------------------------------------------------------------------*/
void AddressStrip_Init(AddressStrip_st* strip, WS2812B_st* ws2812b){
	strip->ws2812b = ws2812b;
	
	strip->numberLeds = ADDRESS_STRIP_DEFAULT_NUMDER_LEDS;
	strip->state = ADDRESS_STRIP_DEFAULT_STATE;
	strip->mode = ADDRESS_STRIP_DEFAULT_MODE;
	strip->brightness = ADDRESS_STRIP_DEFAULT_BRIGHTNESS;
	strip->hue = ADDRESS_STRIP_DEFAULT_HUE;
	strip->saturation = ADDRESS_STRIP_DEFAULT_SATURATION;
	strip->effect = ADDRESS_STRIP_DEFAULT_EFFECT;
	strip->effectSpeed = ADDRESS_STRIP_DEFAULT_EFFECT_SPEED;
	strip->colorMusic = ADDRESS_STRIP_DEFAULT_COLOR_MUSIC;
	strip->colorMusicSpeed = ADDRESS_STRIP_DEFAULT_COLOR_MUSIC_SPEED;	
	strip->colorMusicSignalFlag = 1;
	
	strip->previousNumberLeds = strip->numberLeds;
	strip->previousEffect = strip->effect;
	
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
		if(strip->mode == ADDRESS_STRIP_MODE_COLOR_MUSIC){
			RunColorMusic(strip);
		}				
	}
	else if(strip->state == ADDRESS_STRIP_STATE_OFF){
		RunStateOff(strip);	
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

void AddressStrip_SetEffect(AddressStrip_st* strip, AddressStrip_Effect_et effect){
	strip->effect = effect;
}

void AddressStrip_SetEffectSpeed(AddressStrip_st* strip, uint8_t effectSpeed){
	strip->effectSpeed = effectSpeed;
}

void AddressStrip_SetColorMusic(AddressStrip_st* strip, AddressStrip_ColorMusic_et colorMusic){
	strip->colorMusic = colorMusic;
}

void AddressStrip_SetColorMusicSpeed(AddressStrip_st* strip, uint8_t colorMusicSpeed){
	strip->colorMusicSpeed = colorMusicSpeed;
}

uint8_t AddressStrip_IsColorMusicReady(AddressStrip_st* strip){
	return strip->colorMusicSignalFlag;
}

void AddressStrip_UpdateColorMusicSignal(AddressStrip_st* strip, int16_t* colorMusicSignal){
	if(strip->colorMusicSignalFlag){
		for (uint32_t i = 0; i < ADDRESS_STRIP_COLOR_MUSIC_SIGNAL_BUFFER_SIZE; i++) {
			strip->colorMusicSignal[i] = colorMusicSignal[i];
		}
		strip->colorMusicSignalFlag = 0;
	}
}



/*--------------------------------------------------------------------------------
                        Local Function Definition
--------------------------------------------------------------------------------*/
//Run
static void RunColor(AddressStrip_st* strip){
	uint8_t r, g, b;
	WS2812B_HSV2RGB((float)strip->hue, (float)strip->saturation / 100, (float)strip->brightness / 100, &r, &g, &b);
	for(int i = 0; i < strip->numberLeds; i++){
		WS2812B_SetColor(strip->ws2812b, r, g, b, i);
	}	
}

static void RunEffect(AddressStrip_st* strip){
	switch (strip->effect){
		case ADDRESS_STRIP_EFFECT_NONE: break;
		case ADDRESS_STRIP_EFFECT_RAINBOW_LOOP: EffectRainbowLoop(strip);
	}
}

static void RunColorMusic(AddressStrip_st* strip){
	switch (strip->colorMusic){
		case ADDRESS_STRIP_COLOR_MUSIC_NONE: break;
		case ADDRESS_STRIP_COLOR_MUSIC_SPECTRE: ColorMusicSpectre(strip);
		case ADDRESS_STRIP_COLOR_MUSIC_RAINBOW_TRAIN: ColorMusicRainbowTrain(strip);
	}	
}

static void RunStateOff(AddressStrip_st* strip){
	for(int i = 0; i < strip->numberLeds; i++){
		WS2812B_SetColor(strip->ws2812b, 0, 0, 0, i);
	}	
}



//Preset
static void PresetRainbow(AddressStrip_st* strip, uint16_t start, uint16_t stop){
	for(int i = start; i <= stop; i++){
		WS2812B_SetColorHSV(strip->ws2812b, WS2812B_Interpolation(0, stop - start, 0, 360, i), (float)strip->saturation / 100, (float)strip->brightness / 100, i);
	}		
}

static void PresetHue(AddressStrip_st* strip, uint16_t start, uint16_t stop, uint16_t hue){
	for(int i = start; i <= stop; i++){
		WS2812B_SetColorHSV(strip->ws2812b, hue, (float)strip->saturation / 100, (float)strip->brightness / 100, i);
	}		
}

static void PresetRaindrop(AddressStrip_st* strip, uint16_t start, uint16_t stop){
	for(int i = start; i <= stop; i++){
		WS2812B_SetValue(strip->ws2812b, WS2812B_Interpolation(0, stop - start, 0, (float)strip->brightness / 100, i - start + 1), i);
	}		
}



//Animation
static void AnimationLoop(AddressStrip_st* strip, uint16_t start, uint16_t stop){
	uint8_t r, g, b;
	uint8_t tr, tg, tb;
	
	if(stop > start){
		WS2812B_GetColor(strip->ws2812b, &tr, &tg, &tb, stop);
		for(uint16_t i = stop; i > start; i--){
			WS2812B_GetColor(strip->ws2812b, &r, &g, &b, i - 1);
			WS2812B_SetColor(strip->ws2812b, r, g, b, i);
		}
		WS2812B_SetColor(strip->ws2812b, tr, tg, tb, start);	
	}
	else{
		WS2812B_GetColor(strip->ws2812b, &tr, &tg, &tb, stop);
		for(uint16_t i = stop; i < start; i++){
			WS2812B_GetColor(strip->ws2812b, &r, &g, &b, i + 1);
			WS2812B_SetColor(strip->ws2812b, r, g, b, i);
		}
		WS2812B_SetColor(strip->ws2812b, tr, tg, tb, start);			
	}
}

static void AnimationTrain(AddressStrip_st* strip, uint16_t start, uint16_t stop){
	uint8_t r, g, b;
	if(stop > start){
		for(uint16_t i = stop; i > start; i--){
			WS2812B_GetColor(strip->ws2812b, &r, &g, &b, i - 1);
			WS2812B_SetColor(strip->ws2812b, r, g, b, i);
		}
		WS2812B_SetColor(strip->ws2812b, 0, 0, 0, start);			
	}
	else{
		for(uint16_t i = stop; i < start; i++){
			WS2812B_GetColor(strip->ws2812b, &r, &g, &b, i + 1);
			WS2812B_SetColor(strip->ws2812b, r, g, b, i);
		}
		WS2812B_SetColor(strip->ws2812b, 0, 0, 0, start);			
	}

}



//Effect
static void EffectRainbowLoop(AddressStrip_st* strip){
	static uint32_t tick = 0;
	
	if(strip->numberLeds != strip->previousNumberLeds || strip->effect != strip->previousEffect){
		PresetRainbow(strip, 0, strip->numberLeds - 1);
		strip->previousNumberLeds = strip->numberLeds;
		strip->previousEffect = strip->effect;
	}
	
	if(strip->ws2812b->userFunctions.getTick() - tick > strip->effectSpeed){
		AnimationLoop(strip, 0, strip->numberLeds - 1);
		
		tick = strip->ws2812b->userFunctions.getTick();
	}
}



//ColorMusic
static void ColorMusicSpectre(AddressStrip_st* strip){
	
}

static void ColorMusicRainbowTrain(AddressStrip_st* strip){
	static uint16_t hue = 0;
	static uint32_t tick = 0;
	
	if(strip->numberLeds != strip->previousNumberLeds || strip->colorMusic != strip->previousColorMusic){
		PresetHue(strip, 0, 5, hue);
		PresetRaindrop(strip, 0, 5);
		strip->previousNumberLeds = strip->numberLeds;
		strip->previousColorMusic = strip->colorMusic;
	}

	if(strip->ws2812b->userFunctions.getTick() - tick > strip->colorMusicSpeed){
		AnimationTrain(strip, 0, strip->numberLeds);
		tick = strip->ws2812b->userFunctions.getTick();
	}
	
	if(!strip->colorMusicSignalFlag){
		ColorMusicCalculateMagnitudes(strip);
		
		if(strip->colorMusicMagnitude[5] > 3){
			PresetHue(strip, 0, 5, hue);
			PresetRaindrop(strip, 0, 5);
			hue += 6;
			if(hue > 360){
				hue = 0;
			}
		}
		strip->colorMusicSignalFlag = 1;
	}
}



static void ColorMusicCalculateMagnitudes(AddressStrip_st* strip){
	FFT_st fft = {0};
	float idat[ADDRESS_STRIP_COLOR_MUSIC_SIGNAL_BUFFER_SIZE] = {0};
			
	FFT_Init(&fft, strip->colorMusicSignal, idat, ADDRESS_STRIP_COLOR_MUSIC_SIGNAL_BUFFER_SIZE);
	FFT_Calculate(&fft, FT_INVERSE);
	FFT_Magnitude(&fft, strip->colorMusicMagnitude);
}