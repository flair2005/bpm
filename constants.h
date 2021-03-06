#ifndef CONFIG_H
#define CONFIG_H

// Define project dir folders
#define PROJECT_DIR "/Users/michal/Dev/bpm"
#define DATA_DIR "/Users/michal/Dev/bpm"+"/data"
#define INTENSITIES_DIR DATA_DIR+"/intensities"
#define VIDEO_SAMPLES_DIR DATA_DIR+"/videoSamples"

// How many frames should be in buffer (amplification process together)
#define BUFFER_FRAMES 500
#define FRAMES_FOR_VISUALIZATION 250

// Experimental settings of fps
#define FPS 30

// This is fixed time in milliseconds of on cycle of main loop
#define LOOP_WAIT_TIME_MUS (CLOCKS_PER_SEC / FPS)

// Usually defects of camera when starting (exposure setting etc)
#define CAMERA_INIT 10

#define SECONDS_IN_MINUTE 60

// Resize input video for processing
#define RESIZED_FRAME_WIDTH_FOR_PROCESSING 300

// Resize input video
#define RESIZED_FRAME_WIDTH 480

// Erased border width
#define ERASED_BORDER_WIDTH 10

// Variation between faces needed for upda
#define DETECTOR_UPDATE_VARIATION 0.1

// MASKING MODES
#define FOURIER_MASK_MODE 10
#define FAKE_BEATING_MODE 20

// SOURCE MODES
#define VIDEO_REAL_SOURCE_MODE 110
#define VIDEO_STATIC_SOURCE_MODE 115
#define CAMERA_SOURCE_MODE 120

// CONVERSION
#define CV2_BGR2YIQ 1
#define CV2_YIQ2BGR 2

// INITIAL CUT-OFF FREQ
#define CUTOFF_FL 55
#define CUTOFF_FH 150

// COLORS
#define BLUE_CHANNEL 0
#define GREEN_CHANNEL 1
#define RED_CHANNEL 2

// FREQ MASK
#define FREQ_MASK_WIDTH 20

// FRAME DIMENSIONS FOR AMPLIFICATION
#define MIN_WIDTH_IN_PYRAMID 10
#define IDEAL_WIDTH_FOR_AMPLIFICATION 140

// BPM COMPUTE TYPE
#define AVG_COMPUTE 210
#define MEDIAN_COMPUTE 220

// Generate app data (images)
#define DATA_LOGGING false

#endif // CONFIG_H
