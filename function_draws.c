void draw_circle(HDC hdc, function * f, circle * c);
void draw_triangle(HDC hdc, triangle * t);

void draw_function(HDC, function *);
void draw_func1(HDC hdc, function *);
void draw_func2(HDC hdc, function *);
void draw_func3(HDC hdc, function *);

void draw_function(HDC hdc, function * f){
	if(!f->id){
		return;
	}
	if(f->flicker_count == f->flicker_rate){
		f->is_flicker = !f->is_flicker;
		f->flicker_count = 0;
	}
	f->flicker_count++;

	SelectObject(hdc, f->pen);
	SelectObject(hdc, f->is_flicker ?  f->brush : f->flicker_brush);
	if(f->id == 1){
		draw_func1(hdc, f);
	}
	else if(f->id == 2) {
		draw_func2(hdc, f);
	}
	else if(f->id == 3){
		draw_func3(hdc, f);
	}
	
	pxstrlen(hBuffer, f->result, &f->resultx, &f->resulty);

	SetTextColor(hdc, RGB(0, 200, 250));
	//TextOut(hdc, 5, display_y-f->resulty-5, f->desc, f->desc_len);
}

void draw_circle(HDC hdc, function * f, circle * c){
	// Draw the actual circle
	SelectObject(hdc, c->brush);

	Ellipse(hdc, c->x - c->r, c->y - c->r,
		c->x + c->r, c->y + c->r ) ;
	
	// Draw the center "point"
	Ellipse(hdc, c->x-3, c->y-3, c->x+3, c->y+3); 
	SetTextColor(hdc, RGB(150, 200, 250));
		
	// Draw the result label
	sprintf(c->result, "Radius: %d", c->r);
	pxstrlen(hBuffer, c->result, &c->resultx, &c->resulty);

	TextOut(hdc, c->x-(c->resultx/2), // X axis: Center - text length / 2 (center)
		c->y+c->r+c->resulty, // Y axis: Center + radius + text height 
		c->result, strlen(c->result));
		
	sprintf(c->result, "h: %d k: %d", c->x, c->y);
	pxstrlen(hBuffer, c->result, &c->resultx, &c->resulty);

	TextOut(hdc, c->x-(c->resultx/2), c->y+c->r+(c->resulty*2), c->result, strlen(c->result));
	
	sprintf(c->result, "%d = sqrt( (%d)^2 + (%d)^2 )", c->r, f->x - c->x, f->y - c->y);
	pxstrlen(hBuffer, c->result, &c->resultx, &c->resulty);
	TextOut(hdc, c->x-(c->resultx/2), c->y+c->r+(c->resulty*3), c->result, strlen(c->result));
	
}

void draw_label(HDC hdc, int x, int y, int line, char * label){
	int calcx = x + 5 ; //  extra padding to account for the mouse pointer
	int resultx;
	int resulty;
	pxstrlen(hBuffer, label, &resultx, &resulty);
	if(calcx + resultx + 10 > WINDOW_WIDTH){
		calcx = x - resultx ;
	}
	SetTextColor(hdc, RGB(150, 200, 250));
	TextOut(hdc, calcx, y - (resulty*line), label, strlen(label)); 
}

void draw_func1(HDC hdc, function * f){
	draw_circle(hdc, f, &static_circle);
	SelectObject(hdc, f->is_flicker ?  f->brush : f->flicker_brush);
	Ellipse(hdc, f->x-2.5, f->y-2.5, f->x+2.5, f->y+2.5);
	draw_label(hdc, f->x, f->y, 0.5, f->result);
}

void draw_func2(HDC hdc, function * f){
	Ellipse(hdc, f->x-2.5, f->y-2.5, f->x+2.5, f->y+2.5);
	SelectObject(hdc, static_point.brush);
	Ellipse(hdc, static_point.x-static_point.r, static_point.y-static_point.r, 
		static_point.x+static_point.r,	static_point.y+static_point.r);
	
	draw_label(hdc, f->x, f->y, 0.5, f->result);
	draw_label(hdc, f->x, f->y, 1, f->result2);
	draw_label(hdc, static_point.x, static_point.y, 1, static_point.result);

	MoveToEx(hdc, static_point.x, static_point.y, NULL); // Start line from stationary point
	LineTo(hdc, f->x, f->y);


	//sprintf(f->result, "AB = sqrt( (%d - %d)^2 + (%d - %d)^2 )", 
}

void draw_func3(HDC hdc, function * f){
	Ellipse(hdc, f->x-2.5, f->y-2.5, f->x+2.5, f->y+2.5);
	draw_label(hdc, f->x, f->y, 0.5, f->result);
	
	draw_triangle(hdc, &static_triangle);
}

void draw_triangle(HDC hdc, triangle * t){
	SelectObject(hdc, t->brush);
	
	// Start at point B
	MoveToEx(hdc, t->a.x1, t->a.y1, NULL);
	
	// Line a, move to (right) angle C 
	SelectObject(hdc, t->a.pen);
	LineTo(hdc, t->a.x2, t->a.y2);

	// Line b, move to angle A
	SelectObject(hdc, t->b.pen);
	LineTo(hdc, t->b.x2, t->b.y2);
	
	// Line c, move back to point B
	SelectObject(hdc, t->c.pen);
	LineTo(hdc, t->c.x2, t->c.y2);
}

void draw_pixels(HDC hdc, script * scr){
	int c = 0;
	for(; c != scr->pixel_count; c++){
		SetPixel(hdc, scr->pixels[c].x, scr->pixels[c].y, scr->pixels[c].rgb);
	}
}
