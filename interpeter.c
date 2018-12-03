#define WORDLEN 100 // Max chars in a word
#define OPLEN 2 // Max operator chars

// in-house representation of script tokens, as translated by is_delimiter
enum tokens {
	tinvalid = 0, tint, tprint, tquote, tspace, topenbracket, tclosebracket,
	tbracket, tbrace, topenbrace, tclosebrace, tdoublequotes, tsinglequote, tstddelim,
	tequalsignsign, tdigit, tnotdigit, tsetpixel, tcomma, twhileloop, tplussign, trgb
};

// Retrieve next expected token or word as delimited by delimiter
int next_word(char * in, char * word, enum tokens expected_delimiter);

/* Same as above but includes the delimiter in the word string
If we are at the end of the statement, and a delimiter is not appended
bool * eos is set to 1. */

int next_word_d(char * in, char * word, enum tokens expected_delimiter, bool * eos);

// Parse an entire script
int parse_script(int id, char * scr);

// parse a single line (anything delimited by ;) from the once section
int parse_once_statement(char * in, script * scr); 

// parse a single line (anything delimited by ;) from the body section
int parse_body_statement(char * in, script * scr); 

int parse_once_section(script * scr, char * in, char * buff1, char * buff2, char * gerr);
int parse_body_section(script * scr, char * in, char * buff1, char * buff2, char * gerr);

// Fix statement for \n and whitespace beginnings, also counts new lines.
char * fix_statement(char * statement);

// This function checks if the variable name is valid for declaration:
bool valid_var_name(char * name); // return 0 if the variable name is invalid

// Returns 1 if in is a valid operator
bool is_operator(char * in);

// determines whether c is the delimiter indicated by the delimiter.
bool is_delimiter(char c, enum tokens delim );

// Transforms a spaces-free string to an in-house token, last character is always ignored
enum tokens get_keyword(char * word);

// tok1 will always be "int"
void handle_int_declare(script * scr, char * in, char * tok1, char * tok2, char * gerr);

void handle_rgb_declare(script * scr, char * in, char * tok1, char * tok2, char * gerr);

// This function will take tok1 as the name of the variable 
void handle_int_assign(script * scr, char * in, char * tok1, char * tok2, char * gerr);

// tok1 will always be "print(" 
void handle_print(script * scr, char * in, char * tok1, char * tok2, char * gerr);

// tok1 will always be "setpixel("
void handle_setpixel(script * scr, char * in, char * tok1, char * tok2, char * gerr);

void handle_whileloop(script * scr, char * in, char * tok1, char * tok2, char * gerr);

void handle_int_increment(script * scr, char * in, char * tok1, char * tok2, char * gerr);

// Remove all spaces found in str and put the result in newstr
void strip_spaces(char * str, char * newstr);

// Gets the value x in `  ( x )  `
int get_brackets_value(char * in, char * out, char * gerr);

// Gets the value x in `  " x "  `
int get_dquotes_value(char * in, char * out, char * gerr);

int parse_body_statement(char * in, script * scr){
	if(!*in) // Empty statement
		return ;
	static char tok1[WORDLEN];
	static char tok2[WORDLEN]; 
	static char gerr[100] ; // for general errors
	static int advance ;
	static enum tokens tokenum ; 
	
	gerr[0] = 0;
	advance = 0;

	bool eos = 0;

	// This loop will break if a general error is set or we're at the EOS
	do {
		advance = next_word_d(in, tok1, tstddelim, &eos);
		tokenum = get_keyword(tok1); // Get the first word
		in += advance;

		if(tokenum == tsetpixel){
			in -= advance;
			handle_setpixel(scr, in, tok1, tok2, gerr);
			break;
		}
		else if(tokenum == twhileloop){
			handle_whileloop(scr, in-1, tok1, tok2, gerr);
			break; 
		}
		else if(tokenum == tinvalid){
			int tok1len = strlen(tok1);
			int inlen = strlen(in);
			
			// Check if this is an increment operation:
			if(in[inlen-1] == '+' && in[inlen-2] == '+'){
				in -= advance;
				handle_int_increment(scr, in, tok1, tok2, gerr);
				return ;
			}
			
			// Check if this is a variable for assignment:
			tok1[tok1len-1] = 0; // Nullify the delimiter
			int success ;
			int val = get_int(scr, tok1, &success);
			if(success != -1){ // An int was found
				handle_int_assign(scr, in, tok1, tok2, gerr);
			}
			else{
				sprintf(gerr, "\a\t\tUnknown token '%s' in body section on line %d: %s", tok1,
					curr_script_line, in - advance);
				break ;
			}
		}
		else {
			sprintf(gerr, "\a\t\tInvalid once section token '%s' in body section on line %d.",
				tok1, curr_script_line);
		}
	} while(!eos && !*gerr);
	
	if(*gerr){
		post_line_out(gerr); // If there is a general error, print it
		return 0; // failure
	}
	return 1; // success
}


