extern void handle_timer(HWND, WPARAM);

void setup_buffer(HWND hwnd){
	RECT r;
	GetClientRect(hwnd, &r);

	display_x = r.right;
	display_y = r.bottom;
	HDC hdc;

	if((hdc = GetDC(hwnd)) == NULL) {
		error("from:GetDC");
	}
   
	if((hBuffer = CreateCompatibleDC(hdc)) == NULL){
		error("from:CreateCompatibleDC");
	}
	buffer_bitmap = CreateCompatibleBitmap(hdc, r.right, r.bottom);
	if(buffer_bitmap == NULL){
		error("from:CreateCompatibleBitmap");
	}
	HBITMAP old = (HBITMAP) SelectObject(hBuffer, buffer_bitmap);
	if(old == NULL){
		error("from:SelectObject");
	}
	SelectObject(hBuffer, CreateFont(10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "fixed"));
	ReleaseDC(hwnd, hdc);
	GetTextMetrics(hBuffer, &tm);	
}

void proc_drawing(HWND hwnd){
	HDC hdc = BeginPaint(hwnd, &ps);
	static char out[30];
	SetTextColor(hBuffer, RGB(200, 200, 200));
	sprintf(out, "Frames per sec: %d", frames_per_second);
	TextOut(hBuffer, 10, 5, out, strlen(out));

	BitBlt(hdc, 0, 0, display_x, display_y, hBuffer, 0, 0, SRCCOPY);

	EndPaint(hwnd, &ps) ;
}

void setup_output(void){
	output_rect.left = 7;
	output_rect.top = (WINDOW_HEIGHT - (tm.tmHeight * BUFFLINES) - tm.tmHeight*5) - 10;
	output_rect.right = WINDOW_WIDTH-20;
	output_rect.bottom = WINDOW_WIDTH - ( tm.tmHeight * 3 );
}

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
	curr_script_id = (int) w;
	init_function(&user_function);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	if(message == WM_CREATE){

		SetTimer(hwnd, ANIMATE_TIMER, UPDATE_RATE, NULL);
		SetTimer(hwnd, ONE_SECOND_TIMER, 1000, NULL);
		setup_buffer(hwnd);
		setup_scripts();
		setup_output();
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
		char x[100];
		static z  = 0;
		z++;
		sprintf(x, "%d", z*100);
		//post_line_out(x);
		key_down(wParam);
	}
	else if(message == WM_KEYUP){
		keys[wParam & 0xFF] = false;
	}
	else if(message == WM_MOUSEMOVE){
		static POINT point;
		if(wParam != MK_LBUTTON){
			point.x = mouse_x = LOWORD(lParam);
			point.y = mouse_y = HIWORD(lParam);
			if(PtInRect(&output_rect, point)){
				if(!cursor_shown){
					ShowCursor(1);
					cursor_shown = 1;
				}
			}
			else{
				if(cursor_shown){
					ShowCursor(0);
				}
			}
		}
	}
	else if(message == WM_DESTROY){
		destroy_resources();
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
