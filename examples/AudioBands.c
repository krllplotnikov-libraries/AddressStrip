#include "AudioBands.h"
#include "FFT.h"
#include <math.h>

#define PI_F 3.14159265358979f

/*
 * Octave band edges, Hz. Band i = [edge[i], edge[i + 1]).
 *   0:   40-80    sub-bass (kick drum body)
 *   1:   80-160   bass
 *   2:  160-320   low-mid
 *   3:  320-640   mid
 *   4:  640-1280  mid
 *   5: 1280-2560  high-mid (vocals, snare)
 *   6: 2560-5120  presence
 *   7: 5120-8000  brilliance (hi-hats); limited by Nyquist = sampleRate / 2
 */
static const float bandEdgesHz[AUDIO_BANDS_COUNT + 1] = {40, 80, 160, 320, 640, 1280, 2560, 5120, 8000};

void AudioBands_Init(AudioBands_st* ab, float sampleRate){
	const uint16_t n = AUDIO_BANDS_FFT_SIZE;
	const uint16_t maxBin = n / 2;   // bins above n/2 mirror the lower ones for a real signal

	ab->sampleRate = sampleRate;

	for(uint16_t i = 0; i < n; i++){
		ab->window[i] = 0.5f * (1.0f - cosf(2.0f * PI_F * i / (n - 1)));   // Hann
	}

	uint16_t previousStop = 1;   // skip bin 0 (DC)
	for(uint8_t b = 0; b < AUDIO_BANDS_COUNT; b++){
		uint16_t start = (uint16_t)lroundf(bandEdgesHz[b] * n / sampleRate);
		uint16_t stop  = (uint16_t)lroundf(bandEdgesHz[b + 1] * n / sampleRate);

		if(start < previousStop) start = previousStop;   // bands must not overlap
		if(stop > maxBin) stop = maxBin;
		if(stop <= start) stop = start + 1;               // at least one bin per band
		if(stop > maxBin + 1) stop = maxBin + 1;

		ab->binStart[b] = start;
		ab->binStop[b] = stop;
		previousStop = stop;

		ab->peakDb[b] = 0.0f;
		ab->floorDb[b] = 0.0f;
	}
	ab->started = 0;   // peak/floor are initialized from the first frame
}

void AudioBands_Process(AudioBands_st* ab, const float* samples, float* bands){
	const uint16_t n = AUDIO_BANDS_FFT_SIZE;

	//1. DC removal: MEMS microphones have a constant offset that would leak into the low bins
	float mean = 0;
	for(uint16_t i = 0; i < n; i++){
		mean += samples[i];
	}
	mean /= n;

	//2. Window: reduces spectral leakage (a loud bass would otherwise "light up" every band)
	for(uint16_t i = 0; i < n; i++){
		ab->re[i] = (samples[i] - mean) * ab->window[i];
		ab->im[i] = 0;
	}

	//3. Forward FFT (FT_DIRECT: no 1/N scaling)
	FFT_st fft;
	FFT_Init(&fft, ab->re, ab->im, n);
	FFT_Calculate(&fft, FT_DIRECT);

	//4. Band power. Scale so that a full-scale sine gives 0 dBFS:
	//   |X[k]| of a sine with amplitude A and Hann window = A * N / 4
	const float scale = 4.0f / n;

	for(uint8_t b = 0; b < AUDIO_BANDS_COUNT; b++){
		float power = 0;
		for(uint16_t k = ab->binStart[b]; k < ab->binStop[b]; k++){
			float re = ab->re[k] * scale;
			float im = ab->im[k] * scale;
			power += re * re + im * im;   // sum of power over the band: octave bands of pink noise come out equal
		}

		//5. Logarithmic scale: loudness perception is logarithmic
		float db = 10.0f * log10f(power + 1e-12f);

		//6. Adaptive normalization (AGC) per band: peak follows loud parts, floor follows quiet parts
		if(!ab->started){
			ab->peakDb[b] = db;
			ab->floorDb[b] = db;
		}
		ab->peakDb[b] -= AUDIO_BANDS_PEAK_DECAY_DB;
		if(db > ab->peakDb[b]) ab->peakDb[b] = db;

		if(db < ab->floorDb[b]){
			float fall = ab->floorDb[b] - db;
			if(fall > AUDIO_BANDS_FLOOR_FALL_DB) fall = AUDIO_BANDS_FLOOR_FALL_DB;
			ab->floorDb[b] -= fall;
		}
		else{
			ab->floorDb[b] += AUDIO_BANDS_FLOOR_RISE_DB;
		}

		float range = ab->peakDb[b] - ab->floorDb[b];
		if(range < AUDIO_BANDS_MIN_RANGE_DB) range = AUDIO_BANDS_MIN_RANGE_DB;

		float value = (db - ab->floorDb[b]) / range;
		if(db < AUDIO_BANDS_GATE_DB) value = 0;
		if(value < 0) value = 0;
		if(value > 1) value = 1;

		bands[b] = value;
	}
	
	ab->started = 1;
}
