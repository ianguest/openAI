

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED



#include "./include/yolodet.h"
#include "./include/mqtt.h"


//#define SHOW_GRAPHICS

#define USE_YOLO

//#define USE_TELEGRAM
#define USE_MQTTPP

#ifdef USE_MQTTPP
#include "./include/mqtt.h"
#endif



//#define WRITE_CONSEC_MP4_FILES
//#define WRITE_CONSEC_TRACK_MP4_FILES

#define FILE_SEGMENT_TIME   (60)

#define KEEP_HISTORY_IMAGES
#define HISTORY_DEPTH       (20)

#define DEFAULT_FPS         6

#endif // MAIN_H_INCLUDED
