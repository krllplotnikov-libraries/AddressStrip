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
#define ADDRESS_STRIP_DEFAULT_EFFECT              ADDRESS_STRIP_EFFECT_NONE
#define ADDRESS_STRIP_DEFAULT_EFFECT_SPEED        25
#define ADDRESS_STRIP_DEFAULT_COLOR_MUSIC         ADDRESS_STRIP_COLOR_MUSIC_NONE
#define ADDRESS_STRIP_DEFAULT_COLOR_MUSIC_SPEED   5

/*
 * Color music input: ADDRESS_STRIP_COLOR_MUSIC_BANDS octave bands, each a float in 0..1.
 * 0 - silence / noise floor, 1 - loudest level of this band in the recent past.
 * Values are per analysis frame (not smoothed): smoothing and beat detection are done here.
 * See README.md and examples/AudioBands.c for how to compute them.
 */
#define ADDRESS_STRIP_COLOR_MUSIC_BANDS           8

/*
 * Beat (onset) detection per band, on the envelope x:
 *   excess    = min(max(BEAT_DELTA, BEAT_K * deviation), BEAT_PEAK_RATIO * (peak - average))
 *   threshold = average + excess
 *   beat      = x crosses threshold upwards and x > BEAT_MIN
 *   the band is re-armed when x falls below average + (threshold - average) * BEAT_HYSTERESIS
 * Tunables can be overridden from the build (-D...).
 */
#ifndef ADDRESS_STRIP_COLOR_MUSIC_ATTACK
#define ADDRESS_STRIP_COLOR_MUSIC_ATTACK          0.6f   // envelope rise per frame, 0..1
#endif
#ifndef ADDRESS_STRIP_COLOR_MUSIC_RELEASE
#define ADDRESS_STRIP_COLOR_MUSIC_RELEASE         0.15f  // envelope fall per frame, 0..1
#endif
#ifndef ADDRESS_STRIP_COLOR_MUSIC_AVERAGE
#define ADDRESS_STRIP_COLOR_MUSIC_AVERAGE         0.05f  // average/deviation update per frame (~20 frames memory)
#endif
#ifndef ADDRESS_STRIP_COLOR_MUSIC_BEAT_DELTA
#define ADDRESS_STRIP_COLOR_MUSIC_BEAT_DELTA      0.15f  // minimal excess over the average
#endif
#ifndef ADDRESS_STRIP_COLOR_MUSIC_BEAT_K
#define ADDRESS_STRIP_COLOR_MUSIC_BEAT_K          2.5f   // excess in units of mean absolute deviation
#endif
#ifndef ADDRESS_STRIP_COLOR_MUSIC_BEAT_MIN
#define ADDRESS_STRIP_COLOR_MUSIC_BEAT_MIN        0.40f  // minimal absolute level for a beat
#endif
#ifndef ADDRESS_STRIP_COLOR_MUSIC_BEAT_PEAK_RATIO
#define ADDRESS_STRIP_COLOR_MUSIC_BEAT_PEAK_RATIO 0.6f   // threshold excess is at most this part of (peak - average)
#endif
#ifndef ADDRESS_STRIP_COLOR_MUSIC_PEAK_DECAY
#define ADDRESS_STRIP_COLOR_MUSIC_PEAK_DECAY      0.01f  // band peak falls by this per frame
#endif
#ifndef ADDRESS_STRIP_COLOR_MUSIC_BEAT_HYSTERESIS
#define ADDRESS_STRIP_COLOR_MUSIC_BEAT_HYSTERESIS 0.5f   // re-arm level, fraction of the threshold excess
#endif

typedef enum AddressStrip_State AddressStrip_State_et;
typedef enum AddressStrip_Mode AddressStrip_Mode_et;
typedef enum AddressStrip_Effect AddressStrip_Effect_et;
typedef enum AddressStrip_ColorMusic AddressStrip_ColorMusic_et;

typedef struct AddressStrip AddressStrip_st;
typedef struct AddressStrip_ColorMusicBand AddressStrip_ColorMusicBand_st;

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
	ADDRESS_STRIP_EFFECT_RAINBOW_LOOP,
};

enum AddressStrip_ColorMusic{
	ADDRESS_STRIP_COLOR_MUSIC_NONE,
	ADDRESS_STRIP_COLOR_MUSIC_SPECTRE,
	ADDRESS_STRIP_COLOR_MUSIC_RAINBOW_TRAIN,
};

struct AddressStrip_ColorMusicBand{
	float value;     // last input value, 0..1
	float envelope;  // smoothed value (fast attack, slow release), 0..1
	float average;   // long-term average, reference level for beat detection
	float deviation; // long-term mean absolute deviation from average
	float peak;      // recent maximum (slowly decaying)
	uint8_t above;   // value is above the beat threshold
	uint8_t beat;    // 1 - beat started in the last frame (rising edge of "above")
};

struct AddressStrip{
	WS2812B_st* ws2812b;
	
	uint16_t numberLeds;
	uint16_t previousNumberLeds;
	
	AddressStrip_State_et state;
	AddressStrip_State_et previousState;
	
	AddressStrip_Mode_et mode;
	AddressStrip_Mode_et previousMode;
	
	uint8_t brightness;
	uint8_t previousBrightness;
	
	uint16_t hue;
	uint8_t saturation;
	
	AddressStrip_Effect_et effect;
	AddressStrip_Effect_et previousEffect;
	uint8_t effectSpeed;
	
	AddressStrip_ColorMusic_et colorMusic;
	AddressStrip_ColorMusic_et previousColorMusic;
	uint8_t colorMusicSpeed;
	
	float colorMusicInput[ADDRESS_STRIP_COLOR_MUSIC_BANDS];
	volatile uint8_t colorMusicNewFrame;
	AddressStrip_ColorMusicBand_st colorMusicBand[ADDRESS_STRIP_COLOR_MUSIC_BANDS];
};

void AddressStrip_Init(AddressStrip_st* strip, WS2812B_st* ws2812b);
void AddressStrip_Run(AddressStrip_st* strip);

void AddressStrip_SetNumberLeds(AddressStrip_st* strip, uint16_t numberLeds);
void AddressStrip_SetState(AddressStrip_st* strip, AddressStrip_State_et state);
void AddressStrip_SetMode(AddressStrip_st* strip, uint8_t mode);
void AddressStrip_SetBrightness(AddressStrip_st* strip, uint8_t brightness);

void AddressStrip_SetHue(AddressStrip_st* strip, uint16_t hue);
void AddressStrip_SetSaturation(AddressStrip_st* strip, uint8_t saturation);

void AddressStrip_SetEffect(AddressStrip_st* strip, AddressStrip_Effect_et effect);
void AddressStrip_SetEffectSpeed(AddressStrip_st* strip, uint8_t effectSpeed);

void AddressStrip_SetColorMusic(AddressStrip_st* strip, AddressStrip_ColorMusic_et colorMusic);
void AddressStrip_SetColorMusicSpeed(AddressStrip_st* strip, uint8_t colorMusictSpeed);

/*
 * Pass one frame of band levels (ADDRESS_STRIP_COLOR_MUSIC_BANDS floats, 0..1).
 * Call it from the same context as AddressStrip_Run (main loop / same RTOS task).
 * If a new frame arrives before AddressStrip_Run processed the previous one, the previous one is replaced.
 */
void AddressStrip_UpdateColorMusicBands(AddressStrip_st* strip, const float* bands);

#endif