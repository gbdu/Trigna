#define ANIMATE_TIMER 1
#define ONE_SECOND_TIMER 2
#define UPDATE_RATE 1

#define SCRIPTS_LIMIT 50 // Limit of scripts that can be read at once
#define SCRIPT_EXT ".c" // String of the script file extension, lowercased

#define MAX_VAR_NAME 20

#define MAX_CIRCLES 100
#define MAX_LINES 100
#define MAX_TRIANGLES 100
#define MAX_FUNCTIONS 100
#define MAX_PIXELS 100

#define MAX_INTS 100
#define MAX_RGBS 100

#define MAX_INCREMENTS 100

char appname[50];

HDC hDraw;
PAINTSTRUCT ps;
TEXTMETRIC tm;

HDC hBuffer;
HBITMAP buffer_bitmap;
HPEN null_pen;
HBRUSH output_brush;
HPEN output_pen;
HWND hMainWindow; // Main window handle
HINSTANCE WinInstance; // Instance of main window
RECT output_rect;

bool keys[256]; // True if a key is pressed, false if it's not.

int display_x;
int display_y;
int mouse_x = 100;
int mouse_y = 100;

bool cursor_shown = 0;

int updates_per_frame = 1;

int frames = 0; // frames until this second used for counting
int frames_per_second = 0; // used for output

int curr_script_line = 1;
int curr_script_id = 1;

int chars_per_line = 100; // For the output window

#define LINELEN 200 // Characters in each line (the width)
#define BUFFLINES 10 // Lines in buffer

char lines_out[BUFFLINES][LINELEN]; 
int out_currline = 0;

#include "global_objects.c"
#include "global_scripts.c"

const int menu_lines = 4; // not including 0

char * menu_out[ ] = {
	"Scripts: (Press corresponding number key)",
	"1. General equation of the circle",
	"2. Distance between two points",
	"3. Sine of a right-angle triangle"
};

const char * func_descriptions[ ] = {
	"", // When no function is selected, the id is 0
	"1. r = sqrt((x - h)^2 + (y - k)^2), where (h,k) cordinates of a center of a static "
		"circle and (x,y) are your pointer's cordinates.",
	"2. AB = sqrt( (Bx - Ax)^2 + (By - Ay)^2 ), where A(x,y) is a static point,"
		" B(x,y) is your pointer, AB is the distance.",
	"3. "
};

