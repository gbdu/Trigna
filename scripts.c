bool init_script(WIN32_FIND_DATA * script_wfd);

int GetScripts(WIN32_FIND_DATA * list, int limit){
	WIN32_FIND_DATA wfd;
	HANDLE fh = FindFirstFile("scripts/*", &wfd);

	if(fh == INVALID_HANDLE_VALUE){
		error("Cannot open scripts folder.");
	}

	int counter = 1;
	
	char * ExtToMatch  ; // Pointer to the extension substring in the filename

	while(FindNextFile(fh, &wfd) || counter == limit){
		if((ExtToMatch = laststrchr(wfd.cFileName, '.')) == NULL ) {
			continue; // This file has no extension, skip it.
		}

		strlower(ExtToMatch, ExtToMatch);
		
		if(!strcmp(SCRIPT_EXT, ExtToMatch) ) {
			memcpy(&list[counter], &wfd, sizeof(wfd));
			counter++;
		}
	}

	return counter;
}

int GetFileContent(WIN32_FIND_DATA * wfd, char * content){
	OFSTRUCT fos;
	char path[MAX_PATH+2];
	sprintf(path, "scripts/%s", wfd->cFileName);

	HFILE file = OpenFile(path, &fos, OF_READ );
	
	if(file == HFILE_ERROR) {
		char x[20+MAX_PATH];
		sprintf(x, "from: Cannot read %s", path); 
		post_line_out(x);
		return -1;
	}

	DWORD BytesToRead = (wfd->nFileSizeHigh * MAXDWORD) + wfd->nFileSizeLow;
	DWORD BytesRead; // ReadFile already sets this to 0 for us

	if(!ReadFile((HANDLE) file, content, BytesToRead, &BytesRead, NULL)) {
		char x[20+MAX_PATH];
		sprintf(x, "from: Cannot read %s", path); 
		return -1;
	}

	if(BytesRead != BytesToRead){
		notifye("ReadFile bad bytes");
		return -1;
	}

	_lclose(file);
	content[BytesRead] = 0; 

	return 1; // success
}

void setup_scripts(void){
	WIN32_FIND_DATA file_script_list[SCRIPTS_LIMIT];
	
	post_line_out("Setting up scripts...");

	int scripts = GetScripts(file_script_list, SCRIPTS_LIMIT);
	if(!scripts){
		post_line_out("No scripts to setup.");
		return; // No scripts, nothing to do
	}
	
	char out[FILENAME_MAX+50];
	int scriptc = 1;

	while(scriptc != scripts){
		if(!init_script(&file_script_list[scriptc])){
			sprintf(out, "\aFailed loading script %d ('%s')", scriptc,
				file_script_list[scriptc].cFileName);
		}
		else{
			int var_count = script_list[vascripts].int_count + script_list[vascripts].rgb_count;
			sprintf(out, "\vSuccessfully loaded script ID%d ('%s') with %d variable(s).",
				vascripts, file_script_list[scriptc].cFileName, var_count);
		}
		post_line_out(out);
		scriptc++;		
	}

	return ;
}

bool init_script(WIN32_FIND_DATA * script_wfd){
	char x[FILENAME_MAX+50];
	sprintf(x, "	Loading script %s ...", script_wfd->cFileName);
	post_line_out(x);

	char * script = (char *) malloc((script_wfd->nFileSizeHigh * MAXDWORD) +
		script_wfd->nFileSizeLow); // Char * to put script content into
	if(!script)
		error("Out of memory.");
		
	if(GetFileContent(script_wfd, script) == -1){
		sprintf(x, "\a\tFailed loading %s");
		post_line_out(x);
	}
	
	int ret = parse_script(vascripts, script);	
	free(script);
	return ret;
}

