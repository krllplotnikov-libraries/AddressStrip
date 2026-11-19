/*
 * Example of the application-side audio processing for AddressStrip color music:
 *   I2S microphone samples -> DC removal -> Hann window -> FFT -> magnitudes ->
 *   8 octave bands (power) -> dB -> adaptive normalization -> 8 floats 0..1
 *
 * The result is passed to AddressStrip_UpdateColorMusicBands().
 *
 * This file is not part of the library: copy it into the application and adapt.
 * It uses the FFT library https://github.com/krllplotnikov-libraries/FFT
 * (on STM32 arm_rfft_fast_f32 + arm_cmplx_mag_f32 from CMSIS-DSP is ~2x faster and needs half the RAM).
 */
#ifndef AUDIO_BANDS_H_
#define AUDIO_BANDS_H_

#include <stdint.h>

#define AUDIO_BANDS_COUNT            8      // must be equal to ADDRESS_STRIP_COLOR_MUSIC_BANDS
#define AUDIO_BANDS_FFT_SIZE         512    // 16 kHz / 512 = 31.25 Hz per bin, 32 ms per frame (~31 frames/s)

#ifndef AUDIO_BANDS_MIN_RANGE_DB
#define AUDIO_BANDS_MIN_RANGE_DB     30.0f  // minimal distance floor..peak, so silence/noise is not stretched to 0..1
#endif
#ifndef AUDIO_BANDS_PEAK_DECAY_DB
#define AUDIO_BANDS_PEAK_DECAY_DB    0.1f   // peak falls by this per frame (~3 dB/s): adapts to a quieter song
#endif
#ifndef AUDIO_BANDS_FLOOR_RISE_DB
#define AUDIO_BANDS_FLOOR_RISE_DB    0.05f  // floor rises by this per frame when the band is above it
#endif
#ifndef AUDIO_BANDS_FLOOR_FALL_DB
#define AUDIO_BANDS_FLOOR_FALL_DB    0.5f   // floor falls by up to this per frame when the band is below it
                                            // (rise/(rise+fall) ~ 9% => floor sits near the 9th percentile of the level)
#endif
#ifndef AUDIO_BANDS_GATE_DB
#define AUDIO_BANDS_GATE_DB          -90.0f // below this (dBFS) the band is treated as silence
#endif

typedef struct{
	float sampleRate;
	uint16_t binStart[AUDIO_BANDS_COUNT];   // first FFT bin of the band (inclusive)
	uint16_t binStop[AUDIO_BANDS_COUNT];    // last FFT bin of the band (exclusive)
	float peakDb[AUDIO_BANDS_COUNT];
	float floorDb[AUDIO_BANDS_COUNT];
	uint8_t started;                        // 0 until the first frame initialized peak/floor
	float window[AUDIO_BANDS_FFT_SIZE];
	float re[AUDIO_BANDS_FFT_SIZE];
	float im[AUDIO_BANDS_FFT_SIZE];
}AudioBands_st;

void AudioBands_Init(AudioBands_st* ab, float sampleRate);

/*
 * samples - AUDIO_BANDS_FFT_SIZE mono samples in -1..1 (full scale)
 * bands   - output, AUDIO_BANDS_COUNT values 0..1
 */
void AudioBands_Process(AudioBands_st* ab, const float* samples, float* bands);

/*
 * INMP441 / SPH0645 style microphones: 24-bit two's complement, left-justified in a 32-bit slot.
 * raw32 - the 32-bit slot value (MSB first). Returns -1..1.
 */
static inline float AudioBands_I2S24ToFloat(int32_t raw32){
	return (float)(raw32 >> 8) / 8388608.0f;
}

#endif
