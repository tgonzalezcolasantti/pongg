#ifndef __MAIN_H
#define __MAIN_H

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define GAME_NAME "PONGG"
#define TICKS_FOR_NEXT_FRAME (1000 / 60)
#define FONT_SIZE 120

typedef struct Entity {
    int x;
    int y;
    SDL_Texture *texture;
} Entity;

#endif