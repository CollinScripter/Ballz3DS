#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <math.h>
#include "pp2d.h"

typedef struct Ball {
    double x;
    double y;
    int type;
    double xvel;
    double yvel;
    struct Ball* next;
} Ball;

Ball* init(void);
void stop(Ball* head);
void debug_text(const char* text, ...);
void debug(Ball* balls);
void draw_ui(gfxScreen_t screen);
void draw_blocks(gfxScreen_t screen, gfx3dSide_t side);
void color_blocks(int number);
void shift_blocks(void);
void new_blocks(void);
//int collision(int blockx, int blocky, int ball)
void draw_balls(Ball* head, gfxScreen_t screen);
void new_ball(Ball* head);
void move_balls(Ball* head);
void get_aim(Ball* head, int x, int y);
int main(void);
