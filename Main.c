#include "global.h"
#include "global_vars.c"
#include "scripts.c"
#include "WndProc.c"

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow){
	MSG msg ;
	WNDCLASS wndclass ;
	null_pen = CreatePen(0, 0, 0);	
	
	user_function.pen = CreatePen(PS_SOLID, 1, RGB(150, 255, 50));

	static_circle.r = 20;
	static_circle.rgb = RGB(15, 15, 25);
	static_circle.brush = CreateSolidBrush(static_circle.rgb);

	user_function.id = 1;
	init_function(&user_function);

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon(NULL, IDI_INFORMATION) ;
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = APPNAME ;

	if (!RegisterClass (&wndclass))	{
		error("from:RegisterClass");
		return 0 ;
	}

	ShowCursor(0);

	hMainWindow = CreateWindow (APPNAME, APPNAME, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL) ;

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
		MessageBox (NULL, message, APPNAME, MB_ICONERROR);
		exit(1);
	}
	else{
		MessageBox (NULL, error, APPNAME, MB_ICONERROR);
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
		MessageBox (NULL, message, APPNAME, MB_ICONERROR);
	}
	else{
		MessageBox (NULL, error, APPNAME, MB_ICONERROR);
	}
}

void notify(const char * message){
	MessageBox (NULL, message, APPNAME, 0);
}

void notifyd(const double d){
	static char message[17];
	sprintf(message, "%.0lf", d);
	MessageBox (NULL, message, APPNAME, 0);
}

void notifyc(char c){
	static char message[17];
	sprintf(message, "%c", c);
	MessageBox (NULL, message, APPNAME, 0);
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

void post_line_out(char * out){

}
