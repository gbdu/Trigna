void handle_print(script * scr, char * in, char * tok1, char * tok2, char * gerr){
	static char out[WORDLEN+40];
	if(!get_brackets_value(in, tok1, gerr)){
		return;
	}
	int tok1len = strlen(tok1);
	if(isdigit(*tok1)){ // This is a digital constant, print it
		if(tok1len >= 6){
			sprintf(gerr, "\a\t\tValue too large for int on line %d: %s", curr_script_line,
				tok1);
		}
		int c = 0;
		while(c != tok1len){
			if(!isdigit(*tok1)){
				sprintf(gerr, "\a\t\tInvalid integer for print() output on line %d: %s", 
					curr_script_line, in);
				return ;
			}
			c++;
		}
		sprintf(out, "\r\t\tAScript output: %d", atoi(tok1));
		post_line_out(out);
	}
	else if(*tok1 == '"'){
		if(!get_dquotes_value(in, tok1, gerr)){
			return;
		}
		sprintf(out, "\r\t\tScript output: %s", tok1);
		post_line_out(out);
	}
	else {
		int success;
		int val = get_int(scr, tok1, &success);
		if(success != -1){ // This is an int, print it
			sprintf(out, "\r\t\tScript output: %d", val);
			post_line_out(out);
		}
		else {
			sprintf(gerr, "Unknown constant for printing on line %d: `%s`", curr_script_line,
			tok1);
		}
	}
}

void handle_int_declare(script * scr, char * in, char * tok1, char * tok2, char * gerr){
	int advance = next_word(in, tok2, tstddelim);
	in += advance;

	if(!valid_var_name(tok2)){
		sprintf(gerr, "\a\t\tInvalid variable name for type int on line %d '%s'",
			curr_script_line, tok2);
		return;
	}

	int success ;
	get_int(scr, tok2, &success);
	if(success != -1){ // A variable by this name already exists
		sprintf(gerr, "\a\t\tRedeclaring variable '%s' on line %d", tok2, curr_script_line);
		return ;
	}

	advance = next_word(in, tok1, tdigit);
	in += advance;

	if(*tok1 != '='){
		sprintf(gerr, "Missing assignment operator on line %d", curr_script_line);
		return;
	}

	next_word(in, tok1, tstddelim);
	
	// Don't advance...
	
	// tok2 points to the var name, tok1 to the int's value (hopefully)

	int val;

	if(isdigit(*tok1)){ // This is a literal integer assignment
		int len = next_word(in, tok1, tnotdigit);
		if(len >= 6) {
			sprintf(gerr, "Integer value is too long on line %d", curr_script_line);
			return ;
		}
		
		val = atoi(tok1);
		
		if(!add_var_int(scr, tok2, val)){
			sprintf(gerr, "\a\t\tToo many ints, int variables have a limit of %d", MAX_INTS);
			return;
		}
		in += len;
	}
	else{ // This is an assignment to another variable
		int success;
		val = get_int(scr, tok1, &success);
		if(success == -1){ // Invalid variable
			sprintf(gerr, "\a\t\tInvalid rvalue in assignment on line %d, no variable by"
			" the name of '%s' previously assigned.", curr_script_line, tok1);
			return;
		}
	}
}

void handle_int_increment(script * scr, char * in, char * tok1, char * tok2, char * gerr){
	int tok1len = strlen(tok1);
	if(tok1[tok1len-1] == '+' && tok1[tok1len-2] == '+' ){ 
		in += next_word(in, tok2, tstddelim);
		int int_id ;
		get_int(scr, tok2, &int_id);
		if(int_id == -1){
			sprintf(gerr, "\a\t\tInvalid int variable for incrementation on line %d: `%s`",
				curr_script_line, in);
			return ;
		}
		if(!add_incrementor(scr, &scr->ints[int_id], 1)){ // increment this var by 1 if it exists
			sprintf(gerr, "a\t\tToo many incrementation operations.");
			return ;
		}
		return ;
	}
}