int parse_once_statement(char * in, script * scr){
	if(!*in) // Empty statement
		return ;
	static char tok1[WORDLEN];
	static char tok2[WORDLEN]; 
	static char gerr[100] ; // for general errors
	static int advance ;
	static enum tokens tokenum ; 
	
	
	gerr[0] = 0;
	advance = 0;

	bool eos = 0;

	// This loop will break if a general error is set or we're at the EOS
	do {
		advance = next_word_d(in, tok1, tstddelim, &eos);
		tokenum = get_keyword(tok1); // Get the first word
		in += advance;
		
		if(tokenum == tint){ // Declare and assign an int
			handle_int_declare(scr, in, tok1, tok2, gerr);
			break;
		}
		else if(tokenum == trgb){ // RGB declare and assign
			handle_rgb_declare(scr, in, tok1, tok2, gerr);
			break;
		}
		else if(tokenum == tprint){ // Print a value
			in -= advance;
			handle_print(scr, in, tok1, tok2, gerr);
			break;
		}
		else {			
			int tok1len = strlen(tok1);
			tok1[tok1len-1] = 0; // Nullify the delimiter

			// Check if this is a variable for incrementation:
			if(tok1[tok1len-2] == '+' && tok1[tok1len-3] == '+'){
				sprintf(gerr, "a\t\tIllegal increment operator in once section.");
				break;
			}
			
			// Check if this is a variable for assignment:
			int success ;
			int val = get_int(scr, tok1, &success);
			if(success != -1){ // An int was found
				handle_int_assign(scr, in, tok1, tok2, gerr);
			}
			else{
				sprintf(gerr, "\a\t\tUnknown token '%s' in once section on line %d: %s", tok1,
					curr_script_line, in - advance);
				break ;
			}
		}
	} while(!eos && !*gerr);
	
	if(*gerr){
		post_line_out(gerr); // If there is a general error, print it
		return 0; // failure
	}
	return 1; // success
}

bool valid_var_name(char * name){
	if(strlen(name) > MAX_VAR_NAME-1){
		return 0;
	}
	else if(isdigit(*name)){ // 1.) First character cannot be a digit
		return 0;
	}
	else{
		char * orig = name;
		while(*name){
			if(!isalnum(*name)){ // 2.) Has to be alphabetical or numeric 
				return 0;
			}
			name++;
		}
		name = orig;
		static char tmp[WORDLEN+1]; // To add a useless character to the end, like get_keyword wants
		sprintf(tmp, "%sX", name);
		return get_keyword(tmp) ? 0 : 1; // 3.) Cannot be a Trigna keyword
	}
}

int parse_script(int id, char * scr_content){
	vascripts++; // presumed valid for now

	script * this_script = &script_list[vascripts];
	this_script->id = vascripts;
	
	reset_script_data(vascripts);

	curr_script_line = 1;
	
	char * buff1 = (char*) malloc(strlen(scr_content)+1);
	char * buff2 = (char*) malloc(strlen(scr_content)+1);
	char gerr[100];

	int ret = parse_once_section(this_script, scr_content, buff1, buff2, gerr);
	if(!ret){
		free(buff1);
		free(buff2);
		return 0;
	}
	scr_content += ret;
		
	ret = parse_body_section(this_script, scr_content, buff1, buff2, gerr);
	if(!ret){
		free(buff1);
		free(buff2);
		return 0;
	}
}

