extern void pxstrlen(HDC hdc, char * string, int * x, int * y);
void fill_buffer();
void compute_func1(function *);
void compute_func2(function *);
void compute_func3(function *);

void compute_function(function * f){
	switch(f->id){
		case(0):
			*f->result = 0;
		case(1):
			compute_func1(f);
			break;
		case(2):
			compute_func2(f);
			break;
		case(3):
			compute_func3(f);
			break;
	}
	if(f->is_bounded == false){
		ClipCursor(NULL);
	}
	else{
		ClipCursor(&f->boundry);
	}
	f->result_len = strlen(f->result);
}

void init_function(function * f){
	f->desc =  func_descriptions[f->id];
	f->rgb = RGB(50, 50, 255);
	f->flicker_rgb = RGB(220, 120, 110);
	f->flicker_rate = 15;
	f->brush = CreateSolidBrush(f->rgb);
	f->flicker_brush = CreateSolidBrush(f->flicker_rgb);
	f->desc_len = strlen(f->desc);
}

void init_line(line * l, COLORREF rgb){
	l->rgb = rgb;
	l->pen = CreatePen(PS_SOLID, 1, rgb);
}

void init_triangle(triangle * t, COLORREF rgb){
	t->rgb = rgb;
	t->brush = CreateSolidBrush(rgb);
}

void fill_buffer(){
	compute_function(&user_function);
	compute_operations();
}

void compute_func1(function * f){
	f->is_bounded = false; // No boundry 
	f->x = mouse_x;
	f->y = mouse_y;
	static_circle.x = 300;
	static_circle.y = 300;
	static_circle.r = sqrt( pow( f->x - static_circle.x, 2 ) + pow( f->y -
		static_circle.y , 2 ) );
	sprintf(f->result, "x: %d y: %d", f->x, f->y);
}

void compute_func2(function * f){
	f->is_bounded = false; // No boundry
	f->x = mouse_x;
	f->y = mouse_y;
	sprintf(f->result, "B(%d,%d)", f->x, f->y);
	f->result_len = strlen(f->result);
	
	f->result2x = sqrt ( pow(static_circle.x - f->x, 2) + pow(static_circle.y - f->y, 2));
	sprintf(f->result2, "%d = sqrt( (%d)^2 + (%d)^2 )", f->result2x,
		static_circle.x - f->x, static_circle.y - f->y);
	f->result2_len = strlen(f->result2);

	static_point.x = 300;
	static_point.y = 300;
	sprintf(static_point.result, "A(%d,%d)", static_point.x, static_point.y);
	static_point.result_len = strlen(static_point.result);
}

void compute_func3(function * f){
	if(!f->is_bounded){
		f->is_bounded = true;
		f->boundry.left = 200;
		f->boundry.right = 201;
		f->boundry.top = 400;
		f->boundry.bottom = 500;
		ClipCursor(&f->boundry);
		return;
	}

	// Line A of length 100 px:
	static_triangle.a.x1 = 200;
	static_triangle.a.y1 = 500;
	
	static_triangle.a.x2 = 400;
	static_triangle.a.y2 = 500;
	
	// Line B of length 100 px:
	static_triangle.b.x1 = 400;
	static_triangle.b.y1 = 500;
	
	static_triangle.b.x2 = 400;
	static_triangle.b.y2 = 400;
	
	// Line C:
	static_triangle.c.x1 = 400;
	static_triangle.c.y1 = 400;
	
	static_triangle.c.x2 = 200;
	static_triangle.c.y2 = 500;

	f->y = mouse_y;
	f->x = 200;

	sprintf(f->result, "%d degrees", mouse_y);
	f->result_len = strlen(f->result);
}
