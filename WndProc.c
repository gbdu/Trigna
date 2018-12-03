#include "proc_drawing.c"
#include "animation.c"

void key_down(WPARAM w){
	keys[w & 0xFF] = true;
	switch(w){
		case('0'):
			user_function.id = 0;
			break;
		case('1'):
			user_function.id = 1;
			user_function.pen = CreatePen(PS_SOLID, 1, RGB(150, 255, 50));
			break;
		case('2'):
			static_point.r = 2.5;
			static_point.rgb = RGB(250, 120, 10);
			static_point.brush = CreateSolidBrush(static_point.rgb);

			user_function.id = 2;
			user_function.pen = CreatePen(PS_SOLID, 1, RGB(150, 255, 50));
			break;
		case('3'):
			user_function.id = 3;
			user_function.pen = CreatePen(PS_SOLID, 1, RGB(150, 255, 50));
			init_triangle(&static_triangle, RGB(150, 255, 50));
			
			static_triangle.a.pen = CreatePen(PS_SOLID, 1, RGB(150, 255, 50));
			static_triangle.b.pen = CreatePen(PS_SOLID, 1, RGB(150, 255, 50));
			static_triangle.c.pen = CreatePen(PS_SOLID, 1, RGB(250, 255, 50));
			break;
		case(VK_ESCAPE):
			PostMessage(hMainWindow, WM_QUIT, 0, 0);
			break;
	}
	init_function(&user_function);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	if(message == WM_CREATE){
		GetTextMetrics(hDraw, &tm);
		SetTimer(hwnd, ANIMATE_TIMER, UPDATE_RATE, NULL);
		SetTimer(hwnd, ONE_SECOND_TIMER, 1000, NULL);
		setup_buffer(hwnd);
		setup_scripts();
	}
	else if(message == WM_PAINT){
		proc_drawing(hwnd);
	}
	else if(message == WM_SIZE){
		InvalidateRect(hwnd, NULL, 1);
	}
	else if(message == WM_TIMER){
		handle_timer(hwnd, wParam);
	}
	else if(message == WM_KEYDOWN){
		key_down(wParam);
	}
	else if(message == WM_KEYUP){
		keys[wParam & 0xFF] = false;
	}
	else if(message == WM_MOUSEMOVE){
		if(wParam != MK_LBUTTON){
			mouse_x = LOWORD(lParam);
			mouse_y = HIWORD(lParam);
		}
	}
	else if(message == WM_DESTROY){
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
