#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define BLOCK_RATE 2+1
#define COLOR_MULT 1 //Above 0
#define SPEED 2

int rgbColor[3] = {255,255,0}; //In RGB, Yellow
int level = 1;
int ballcount = 1;

int blocks[9][7] = {
        {0,0,0,0,1,0,0},
        {11,12,13,14,15,16,17},
        {21,22,23,24,25,26,27},
        {31,32,33,34,35,36,37},
        {41,42,43,-1,45,46,47},
        {51,52,53,54,55,56,57},
        {61,62,63,64,65,66,67},
        {71,72,73,74,75,76,77},
        {0,0,0,0,0,0,0},
};

typedef struct Ball {
    int x;
    int y;
    int type;
    int angle;
    struct Ball* next;
} Ball;

void draw_blocks() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 7; j++) {
            printf("%i, ", blocks[i][j]);
        }
        printf("\n");
    }
}

void shift_blocks() {
    for (int i = 7; i >= 0; i--) {
        for (int j = 8; j >= 0; j--) {
            blocks[j+1][i] = blocks[j][i];
            //printf("Shifting %i, %i to %i, %i\n", j+1, i, j, i); //shift all blocks down
        }
    }
    for (int k = 0; k < 7; k++) { //clear top row (replace with new blocks function)
        blocks[0][k] = 0;
    }
}

void new_blocks() {
    for (int i = 0; i < 7; i++) {
        blocks[0][i] = 0;
        int random = (rand() - 1) % BLOCK_RATE;
        if (random == 1) {
            int random2 = rand() % 2;
            if (random2 == 1) blocks[0][i] = 2 * level;
            else blocks[0][i] = level;
        }
    }
}

void color_blocks(int number) {
    //Line below for gdb debugging
    //printf "Red: %i, Green: %i, Blue: %i, Number:%i, i:%i, Number - i: %i, Color_State: %i\n", rgbColor[0], rgbColor[1], rgbColor[2], number, i, number - i, color_state
    int i = 0;
    int color_state = 0;

    number = number * COLOR_MULT;

    do {
        if (color_state == 0) { //Yellow, to Green
            if (number - i < rgbColor[0]) {
                rgbColor[0] = rgbColor[0] - (number - i);
                i = number;
            }
            else {
                color_state += 1;
                i += rgbColor[0];
                rgbColor[0] = 0;
            }
        }

        if (color_state == 1) { //Green, to Teal
            if (number - i < 255 - rgbColor[2]) {
                rgbColor[2] = rgbColor[2] + (number - i);
                i = number;
            }
            else {
                color_state += 1;
                i += 255 - rgbColor[2];
                rgbColor[2] = 255;
            }
        }

        if (color_state == 2) { //Teal, to Blue
            if (number - i < rgbColor[1]) {
                rgbColor[1] = rgbColor[1] - (number - i);
                i = number;
            }
            else {
                color_state += 1;
                i += rgbColor[1];
                rgbColor[1] = 0;
            }
        }

        if (color_state == 3) { //Blue, to Pink
            if (number - i < 255 - rgbColor[0]) {
                rgbColor[0] = rgbColor[0] + (number - i);
                i = number;
            }
            else {
                color_state += 1;
                i += 255 - rgbColor[0];
                rgbColor[0] = 255;
            }
        }

        if (color_state == 4) { //Pink to Red
            if (number - i < rgbColor[2]) {
                rgbColor[2] = rgbColor[2] - (number - i);
                i = number;
            }
            else {
                color_state += 1;
                i += rgbColor[2];
                rgbColor[2] = 0;
            }
        }

        if (color_state == 5) { //Red to Yellow
            if (number - i < 255 - rgbColor[1]) {
                rgbColor[1] = rgbColor[1] + (number - i);
                i = number;
            }
            else {
                color_state = 0;
                i += 255 - rgbColor[1];
                rgbColor[1] = 255;
            }
        }

    } while (i < number);
}

//int collison

void print_list(Ball * head) {
    Ball * current = head;

    while (current != NULL) {
        printf("%d\n", current->x);
        current = current->next;
    }
}

void push(Ball * head, int val) {
    Ball * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = malloc(sizeof(Ball));
    current->next->x = val;
    current->next->y = 0;
    current->next->type = 0;
    current->next->angle = 0;
    current->next->next = NULL;
}

void move_balls(Ball* head) {
	Ball* current = head;
	int i = 0;
	printf("loadloop");
	while (current != NULL) {
		printf("inloop");
		i++;
		/*if (current->y > 190) {
			current->x = head->x;
			current->y = head->y;
			current->angle = 0;
			current->type = 0;
		}

		else */if (current->type == 1) {
			printf("changing speed %i", i);
			current->x = current->x + (SPEED * cosh(current->angle));
			current->y = current->y - (SPEED * sinh(current->angle));
		}
		//if (current->next == NULL) {
		//	running = 0;
		//}
		current = current->next;
	}
}



int main() {
    int number = 64 * 24;

    Ball* balls = malloc(sizeof(Ball));
    if (balls == NULL) {
        printf("Malloc Failed");
    }

    balls->x = number;
    balls->y = 0;
    balls->type = 0;
    balls->angle = 0;
    balls->next = NULL;





    //draw_blocks();
    //shift_blocks();
    //draw_blocks();
    //new_blocks();
    //draw_blocks();
    //shift_blocks();
    //draw_blocks();
    //int loop = 0;
    //while (loop != 1) {
        color_blocks(number);
        printf("%i, %i, %i\n", rgbColor[0], rgbColor[1], rgbColor[2]);
    //}
    return 0;
}
