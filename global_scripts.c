typedef struct{
	int val;
	char name[MAX_VAR_NAME];
} int_var; // Integer variable

typedef struct {
	COLORREF cref;
	char name[MAX_VAR_NAME];
} rgb_var; // RGB (red-green-blue) variable 

typedef struct {
	int x;
	int y;
	COLORREF rgb;
} pixel;

typedef struct {
	int_var * var;
	int amount;
} increment_op;

typedef struct{
	int id;
	
	// Variable data:
	int_var ints[MAX_INTS];
	rgb_var rgbs[MAX_RGBS];

	int int_count;
	int rgb_count;

	// Drawing data:
	circle circles[MAX_CIRCLES];
	int circles_count;
	line lines[MAX_LINES];
	int lines_count;
	triangle triangles[MAX_TRIANGLES];
	int triangles_count;
	function functions[MAX_FUNCTIONS];
	int function_count;
	pixel pixels[MAX_PIXELS];
	int pixel_count;
	
	// Operations
	increment_op increments[MAX_INCREMENTS];
	int increment_count;
	
} script;

script script_list[SCRIPTS_LIMIT];
int vascripts = 0;

COLORREF	default_rgb_val = RGB(255, 255, 0);
int			default_int_val = 0;
