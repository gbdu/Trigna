#define ANIMATE_TIMER 1
#define ONE_SECOND_TIMER 2
#define UPDATE_RATE 1

#define SCRIPTS_LIMIT 50 // Limit of scripts that can be read at once
#define SCRIPT_EXT ".tgna" // String of the script file extension, not case sensative.

HDC hDraw;
PAINTSTRUCT ps;
TEXTMETRIC tm;

HDC hBuffer;
HBITMAP buffer_bitmap;
HPEN null_pen;

HWND hMainWindow;
bool keys[256]; // True if a key is pressed, false if it's not.

int display_x;
int display_y;
int mouse_x = 100;
int mouse_y = 100;

int updates_per_frame = 1;

int frames = 0; // frames until this second used for counting
int frames_per_second = 0; // used for output

int chars_per_line = 100;

#define LINELEN 200 // Characters in each line (the width)
#define BUFFLINES 10 // Lines in buffer

char lines_out[BUFFLINES][LINELEN]; 

struct {
	int x;
	int y;
	int r;
	COLORREF rgb;
	HBRUSH brush;
	char result[256];
	int result_len;
	int resultx;
	int resulty;
} typedef circle;

circle static_circle = { 0 } ;
circle static_point = { 0 };

struct {
	// First point
	int x1;
	int y1;
	// Second point
	int x2;
	int y2;
	// Color for pen
	COLORREF rgb;
	// Pen filled by init_line:
	HPEN pen;
	char result[256];
	int resultx;
	int resulty;
} typedef line;

line static_line = { 0 };

struct {
	COLORREF rgb;
	HBRUSH brush;

	line a;
	line b;
	line c;
	
	// Angles in degrees:
	int ADeg;
	int BDeg;
	int DDeg;

} typedef triangle;

triangle static_triangle = {0};

typedef struct { // trig function and test results
	int id; // function id
	int x; // result x
	int y; // result y

	RECT boundry;
	bool is_bounded; // Is the boundry in effect?
	HBRUSH brush;
	COLORREF rgb;
	HPEN pen;

	const char * desc;
	int desc_len; // strlen of desc to be populated in compute_function
	
	char result[256]; // result output
	int result_len; // result output strlen to be populated in compute_function
	int resultx; // function result length in pixels on the x axis
	int resulty; // function result length in pixels on the y axis
	
	char result2[256];
	int result2_len;
	int result2x;
	int result2y;

	HBRUSH flicker_brush;
	COLORREF flicker_rgb;

	int flicker_rate; // how many frames until changing the flicker color
	int flicker_count; // how many frames have gone since last flicker

	bool is_flicker; // true if the flicker brush is selected

} function;

function user_function;

const int menu_lines = 4; // not including 0

char * menu_out[ ] = {
	"Functions: (Press corresponding number key)",
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