int parse_once_section(script * scr, char * in, char * buff1, char * buff2, char * gerr){
	char tok1[WORDLEN+1];
	char tok2[WORDLEN+2];

	int advance = next_word(in, tok1, topenbrace);
	strip_spaces(tok1, tok1);
	if(strcmp(tok1, "once")){ 
		strcpy(gerr, "\a\t\tMissing 'once' section of script.");
		post_line_out(gerr);
		return 0;
	}

	advance += get_cbraces_value(in + advance, buff1, gerr);

	char * curr_statement = NULL;
	char * stringtoken = buff1;

	while(curr_statement = strtok(stringtoken, ";")){
		curr_statement = fix_statement(curr_statement);
		if(!parse_once_statement(curr_statement, scr ) ) {
			return 0;
		}
		stringtoken = NULL;
	}
	
	return advance;
}

int parse_body_section(script * scr, char * in, char * buff1, char * buff2, char * gerr){
	char * curr_statement = NULL;
	char * stringtoken = in;
	
	while(curr_statement = strtok(stringtoken, ";")){
		curr_statement = fix_statement(curr_statement);
		if(!parse_body_statement(curr_statement, scr)){
			return 0;
		}
		stringtoken = NULL;
	}

	return 1; // success
}

char * fix_statement(char * statement) {
	// Count new lines:
	int c = strlen(statement);
	for(; c ; c--){
		if(statement[c] == '\n'){
			curr_script_line++;
		}
		if(statement[c] == '/' && statement[c+1] == '/'){
			statement[c] = 0; // switch everything from here off, this is a comment
			return statement ;
		}
	}
	statement[c-1] = 0 ; // Switch ; for a NULL char
	while(isspace(*statement)){
		statement++;
	}
	return statement;
}

int next_word_d(char * in, char * word, enum tokens expected_delimiter, bool * eos){
	int spaces = 0;
	while(isspace(*in)) { // Discard and count leading whitespaces
		in++;
		spaces++;
	}
	int count = 0; // For the actual word, not including the spaces prepending spaces, if any. 
	while(*in && count != WORDLEN-1){ // while we're not at a null char
		*word = *in; // Assign current word char to corrosponding in char
		word++; // Next char in word
		count++; // Advance the char counter
		if(is_delimiter(*in, expected_delimiter)){
			break;
		}
		in++; // Next char in in	
	}
	// Terminate the string, not including the delimiter, or cutt-off long word
	*word = 0;
	if(eos != NULL){
		*eos = *in ? 0 : 1; // A delimiter was not appended, so we're at a null char (EOS)
	}
	return spaces + count;
}

int next_word(char * in, char * word, enum tokens expected_delimiter){
	bool is_eos = 0;
	int ret = next_word_d(in, word, expected_delimiter, &is_eos);
	if(!is_eos){ // This is not the end of statement, a delimiter was appended
		word[strlen(word)-1] = 0; // Nullify the delimiter
	}
	return ret - 1;
}

int next_op(char * in, char * op){ // Retrieve next operator
	int next;
	while(isspace(*in)) {
		in++;
		next++;
	} // Discard leading whitespaces
	return is_operator(op);
}

bool is_operator(char * in){
	return (
		*in == '+' ||
		*in == '-' ||
		*in == '*' ||
		*in == '/' );
}

enum tokens get_keyword(char * word){
	int chklen = strlen(word)-1; // Ignore the last character
	if(!strncmp(word, "int", chklen)){
		return tint;
	}
	else if(!strncmp(word, "rgb", chklen)){
		return trgb;
	}
	else if(!strncmp(word, "print", chklen)) {
		return tprint;
	}
	else if(!strncmp(word, "setpixel", chklen)){
		return tsetpixel;
	}
	else if(!strncmp(word, "while", chklen)){
		return twhileloop;
	}
	else{
		return tinvalid; // It's not a tokens
	}
}

