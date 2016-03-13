#ifndef CONFIG_H
#define CONFIG_H

// Define project dir folder
#define PROJECT_DIR "/Users/michal/Dev/bpm"

// How many frames should be in buffer (amplification process together)
#define BUFFER_FRAMES 50

// Experimental settings of fps
#define FRAME_RATE 10

// This is fixed time in milliseconds of on cycle of main loop
#define LOOP_WAIT_TIME_MUS (CLOCKS_PER_SEC / FRAME_RATE)

// Usually defects of camera when starting (exposure setting etc)
#define CAMERA_INIT 10

// Resize input video
#define RESIZED_FRAME_WIDTH 600

// Boundary to be removed
#define UNWANTED_BOUNDARY_WIDTH 20


#endif // CONFIG_H
