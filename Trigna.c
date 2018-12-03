#include "global_declares.c"
#include "global.c"
#include "interpeter_calls.c"
#include "interpeter.c"
#include "interpeter_handlers.c"
#include "scripts.c"
#include "computations.c"
#include "compute.c"
#include "function_draws.c"
#include "proc_drawing.c"
#include "animation.c"

#define VERSION "0.1-Beta"

void setup_win32(void);
void setup_general(void);
void setup_resources(void);

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, 
	int iCmdShow){
	WinInstance = hInstance; // WinMain instance assigned to global Instance
	
	setup_resources();
	setup_general();
	setup_win32();

	MSG msg ;
	ShowWindow (hMainWindow, iCmdShow) ;
	UpdateWindow (hMainWindow) ;

	while (GetMessage (&msg, NULL, 0, 0)){
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

void error(const char * error){
	static char message[530];
	if(!strncmp(error, "from:", 5)){
		error += 5;
		char formulated[513];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0,
			formulated, 512, NULL);
		sprintf(message, "%s: %s", error, formulated);
		MessageBox (NULL, message, appname, MB_ICONERROR);
		exit(1);
	}
	else{
		MessageBox (NULL, error, appname, MB_ICONERROR);
		exit(1);
	}
}

void notifye(const char * error){
	static char message[530];
	if(!strncmp(error, "from:", 5)){
		error += 5;
		char formulated[513];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0,
			formulated, 512, NULL);
		sprintf(message, "%s: %s", error, formulated);
		MessageBox (NULL, message, appname, MB_ICONERROR);
	}
	else{
		MessageBox (NULL, error, appname, MB_ICONERROR);
	}
}

void notify(const char * message){
	MessageBox (NULL, message, appname, 0);
}

void notifyd(const double d){
	static char message[17];
	sprintf(message, "%.0lf", d);
	MessageBox (NULL, message, appname, 0);
}

void notifyc(char c){
	static char message[1];
	sprintf(message, "%c", c);
	MessageBox (NULL, message, appname, 0);
}

void strlower(const char * str, char * lowered){
	strcpy(lowered, str);
	while(*lowered) {
		*lowered = tolower(*lowered);
		lowered++;
	}
}

char * laststrchr(char * str, char c){
	int length = strlen(str);
	while(length--){
		if(str[length] == c){
			return &str[length];
		}
	}
	return NULL;
}

void setup_win32(void){
	WNDCLASS wndclass ;
	wndclass.style = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc	= WndProc ;
	wndclass.cbClsExtra		= 0 ;
	wndclass.cbWndExtra		= 0 ;
	wndclass.hInstance		= WinInstance ;
	wndclass.hIcon			= LoadIcon( NULL, IDI_APPLICATION) ;
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH) GetStockObject (BLACK_BRUSH) ;
	wndclass.lpszMenuName	= NULL ;
	wndclass.lpszClassName	= appname ;
	if (!RegisterClass (&wndclass))	{
		error("from:RegisterClass");
	}
	ShowCursor(0);
	hMainWindow = CreateWindow (appname, appname,
		WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
		10, 10, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, WinInstance, NULL) ;
	if(hMainWindow == NULL){
		error("from:CreateWindow");
	}
}

void setup_general(void){
	char welcome[60];
	struct tm t ;
	time_t initTime = time(NULL);
	char * time_chars = ctime(&initTime);
	time_chars[strlen(time_chars)-1] = 0; // for the \n character (which shows up weird in TextOut)
	sprintf(appname, "Trigna v%s", VERSION);
	sprintf(welcome, "Welcome to %s - %s", appname, time_chars);
	post_line_out(welcome);

	static_circle.r = 20;
	user_function.id = 1;
}

void setup_resources(void){
	null_pen = CreatePen(0, 0, 0);	
	output_brush = CreateSolidBrush(RGB(5, 5, 10));
	output_pen = CreatePen(PS_SOLID, 1, RGB(30, 30, 60));
	user_function.pen = CreatePen(PS_SOLID, 1, RGB(150, 255, 50));
	static_circle.rgb = RGB(15, 15, 25);
	static_circle.brush = CreateSolidBrush(static_circle.rgb);
	init_function(&user_function);
}

void destroy_resources(void){
	DeleteObject(null_pen);
	DeleteObject(output_brush);
	DeleteObject(output_pen);
	DeleteObject(user_function.pen);
	DeleteObject(static_circle.brush);
}

void post_line_out(char * out){
	if(strlen(out) > LINELEN-1)
		out[LINELEN-1] = 0;
	if(out_currline != BUFFLINES){ // We're not at the line limit
		strcpy(lines_out[out_currline], out);
		out_currline++;
	}
	else{
		memcpy(lines_out[0], lines_out[1], LINELEN * (BUFFLINES-1)); // Move the lines up
		strcpy(lines_out[out_currline-1], out); 
	}
}
