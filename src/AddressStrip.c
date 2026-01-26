#include "AddressStrip.h"
#include <stdint.h>
#include <string.h>

//static void SetPixelHue(AddressStrip_st* strip, uint16_t hue,  uint16_t index);
//static void SetPixelSaturation(AddressStrip_st* strip, uint8_t saturation,  uint16_t index);
//static void SetPixelValue(AddressStrip_st* strip, uint8_t value,  uint16_t index);

static void RunColor(AddressStrip_st* strip);
static void RunEffect(AddressStrip_st* strip);
static void RunColorMusic(AddressStrip_st* strip);
static void RunStateOff(AddressStrip_st* strip);

static void AnimationTrain(AddressStrip_st* strip, uint16_t start, uint16_t stop);
static void AnimationLoop(AddressStrip_st* strip, uint16_t start, uint16_t stop);

static void PresetRainbow(AddressStrip_st* strip, uint16_t start, uint16_t stop);
static void PresetHue(AddressStrip_st* strip, uint16_t start, uint16_t stop, uint16_t hue);
static void PresetRaindrop(AddressStrip_st* strip, uint16_t start, uint16_t stop, uint16_t hue);

static void EffectRainbowLoop(AddressStrip_st* strip);

static void ColorMusicSpectre(AddressStrip_st* strip);

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
	strip->effectSpeed = colorMusicSpeed;
}

uint8_t AddressStrip_IsColorMusicReady(AddressStrip_st* strip){
	return strip->colorMusicSignalFlag;
}

void AddressStrip_UpdateColorMusicSignal(AddressStrip_st* strip, int16_t* colorMusicSignal){
	if(strip->colorMusicSignalFlag){
		memcpy(strip->colorMusicSignal, colorMusicSignal, 256 * sizeof(int16_t));
		strip->colorMusicSignalFlag = 0;
	}
}



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
		case ADDRESS_STRIP_EFFECT_RAINBOW: EffectRainbowLoop(strip);
	}
}

static void RunColorMusic(AddressStrip_st* strip){
	switch (strip->colorMusic){
		case ADDRESS_STRIP_COLOR_MUSIC_NONE: break;
		case ADDRESS_STRIP_COLOR_MUSIC_SPECTRE: ColorMusicSpectre(strip);
	}	
}

static void RunStateOff(AddressStrip_st* strip){
	for(int i = 0; i < strip->numberLeds; i++){
		WS2812B_SetColor(strip->ws2812b, 0, 0, 0, i);
	}	
}



static void AnimationLoop(AddressStrip_st* strip, uint16_t start, uint16_t stop){
	uint8_t r, g, b;
	uint8_t tr, tg, tb;
	WS2812B_GetColor(strip->ws2812b, &tr, &tg, &tb, strip->numberLeds - 1);
	for(uint16_t i = strip->numberLeds - 1; i > 0; i--){
		WS2812B_GetColor(strip->ws2812b, &r, &g, &b, i - 1);
		WS2812B_SetColor(strip->ws2812b, r, g, b, i);
	}
	WS2812B_SetColor(strip->ws2812b, tr, tg, tb, 0);
}

static void AnimationTrain(AddressStrip_st* strip, uint16_t start, uint16_t stop){
	uint8_t r, g, b;
	for(uint16_t i = strip->numberLeds - 1; i > 0; i--){
		WS2812B_GetColor(strip->ws2812b, &r, &g, &b, i - 1);
		WS2812B_SetColor(strip->ws2812b, r, g, b, i);
	}
	WS2812B_SetColor(strip->ws2812b, 0, 0, 0, 0);	
}



static void PresetRainbow(AddressStrip_st* strip, uint16_t start, uint16_t stop){
	for(int i = start; i < stop; i++){
		uint8_t r, g, b;
		WS2812B_HSV2RGB(WS2812B_Interpolation(0, stop - start, 0, 360, i), (float)strip->saturation / 100, (float)strip->brightness / 100, &r, &g, &b);		
		WS2812B_SetColor(strip->ws2812b, r, g, b, i);
	}		
}

