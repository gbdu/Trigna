void compute_operations(){
	script * scr = &script_list[curr_script_id];

	int c = 0;
	for(; c != scr->increment_count; c++){
		scr->increments[c].var->val += scr->increments[c].amount;
	}
}

