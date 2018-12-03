void pxstrlen(HDC hdc, char * string, int * x, int * y);
void draw_buffer(HDC);
void draw_menu(HDC);
void draw_out(HDC);

void draw_buffer(HDC hdc){
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, (HBRUSH) GetStockObject (BLACK_BRUSH));
	Rectangle(hdc, -2, -2, display_x+2, display_y+2); // clear buffer
	
	draw_function(hdc, &user_function);
	draw_pixels(hdc, &script_list[1]);

	draw_menu(hdc);
	draw_out(hdc);
}

void handle_timer(HWND hwnd, WPARAM id) {
	if(id == ANIMATE_TIMER){
		int c = 0;
		while(c != updates_per_frame){
			fill_buffer();
			c++;
		}
		frames++;
		draw_buffer(hBuffer); // draw to the off-screen dc
		InvalidateRect(hwnd, NULL, 0);
	}
	else if(id == ONE_SECOND_TIMER){
		frames_per_second = frames;
		frames = 0;
	}
}

void pxstrlen(HDC hdc, char * string, int * x, int * y){
	SIZE temp_size;
	GetTextExtentPoint(hdc, string, strlen(string), &temp_size);
	if(x != NULL)
		*x = temp_size.cx;
	if(y != NULL)
		*y = temp_size.cy;
}

void draw_menu(HDC hdc){
	int xlen;
	int ylen;
	int line = 0;
	int xMax = 0;
	while(line != menu_lines){
		pxstrlen(hdc, menu_out[line], &xlen, &ylen);
		xMax = max(xlen, xMax);
		line++;
	}
	line = 0;

	SetTextColor(hdc, RGB(0, 200, 250));
	while(line != menu_lines){
		if(user_function.id > 0 && user_function.id == line){
			SetTextColor(hdc, RGB(255, 255, 255)); // This is the item selected
		}
		TextOut(hdc, WINDOW_WIDTH - xMax - 20,
			(line+1) * ylen, menu_out[line], strlen(menu_out[line]));
		line++;

		SetTextColor(hdc, RGB(150, 150, 150));
	}
}

void draw_out(HDC hdc){
	int ylen;
	int ypos = 0;
	char s[100];
	SetTextColor(hdc, RGB(200, 200, 200));
	
	ylen = tm.tmHeight + tm.tmExternalLeading;

	ypos = output_rect.top;
	
	SelectObject(hdc, output_brush);
	SelectObject(hdc, output_pen);

	RoundRect(hdc, output_rect.left, ypos , output_rect.right, output_rect.bottom, 5, 5);

	TextOut(hdc, 10, ypos + 5, "Output:", 7);
	
	ypos += ylen + ylen/2;
	
	int c = 0;
	int lines = 0;
	int x_multiplier = 0;
	
	while(lines != out_currline){
		SetTextColor(hdc, RGB(100, 100, 150));
		if(lines_out[lines][0] == '\a'){ // error output color modifier
			SetTextColor(hdc, RGB(200, 10, 15));
			c++;
		}
		else if(lines_out[lines][0] == '\v'){ // success output color modifier
			SetTextColor(hdc, RGB(0, 255, 50));
			c++;
		}
		else if(lines_out[lines][0] == '\r'){ // script output color modifier
			SetTextColor(hdc, RGB(90, 100, 90));
			c++;
		}
		while(lines_out[lines][c] == '\t'){
			x_multiplier++;
			c++;
		}
		TextOut(hdc, (x_multiplier+1) * 20, ypos, &lines_out[lines][c],
			strlen(lines_out[lines]));
		
		ypos += ylen;
		lines++;
		x_multiplier = 0;
		c = 0;
	}

}