static void PresetHue(AddressStrip_st* strip, uint16_t start, uint16_t stop, uint16_t hue){
	for(int i = start; i < stop; i++){
		uint8_t r, g, b;
		WS2812B_HSV2RGB((float)hue, (float)strip->saturation / 100, (float)strip->brightness / 100, &r, &g, &b);		
		WS2812B_SetColor(strip->ws2812b, r, g, b, i);
	}		
}

static void PresetRaindrop(AddressStrip_st* strip, uint16_t start, uint16_t stop, uint16_t hue){
	for(int i = start; i < stop; i++){
		uint8_t r, g, b;
		WS2812B_HSV2RGB((float)hue, (float)strip->saturation / 100, WS2812B_Interpolation(0, stop - start, 0, (float)strip->brightness / 100, i - start + 1), &r, &g, &b);	
		WS2812B_SetColor(strip->ws2812b, r, g, b, i);
	}		
}


static void EffectRainbowLoop(AddressStrip_st* strip){
	if(strip->numberLeds != strip->previousNumberLeds || strip->effect != strip->previousEffect){
		PresetRainbow(strip, 0, strip->numberLeds);
		strip->previousNumberLeds = strip->numberLeds;
		strip->previousEffect = strip->effect;
	}
	
	static uint32_t tick = 0;
	
	if(strip->ws2812b->userFunctions.getTick() - tick > (UINT8_MAX - strip->effectSpeed)){
		AnimationLoop(strip, 0, strip->numberLeds);
		tick = strip->ws2812b->userFunctions.getTick();
	}
	
}




static void ColorMusicSpectre(AddressStrip_st* strip){
	static uint16_t hue = 0;
	
	if(strip->numberLeds != strip->previousNumberLeds || strip->colorMusic != strip->previousColorMusic){
		PresetRaindrop(strip, 0, 5, hue);
		strip->previousNumberLeds = strip->numberLeds;
		strip->previousColorMusic = strip->colorMusic;
	}

	AnimationTrain(strip, 0, strip->numberLeds);

	static uint32_t tick = 0;
	
	//if(strip->ws2812b->userFunctions.getTick() - tick > (UINT8_MAX - strip->colorMusicSpeed)){
		if(!strip->colorMusicSignalFlag){
	      float signal[ADDRESS_STRIP_COLOR_MUSIC_SIGNAL_BUFFER_SIZE];
	      for (int i = 0; i < ADDRESS_STRIP_COLOR_MUSIC_SIGNAL_BUFFER_SIZE; i++) {
	        signal[i] = strip->colorMusicSignal[i];
	      }
	      float rdat[ADDRESS_STRIP_COLOR_MUSIC_SIGNAL_BUFFER_SIZE];
	      float idat[ADDRESS_STRIP_COLOR_MUSIC_SIGNAL_BUFFER_SIZE];
	      float magnitude[ADDRESS_STRIP_COLOR_MUSIC_SIGNAL_BUFFER_SIZE];
	      memset(idat, 0, ADDRESS_STRIP_COLOR_MUSIC_SIGNAL_BUFFER_SIZE * sizeof(float));
	      memcpy(rdat, signal, ADDRESS_STRIP_COLOR_MUSIC_SIGNAL_BUFFER_SIZE * sizeof(float));
	      FFT(rdat, idat, ADDRESS_STRIP_COLOR_MUSIC_SIGNAL_BUFFER_SIZE, FT_INVERSE);
	      for (int i = 0; i < ADDRESS_STRIP_COLOR_MUSIC_SIGNAL_BUFFER_SIZE; i++) {
	        magnitude[i] = sqrtf(rdat[i] * rdat[i] + idat[i] * idat[i]);
	      }		
	      
	      if(magnitude[5] > 3){
			  PresetRaindrop(strip, 0, 5, hue);
			  hue += 6;
			  if(hue > 360){
				  hue = 0;
			  }
		  }
			
			strip->colorMusicSignalFlag = 1;
		}
			
		tick = strip->ws2812b->userFunctions.getTick();
	//}
}