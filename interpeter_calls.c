bool add_var_int(script * scr, char * name, int val);
/* Adds the value val to the integer list of script *scr, with the name given
returns 1 or 0 based on the success of the operation */

int get_int(script * script, char * name, int * int_id);
/* Return the value of integer of this name in this script, sets int_id to either -1
0 to indicate failure, or the variable's ID within the script's int list */

void set_int(script * script, char * name, int newval);

bool add_var_int(script * scr, char * name, int val){
	if(scr->int_count == MAX_INTS-1){
		return 0;
	}

	strcpy(scr->ints[scr->int_count].name, name); // copy var name
	scr->ints[scr->int_count].val = val; // copy the value
	
	scr->int_count++; // Move on to next item on the variable list for this script
	return 1;
}

int get_int(script * script, char * name, int * int_id){
	int c = 0;
	for(; c != script->int_count; c++){
		if(!strcmp(script->ints[c].name, name)){
			*int_id = c;
			return script->ints[c].val;
		}
	}
	*int_id = -1; // The variable was not found
}

void set_int(script * script, char * name, int newval){
	int c = 0;
	for(; c != script->int_count; c++){
		if(!strcmp(script->ints[c].name, name)){
			script->ints[c].val = newval;
			return ;
		}
	}
}

bool add_var_rgb(script * scr, char * name, COLORREF val){
	if(scr->rgb_count == MAX_RGBS-1){
		return 0;
	}

	strcpy(scr->rgbs[scr->rgb_count].name, name); // copy var name
	scr->rgbs[scr->rgb_count].cref = val; // copy the value
	scr->rgb_count++; // Move on to next item on the variable list for this script
	return 1;
}

COLORREF get_rgb(script * script, char * name, int * rgb_id){
	int c = 0;
	for(; c != script->rgb_count; c++){
		if(!strcmp(script->rgbs[c].name, name)){
			*rgb_id = c;
			return script->rgbs[c].cref;
		}
	}
	*rgb_id = -1; // The variable was not found
}

void set_rgb(script * script, char * name, COLORREF newval){
	int c = 0;
	for(; c != script->rgb_count; c++){
		if(!strcmp(script->rgbs[c].name, name)){
			script->rgbs[c].cref = newval;
			return ;
		}
	}
}

bool add_pixel(script * scr, pixel px ){
	if(scr->pixel_count == MAX_PIXELS-1){
		return 0;
	}
	scr->pixels[scr->pixel_count] = px;
	scr->pixel_count++;
	return 0;
}

int add_incrementor(script * scr, int_var * var, int amount){
	if(scr->increment_count == MAX_INCREMENTS-1){
		return 0;
	}

	scr->increments[scr->increment_count].var = var;
	scr->increments[scr->increment_count].amount = amount;
	
	scr->increment_count++;
}

void reset_script_data(int id){
	script_list[id].int_count = 0;
	script_list[id].rgb_count = 0;
}