bool is_delimiter(char c, enum tokens delim){
	switch (delim){
		case tstddelim:
			return isspace(c) ||
			c == ';' ||
			c == '=' ||
			c == '(' ||
			c == ')' ;
		case tquote:
			return c == '"' || c == '\'';
		case tsinglequote:
			return c == '\'';
		case tbracket:
			return c == '(' || c == ')';
		case topenbracket:
			return c == '(';
		case tclosebracket:
			return c == ')';
		case tbrace:
			return c == '{' || c == '}';
		case topenbrace:
			return c == '{';
		case tclosebrace:
			return c == '}';
		case tdoublequotes:
			return c == '\"';
		case tequalsignsign:
			return c == '=';
		case tcomma:
			return c == ',';
		case tplussign:
			return c == '+';
		case tdigit:
			return isdigit(c);
		case tnotdigit:
			return !isdigit(c); // only if the char is not a digit
		default:
			return 0;
	}
}

void strip_spaces(char * str, char * newstr){
	while(*str){
		if(!isspace(*str)){
			*newstr = *str;
			newstr++;
		}
		str++;
	}
	*newstr = 0; // Null terminate the string
}

int get_brackets_value(char * in, char * out, char * gerr){
	int advance = 0;

	in += advance = next_word_d(in, out, topenbracket, NULL);
	int outlen = strlen(out);
	if(out[outlen-1] != '('){
		sprintf(gerr, "\a\t\tExpected '(' on line %d.", curr_script_line);
		return 0;
	}

	in += advance = next_word_d(in, out, tclosebracket, NULL);
	outlen = strlen(out);
	if(out[outlen-1] != ')'){
		sprintf(gerr, "\a\t\tExpected ')' on line %d.", curr_script_line, in-advance);
		return 0;
	}
	
	out[outlen-1] = 0; // Set the ')' to a null char
	strip_spaces(out, out);
	
	return advance;
}

int get_dquotes_value(char * in, char * out, char * gerr){
	int advance = 0;
	
	in += advance = next_word_d(in, out, tdoublequotes, NULL);
	int outlen = strlen(out) ;
	if(out[outlen-1] != '"'){
		sprintf(gerr, "\a\t\tExpected opening '\"' on line %d: `%s`", 
			curr_script_line, out);
		return 0;
	}

	in += advance = next_word_d(in, out, tdoublequotes, NULL);
	outlen = strlen(out);
	if(out[outlen-1] != '"'){
		sprintf(gerr, "\a\t\t\Expected closing '\"' on line %d: `%s`",
			curr_script_line, out);
		return 0;
	}

	out[outlen-1] = 0;

	return advance;
}

int get_cbraces_value(char * in, char * out, char * gerr){
	int advance = 0;
	int outlen = 0;
	int inlen = 0;
	int opening = 0; // How many close brackets to ignore
	int closing = 0; // Closing braces found so far
	int seek = 0;
	int seek_first = 0; // First { found
	
	advance = next_word_d(in, out, topenbrace, NULL);
	outlen = strlen(out);

	if(out[outlen-1] != '{'){
		sprintf(gerr, "\a\t\tExpected '{' on line %d: `%s`", curr_script_line, out);
		return 0;
	}

	inlen = strlen(in);

	for(; seek != inlen; seek++){
		if(in[seek] == '{'){
			if(!opening){ // first brace
				seek_first = seek;
			}
			opening++;
		}
		else if(in[seek] == '}'){
			closing++;
			if(closing == opening){ // Matching number of closing braces was found
				strncpy(out, in+1, seek-1);
				out[seek-1] = 0;
				return advance + seek;
			}
		}
	}
	
	// Not enough closing braces:
	sprintf(gerr, "\a\t\tMissing %d closing brace%s('}')", opening, opening == 1 ? " " : "s ");
	return 0;
}

/*
int get_int_expr(script * scr, char * in, int * val, char * tok1, char * tok2, char * gerr){
	strip_spaces(in, in);
	int inlen = strlen(in);
	int seek = 0;
	int ival = 0;
	bool reading_digit = 0;

	char * curr_val = (char*) malloc(in+2);
	

	for(; seek != inlen; seek++){
		if(isdigit(in[seek])){
			int len = next_word(in, tok1, tnotdigit);
			if(len >= 6){
				sprintf(gerr, "\a\t\tInteger value is too long on line %d.", curr_script_line);
				return ;
			}
			ival = atoi(tok1);
			seek += len;
		}
		else if(in[seek] == '+'){
		}
	}

	free(curr_val);
}*/