void handle_int_assign(script * scr, char * in, char * tok1, char * tok2, char * gerr){
	int advance = next_word(in, tok1, tstddelim);
	int tok1len = strlen(tok1);
	
	// check if this is an increment operation first
	in += next_word(in, tok1, tdigit);
	if(*tok1 != '=') {
		sprintf(gerr, "\a\t\tMissing assignment operator on line %d", curr_script_line);
		return;
	}
	
	next_word(in, tok1, tstddelim);
	int val; // To hold the new val

	if(isdigit(*tok1)){ // This is a literal integer assignment
		int len = next_word(in, tok1, tnotdigit);
		if(len >= 5) {
			sprintf(gerr, "\a\t\tInteger value is too long on line %d", curr_script_line);
			return ;
		}
		
		val = atoi(tok1);
	}
	else{ // This is an assignment to another variable
		int success;
		val = get_int(scr, tok1, &success);
		if(success == -1){ // Invalid variable
			sprintf(gerr, "\a\t\tInvalid rvalue in assignment on line %d, no variable by"
			" the name of '%s' previously assigned.", curr_script_line, tok1);
			return;
		}
	}

	set_int(scr, tok1, val);
}

int next_int_arg(script * scr, char * in, int * val, bool last){
	static char tok1[WORDLEN];
	static char tok2[WORDLEN];

	bool eos;
	int advance = next_word_d(in, tok1, tcomma, &eos);
	int tok1len = strlen(tok1);
	
	if(!last && tok1[tok1len-1] != ','){
		return 0;
	}

	int value = 0;
	if(isdigit(*tok1)){
		int tok2len = next_word(in, tok2, tnotdigit);
		if(tok2len != tok1len-1){
			return 0; // not completely a constant
		}
		*val = atoi(tok2);
		return tok2len+1;
	}
	else{
		tok1[strlen(tok1)-1] = 0;
		int success;
		*val = get_int(scr, tok1, &success);
		if(success == -1){
			return 0; // Not an int either
		}
		return tok1len;
	}
}

int next_rgb_set(script * scr, char * in, COLORREF * val, char * gerr){
	int advance = 0;
	int ret = 0;
	static char tok1[WORDLEN];
	int r,g,b;

	if(!get_cbraces_value(in, tok1, gerr)){
		return 0;
	}
	strip_spaces(tok1, tok1);

	advance += ret = next_int_arg(scr, tok1, &r, 0);
	if(!ret){
		sprintf(gerr, "\a\t\tInvalid first argument for Red-Green-Blue set"
		" `%s` on line %d", &tok1[advance], curr_script_line, 0);
		return 0;
	}
	
	advance += ret = next_int_arg(scr, &tok1[advance], &g, 0);
	if(!ret){
		sprintf(gerr, "\a\t\tInvalid second argument for Red-Green-Blue set"
		" `%s` on line %d", &tok1[advance], curr_script_line);
		return 0;
	}
		
	advance += ret = next_int_arg(scr, &tok1[advance], &b, 1); // last arg
	if(!ret){
		sprintf(gerr, "\a\t\tInvalid third argument for Red-Green-Blue set"
		" `%s` on line %d", &tok1[advance], curr_script_line); 
		return 0;
	}
	if(advance != strlen(tok1)){
		sprintf(gerr, "\a\t\tInvalid arguments for Red-Green-Blue set `%s` on"
		" line %d", &tok1[advance], curr_script_line);
	}
	*val = RGB(r, g, b);

	return advance;
}

void handle_setpixel(script * scr, char * in, char * tok1, char * tok2, char * gerr){
	if(scr->pixel_count == MAX_PIXELS-1){
		sprintf(gerr, "\a\t\tToo many pixels, maximum amount of pixels to set are %d", MAX_PIXELS);
		return ;
	}
	if(!get_brackets_value(in, tok1, gerr)){
		return ;
	}
	// setpixel(x, y, {r,g,b} );
	pixel pix;

	int val;
	int advance = next_int_arg(scr, tok1, &val, 0);
	
	if(advance == 0){
		sprintf(gerr, "\a\t\tInvalid first argument for setpixel on line %d: `%s`",
			curr_script_line, tok1) ;
		return ;
	}
	pix.x = val;

	tok1 += advance;
	advance = next_int_arg(scr, tok1, &val, 0);
	if(advance == 0){
		sprintf(gerr, "\a\t\tInvalid second argument for setpixel on line %d: `%s`",
			curr_script_line, tok1) ;
		return ;
	}
	pix.y = val;

	tok1 += advance;

	// Get rgb set {r,g,b}
	advance = next_rgb_set(scr, tok1, &pix.rgb, gerr);
	if(!advance){
		return;
	}

	add_pixel(scr, pix);
}

bool eval_expression(script * scr, char * in, char * tok1, char * tok2){
	
}

void handle_whileloop(script * scr, char * in, char * tok1, char * tok2, char * gerr){
	char * expr = (char*) malloc(strlen(in)); // For the expression
	if(!get_brackets_value(in, expr, gerr)){
		return;
	}

	free(expr);
}
