// Playback sounds in real time, allowing multiple simultaneous wave files
// to be mixed together and played without jitter.
#ifndef AUDIO_MODULE_H
#define AUDIO_MODULE_H

typedef struct {
	int numSamples;
	short *pData;
} wavedata_t;

#define AM_MAX_VOLUME 100

wavedata_t alarmSound;

// init() must be called before any other functions,
// cleanup() must be called last to stop playback threads and free memory.
void AM_init(void);
void AM_cleanup(void);

// Read the contents of a wave file into the pSound structure. Note that
// the pData pointer in this structure will be dynamically allocated in
// readWaveFileIntoMemory(), and is freed by calling freeWaveFileData().
void AM_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound);
void AM_freeWaveFileData(wavedata_t *pSound);

// Queue up another sound bite to play as soon as possible.
void AM_queueSound(wavedata_t *pSound);

// Get/set the volume.
// setVolume() function posted by StackOverflow user "trenki" at:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
int  AM_getVolume();
void AM_setVolume(int newVolume);

#endif
