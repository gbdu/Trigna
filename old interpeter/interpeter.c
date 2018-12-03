#define WORDLEN 100 // Max chars in a word
#define OPLEN 2 // Max operator chars

// in-house representation of script tokens, as translated by is_delimiter
enum tokens {
	tinvalid = 0, tint, tprint, tquote, tspace, topenbracket, tclosebracket,
	tbracket, tbrace, topeningbrace, tclosingbrace, tdoublequotes, tsinglequote, tstddelim,
	tequalsignsign, tdigit, tnotdigit, tsetpixel, tcomma, twhileloop, tplussign
};

// Retrieve next expected token or word as delimited by delimiter
int next_word(char * in, char * word, enum tokens expected_delimiter);

/* Same as above but includes the delimiter in the word string
If we are at the end of the statement, and a delimiter is not appended
bool * eos is set to 1. */

int next_word_d(char * in, char * word, enum tokens expected_delimiter, bool * eos);

// Parse an entire script
int parse_script(int id, char * scr);

// parse a single line (anything delimited by ;) from the text
int parse_statement(char * in, script * scr); 

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

int parse_statement(char * in, script * scr){
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
		else if(tokenum == tprint){ // Print a value
			in -= advance;
			handle_print(scr, in, tok1, tok2, gerr);
			break;
		}
		else if(tokenum == tsetpixel){
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
				sprintf(gerr, "\a\t\tUnknown token '%s' on line %d: %s", tok1,
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
	
	reset_script_data(vascripts); // Just incase the last script was invalid

	curr_script_line = 1;
	
	char * curr_statement; // Current line
	char * strtok_content = scr_content; // first argument to strtok
	
	char * braces_content = (char*) malloc(strlen(scr_content)+1); // Incase we find a {} set
	char * braces_full_content = (char*) malloc(strlen(scr_content)+1); // Incase we find a {} set

	static char tok1[WORDLEN];
	tok1[0] = 0;
	static char gerr[100];
	gerr[0] = 0;
	enum tokens tokenum;
	
	int content_pos = 0;
	int statement_len = 0;
	int last_stat_len = 0;

	while(curr_statement = strtok(strtok_content, ";" )){
		statement_len = strlen(curr_statement);
		curr_statement = fix_statement(curr_statement);
		if(curr_statement != NULL){
			next_word_d(curr_statement, tok1, tstddelim, NULL);
			tokenum = get_keyword(tok1); // Get the first word
			/*
			if(tokenum == twhileloop){
				notify(scr_content);
				notify(curr_statement);
				if(!get_cbraces_value(scr_content, braces_content, gerr)){
					post_line_out(gerr);
					free(braces_content);
					return 0;
				}
				if(!parse_statement(braces_content, this_script)){
					free(braces_content);
					return 0;
				}
				break;
			}
			*/
			if(!parse_statement(curr_statement, this_script)){
				free(braces_content);
				return 0;
			}
		}

		content_pos += statement_len;
		last_stat_len = statement_len;

		strtok_content = NULL; // For strtok
	}
	return 1;
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
		case topeningbrace:
			return c == '{';
		case tclosingbrace:
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
	in += advance = next_word_d(in, out, topeningbrace, NULL);
	int outlen = strlen(out);
	if(out[outlen-1] != '{'){
		sprintf(gerr, "\a\t\tExpected '{' on line %d: `%s` `%c`", curr_script_line, out);
		return 0;
	}
	
	in += advance = next_word_d(in, out, tclosingbrace, NULL);
	outlen = strlen(out);
	if(out[outlen-1] != '}'){
		sprintf(gerr, "\a\t\tExpected '}' for on line %d: `%s`.",
			curr_script_line, out);
		return 0;
	}
	
	out[outlen-1] = 0; // Set the '}' to a null char
	//strip_spaces(out, out);
	
	return advance;
}
