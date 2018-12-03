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
