#include "main.h"

#define DEBUG 0
#define DEBUG2 0
#define OPACITY 180 //Default 255
#define DEFAULT_LINE 5
#define TEXT_HEIGHT 15
#define BLOCK_RATE 4
#define COLOR_MULT 24
#define SPEED 2

uint8_t sides = 10;
uint8_t size = 7;
bool running = 0;
int line = DEFAULT_LINE;
int ballcount = 1;
int level = 1;
int rgbColor[3] = {255,255,0}; //In RGB, Yellow


/*int blocks[9][7] = {
	{0,0,0,0,1,0,0},
	{11,12,13,14,15,16,17},
	{21,22,23,24,25,26,27},
	{31,32,33,34,35,36,37},
	{41,42,43,-1,45,46,47},
	{51,52,53,54,55,56,57},
	{61,62,63,64,65,66,67},
	{71,72,73,74,75,76,77},
	{0,0,0,0,0,0,0},
};*/

int blocks[9][7] = {
	{0,0,0,0,1,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
};

Ball* init() {
	romfsInit();
	pp2d_init();
	pp2d_set_3D(true); //add option to toggle, for those with broken switches
	pp2d_set_screen_color(GFX_TOP, ABGR8(255,33,31,32));
	pp2d_set_screen_color(GFX_BOTTOM, ABGR8(255,33,31,32));
	if (DEBUG2) {
		gfxInitDefault();
		consoleInit(GFX_TOP, NULL);
	}
	Ball* balls = malloc(sizeof(Ball));
	if (balls == NULL && DEBUG2) {
		printf("Malloc Failed");
	}
	balls->x = 160;
	balls->y = 190;
	balls->type = 0;
	balls->xvel= 0;
	balls->yvel= 0;
	balls->next = NULL;

	return balls;
}

void stop(Ball* head) {
	romfsExit();
	pp2d_exit();
	if (DEBUG2) gfxExit();
	free(head);
}

/*Radius: 8
 *Color (green): RGBA8(132,194,63,255)
 *Sides: 20
 *
 *Smaller size:
 *Radius: 7
 *Sides: 10
*/

void debug_text(const char* text, ...) {
	if(DEBUG) {
	pp2d_frame_draw_on(GFX_BOTTOM, GFX_LEFT);
	line++;
	pp2d_draw_textf(2, 2 + TEXT_HEIGHT*line, 0.5f, 0.5f, RGBA8(0xFE, 0xFE, 0xFE, 0xFF), text);
	}
}

void debug(Ball* head) {
	if (DEBUG) {
	if(hidKeysHeld() & KEY_UP) sides++;
	if(hidKeysHeld() & KEY_DOWN && sides > 2) sides--;
	if(hidKeysHeld() & KEY_RIGHT) size++;
	if(hidKeysHeld() & KEY_LEFT) size--;
	if(hidKeysDown() & KEY_A) new_ball(head);
	pp2d_draw_textf(2, 2, 0.5f, 0.5f, RGBA8(0xFE, 0xFE, 0xFE, 0xFF), "SIDES: %i", sides);
	pp2d_draw_textf(2, 2 + TEXT_HEIGHT, 0.5f, 0.5f, RGBA8(0xFE, 0xFE, 0xFE, 0xFF), "SIZE: %i", size);
	pp2d_draw_textf(2, 2 + TEXT_HEIGHT*2, 0.5f, 0.5f, RGBA8(0xFE, 0xFE, 0xFE, 0xFF), "CPU: %2.2f%%", C3D_GetProcessingTime()*6.0f);
	pp2d_draw_textf(2, 2 + TEXT_HEIGHT*3, 0.5f, 0.5f, RGBA8(0xFE, 0xFE, 0xFE, 0xFF), "GPU: %2.2f%%", C3D_GetDrawingTime()*6.0f);
	pp2d_draw_textf(2, 2 + TEXT_HEIGHT*4, 0.5f, 0.5f, RGBA8(0xFE, 0xFE, 0xFE, 0xFF), "CmdBuf: %2.2f%%", C3D_GetCmdBufUsage()*100.0f);
	pp2d_draw_textf(2, 2 + TEXT_HEIGHT*5, 0.5f, 0.5f, RGBA8(0xFE, 0xFE, 0xFE, 0xFF), "Vertexes: %u", pp2d_get_verticies()); //This isn't technically correct, but it should be fine for testing
	pp2d_draw_textf(2, 2 + TEXT_HEIGHT*6, 0.5f, 0.5f, RGBA8(0xFE, 0xFE, 0xFE, 0xFF), "No. Ball: %i", ballcount);
	}
}

void draw_ui(gfxScreen_t screen) {
	if (screen == GFX_TOP) {
		pp2d_draw_rectangle(0, 0, 400, 20, RGBA8(38, 38, 38, 255)); //Top
		pp2d_draw_rectangle(0, 20, 40, 220, RGBA8(38, 38, 38, 255)); //Left border
		pp2d_draw_rectangle(360, 20, 40, 220, RGBA8(38, 38, 38, 255)); //Right border
		pp2d_draw_text(0.5, 2, 0.6f, 0.6f, RGBA8(255, 255, 255, 255), "BEST");
		pp2d_draw_textf(0.5, 12, 0.6f, 0.6f, RGBA8(255, 255, 255, 255), "%i", level);
	} else {
		pp2d_draw_rectangle(0, 190, 320, 50, RGBA8(38, 38, 38, 255)); //Bottom border
		if (!running) {
			//pp2d_draw_text() for num of balls
		}
	}
	//pp2d_draw_textf(2, 20, 0.7f, 0.7f, RGBA8(0xFE, 0xFE, 0xFE, 0xFF), "%i", score);
}

void draw_blocks(gfxScreen_t screen, gfx3dSide_t side) {
	int offsetx = 5; //Left gap
	int offsety = 0;
	int range[2] = {0};
	if (screen == GFX_TOP) {
		if (side == GFX_RIGHT) offsetx += osGet3DSliderState(); //Converted to int, because pp2d takes an int, not a float, not anything I can do about it
		offsetx += 40;
		offsety += 20;
		range[0] = 0;
		range[1] = 5;
	} else if (screen == GFX_BOTTOM) {
		offsety = -220;
		range[0] = 5; //I did this without internet, look to see if there's a way to assign the entire int array.
		range[1] = 9;
	}
	for (int i = 0; i < 7; i++) {
		for (int j = range[0]; j < range[1]; j++) {
			if (blocks[j][i] >= 1) {
				char buf[7]; //I think this should cover the integer limit
				sprintf(buf, "%i", blocks[j][i]);
				float width = pp2d_get_text_width(buf, 0.6f, 0.6f);
				float height = pp2d_get_text_height(buf, 0.6f, 0.6f);
				color_blocks(blocks[j][i]);
				pp2d_draw_square(i * 45 + offsetx, j * 45 + offsety, 40, RGBA8(rgbColor[0], rgbColor[1], rgbColor[2], OPACITY));
				pp2d_draw_text(i * 45 + offsetx + 20 - width / 2, j * 45 + offsety + 20 - height / 2, 0.6f, 0.6f, RGBA8(255, 255, 255, 128), buf);

				if (DEBUG || DEBUG2) {
					pp2d_draw_circle(i * 45 + offsetx, j * 45 + offsety, 2, RGBA8(255,194,63,255), 5); //Left Top corner of boxes
					pp2d_draw_square(i * 45 + offsetx + 20, j * 45 + offsety + 20, 1, RGBA8(255,194,63,255)); //Center pixel of boxes
					pp2d_draw_circle(i * 45 + offsetx + 40, j * 45 + offsety + 40, 2, RGBA8(255,194,63,255), 5); //Bottom Right corner of boxes
				}

				if (DEBUG2) {
					printf("Range: %i, %i\n", range[0], range[1]); //This is going to always be bottom screen (5, 9)
					printf("Block: %i, %i\n", j, i);
					printf("Value: %i\n", blocks[j][i]);
					printf("Rectangle at %ix, %iy to %ix, %iy\n", i * 45 + offsetx, j * 45 + offsety, i * 45 + offsetx + 40, j * 45 + offsety + 40);
					printf("Drawing text at %f, %f\n", i * 45 + offsetx + 20 - width / 2, j * 45 + offsety + 20 - height / 2);
					printf("Color (RBG): %i, %i, %i\n", rgbColor[0], rgbColor[1], rgbColor[2]);
					printf("Press key to continue...\n");
					do {
						hidScanInput();
					} while (!hidKeysDown());
				}
			}

			else if (blocks[j][i] < 0) {
				pp2d_draw_circle(i * 45 + offsetx + 20, j * 45 + offsety + 20, 16, RGBA8(255, 255, 255, 255), 20);
				pp2d_draw_circle(i * 45 + offsetx + 20, j * 45 + offsety + 20, 12, RGBA8(32, 31, 33, 255), 20);
				pp2d_draw_circle(i * 45 + offsetx + 20, j * 45 + offsety + 20, 7, RGBA8(255, 255, 255, 255), 10);
			}
		}
	}
}

void color_blocks(int number) {
	//Line below for gdb debugging
	//printf "Red: %i, Green: %i, Blue: %i, Number:%i, i:%i, Number - i: %i, Color_State: %i\n", rgbColor[0], rgbColor[1], rgbColor[2], number, i, number - i, color_state
	int i = 0;
	int color_state = 0;
	rgbColor[0] = 255;
	rgbColor[1] = 255;
	rgbColor[2] = 0;

	number = number * COLOR_MULT;

	do {//There are only really two methods used here, but converting them to different functions would need me to pass arguments, and I'd rather not deal with that in C
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

void shift_blocks() {
	for (int i = 7; i >= 0; i--) {
		for (int j = 8; j >= 0; j--) {
			blocks[j+1][i] = blocks[j][i];
			//printf("Shifting %i, %i to %i, %i\n", j+1, i, j, i); //shift all blocks down
		}
	}
	new_blocks();
}

void new_blocks() {
	for (int k = 0; k < 7; k++) { //clear top row
		blocks[0][k] = 0;
	}

	for (int i = 0; i < 7; i++) {
		blocks[0][i] = 0;
		int random = (rand() % BLOCK_RATE);
		if (random == 1) {
			int random2 = rand() % 3;
			if (random2 == 1) blocks[0][i] = 2 * level;
			int random3 = rand() % 4;
			if (random3 == 1) blocks[0][i] = -1;
			else blocks[0][i] = level;
		}
	}
	level++;
}

void collision(Ball* balls) {
	//https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection

	Ball* current = balls;

	while (current != NULL) {
		/*for (int i = 0; i < 7; i++) {
			for (int j = range[0]; j < range[1]; j++) {
				if (blocks[j][i] >= 1) {
					int x = i * 45 + offsetx;
					int y = j * 45 + offsety;
					if (current->x < (x + 40) && (current->x + 14) > x && current->y < (y + 40) && (14 + current->y) > y) {
						current->angle = -current->angle;
					}
				}
				if (blocks[j][i] == -1) {
					new_ball(balls);
				}
			}
		}*/
		if (current->x > 320) current->xvel = -current->xvel;
		if (current->x < 0) current->xvel = -current->xvel;
		if (current->y > 190) running = 0;
		if (current->y < -220) current->yvel = -current->yvel;
	current = current->next;
	}
}

void draw_balls(Ball* head, gfxScreen_t screen) {
	Ball* current = head;

	while (current != NULL) {
		if (screen == GFX_TOP) {
			pp2d_draw_circle(current->x + 40, current->y + 240, size, RGBA8(255, 255, 255 , 255), sides);
		} else if (screen == GFX_BOTTOM) {
			pp2d_draw_circle(current->x, current->y, size, RGBA8(255, 255, 255 , 255), sides);
		}
		current = current->next;
	}
}

void new_ball(Ball* head) {//http://www.learn-c.org/en/Linked_lists
	ballcount++;
	Ball* current = head;
	while (current->next != NULL) {
		current = current->next;
	}

	current->next = malloc(sizeof(Ball));
	current->next->x = -7;
	current->next->y = 247;
	current->next->type = 1;
	current->next->xvel = 0;
	current->next->yvel = 0;
	current->next->next = NULL;
}

void move_balls(Ball* head) {
	Ball* current = head;
	int i = 0;
	while (current != NULL) {
		i++;
		if (current->y > 190) {
			current->x = head->x;
			current->y = head->y;
			current->xvel = 0;
			current->yvel = 0;
			current->type = 0;
		}
		else if (current->type == 1) {
			current->x += SPEED * current->xvel;
			current->y += SPEED * current->yvel;
			//current->x += SPEED * cos(current->angle);
			//current->y += SPEED * sin(current->angle);
		}

		if (current->next == NULL) {
			if (current->y >= 200) {
				running = 0;
				current->y -= 10;
			}
		}

		current = current->next;
	}
}

void get_aim(Ball* head, int x, int y) {
	if (hidKeysHeld() & KEY_TOUCH) {
		int atanX = (x - 180);
		int atanY = (y - 190);
		double direction = atan2(atanY, atanX);

		Ball* current = head;
		while (current != NULL) {

			//current->x += cos(current->angle);
			//current->y += sin(current->angle);
			//current->angle = direction;
			current->xvel = cos(direction);
			current->yvel = sin(direction);
			current->type = 1;
			current = current->next;
		}
		running = 1;
	}
}

void reset_pos(Ball* balls) {
	Ball* current = balls;
	while (current != NULL) {
		current->x = 160;
		current->y = 190;
		current->xvel = 0;
		current->yvel = 0;
		current = current->next;
	}
}


int main() {
	Ball* balls = init();

	while(aptMainLoop() && !(hidKeysDown() & KEY_START)) {
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		if (running == 0) {
			get_aim(balls, touch.px, touch.py); //looks for touch input to launch the ball
			//reset_pos(balls);
		} else move_balls(balls);
		if (!DEBUG2) {
			pp2d_frame_begin(GFX_TOP, GFX_LEFT);//Default non-3d screen
				draw_ui(GFX_TOP); //draws top screen ui
				draw_blocks(GFX_TOP, GFX_LEFT); //blocks
				draw_balls(balls, GFX_TOP); //balls
				//draw top left screen
				pp2d_frame_draw_on(GFX_TOP, GFX_RIGHT);//change frame
				//draw top right screen*/
				pp2d_frame_draw_on(GFX_BOTTOM, GFX_LEFT); //change frame
		} else pp2d_frame_begin(GFX_BOTTOM, GFX_LEFT); //change frame
			draw_ui(GFX_BOTTOM); //ui bottom screen
			draw_blocks(GFX_BOTTOM, GFX_LEFT); //Draws blocks on bottom screen
			draw_balls(balls, GFX_BOTTOM); //Draws ball on bottom screen
			collision(balls); //Checks for ball collision
			//pp2d_draw_circle(180, 183, size, RGBA8(132,194,63,255), sides);//Test Ball
			//draw bottom screen
			debug(balls);
			//shift_blocks();
			pp2d_frame_end();
		line = DEFAULT_LINE;
		if (hidKeysDown() & KEY_Y) {
			for (int i = 0; i < 4; i++) {
				shift_blocks();
			}
		}
		if (DEBUG2) while(!hidKeysDown()) hidScanInput();
	}
	stop(balls);
	return 0;
}
