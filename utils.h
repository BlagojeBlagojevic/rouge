#ifndef _UTILS_H
#define _UTILS_H
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include<SDL2/SDL_hints.h>
#include<SDL2/SDL_error.h>
#include<SDL2/SDL_log.h>
#include<SDL2/SDL_video.h>
#include<SDL2/SDL_render.h>
#include<SDL2/SDL_pixels.h>
#include<SDL2/SDL_rect.h>
#include<SDL2/SDL_surface.h>
#include<SDL2/SDL_clipboard.h>
#include<SDL2/SDL_events.h>
#include<SDL2/SDL_image.h>
//#include<SDL2/SDl_timer.h>
#include<SDL2/SDL_keyboard.h>
#include<SDL2/SDL_scancode.h>
#undef main

//COLORS
#define WHITE (SDL_Color){255, 255, 255, 0}
#define RED   (SDL_Color){255, 0, 0, 0}
#define GREEN (SDL_Color){0, 255, 0, 0}
#define BLUE  (SDL_Color){30, 0, 255, 0}
//TTF
#include<SDL2/SDL_ttf.h>
//static const char* fontLoc = "assets/fonts/f.ttf";
//#define fontLoc "assets/fonts/f.ttf"
#define fontLoc "assets/fonts/w.ttf"
//ARROW KEYS
#define LEFT_ARROW 1073741904
#define RIGHT_ARROW 1073741903
#define UP_ARROW 1073741906
#define DOWN_ARROW 1073741905
#define SPACE 32
#define KEY_I 105
#define KEY_P 112

//#include<pthread.h> TBD asychronus stuff

//TYPES
#include<stdint.h>
typedef  uint8_t  u8;
typedef  uint16_t u16;
typedef  uint32_t u32;
typedef  uint64_t u64;
typedef  int16_t  i16;
typedef  int32_t  i32;
typedef  int64_t  i64;
typedef  float    f32;
typedef  double   f64;

#define rand_f64() (f64)rand()/(f64)RAND_MAX

//DYNAMIC ARRAY
#define DA_SIZE 16
#define da_append(da, item)                                                            \
	do {                                                                                 \
		if ((da)->count >= (da)->capacity) {                                               \
			(da)->capacity = (da)->capacity == 0 ? DA_SIZE : (da)->capacity*2;               \
			(da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items));         \
			assert((da)->items != NULL && "Realloc fail !!!");                               \
			}                                                                                \
		\
		(da)->items[(da)->count++] = (item);                                               \
		} while (0)





//ERROR AND LOG HANDLING

#define WARNING(...)        fprintf(stdout, __VA_ARGS__)
#define ERROR_BREAK(...)    fprintf(stderr, __VA_ARGS__); exit(-1)
#define LOG(...)     			  fprintf(stdout, __VA_ARGS__)
#define PAUSE()           {char a; fputs(a, stdin);}
#define CLAMP(X, LOW, HIGH) {if(X < LOW) X = LOW; if(X > HIGH) X = HIGH;}
#define ASSERT(msg) {fprintf(stderr, "aseert in:\n\tFILE %s\n\tLINE %d\n\tmsg: %s" , __FILE__, __LINE__, msg); exit(-1);}
#define DROP(var) {(void)var;}

typedef struct {
	u64 	count;
	u64 	capacity;
	char** items;
	} Str;

#define MAP_X 80
#define MAP_Y 80

typedef struct Graphics_State {
	SDL_Window   *window;
	SDL_Renderer *renderer;
	TTF_Font     *font;
	i32           fontW;
	i32           fontH;
	SDL_Event     event;   //TBD like a queue of elements
	i32           width;
	i32           height;
	u8            isQuit;
	SDL_bool      isMovmentEvent;
	Str           messages;
	SDL_bool      isRenderItemsOnMap;
	SDL_bool      isPickingItem;
	} Graphics_State;

//static const char* title = "Ime kakvo";
#define title "Ime kakvo"

#define FONT_H_MESSAGES 20
#define MAX_NAME 30

extern Graphics_State mainGraphics;
#define WINDOW    mainGraphics.window
#define RENDERER  mainGraphics.renderer
#define FONT      mainGraphics.font
#define FONT_W    mainGraphics.fontW
#define FONT_H    mainGraphics.fontH
#define QUIT      mainGraphics.isQuit
#define EVENT     mainGraphics.event
#define WIDTH     mainGraphics.width
#define HEIGHT    mainGraphics.height
#define MOVMENT   mainGraphics.isMovmentEvent
#define MESSAGES  mainGraphics.messages
#define ITEMSREND mainGraphics.isRenderItemsOnMap
#define PICKITEM  mainGraphics.isPickingItem
typedef struct {
	i32 x;
	i32 y;
	} Position;


#define INF (f64)100000.0f
#define ZERO 0.0f
#define DISTANCE(x1, y1, x2, y2) sqrt((f64)((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)))
#define DISTANCE_RANGE_ATTACK_MAX 3.0f
#define DISTANCE_RANGE_ATTACK_MIN 1.1f

#define PERCANTAGE_RUN_CHANCE 0.3f
#define PERCANTAGE_CROW_RUN_CHANCE 0.5f
#define PERCENTAGE_MONSTER_GENERATED 0.005f
#define MAX_STOP_RUN_DISTANCE 10.0f
#define MAX_CURAGE_CHANCE 0.1f

#define MAX_HEALTH_VALUE 255
#define NUM_RENDER_MSG 5
#define RADIUS 10



#endif
