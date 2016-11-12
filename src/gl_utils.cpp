// copy a shader from a plain text file into a character array
bool parse_file_into_str(const char* file_name, char* shader_str, int max_len)
{
	FILE* file = fopen(file_name, "r");
	int current_len = 0;
	char line[2048];
	shader_str[0] = '\0'; /* reset string */
	if ( !file ) 
	{
		fprintf(stderr, "ERROR: opening file for reading: %s\n", file_name);
		return false;
	}
	strcpy(line, ""); /* remember to clean up before using for first time! */
	while ( !feof(file) ) 
	{
		if ( NULL != fgets(line, 2048, file) )
		{
			current_len += strlen(line); /* +1 for \n at end */
			if ( current_len >= max_len ) 
			{
				fprintf(stderr,
					"ERROR: shader length is longer than string buffer length %i\n",
					max_len);
			}
			strcat(shader_str, line);
		}
	}
	if ( EOF == fclose(file) ) 
	{ 
		// probably unnecesssary validation
		fprintf(stderr, "ERROR: closing file from reading %s\n", file_name);
	return false;
	}
return true;
}

void print_shader_info_log(unsigned int shader_index) 
{
	int max_length = 2048;
	int actual_length = 0;
	char log[2048];
	glGetShaderInfoLog(shader_index, max_length, &actual_length, log);
	printf("shader info log for GL index %u\n%s\n", shader_index, log);
}

bool compile_shader(unsigned int s, const char* file_name) 
{
	char str[MAX_SHADER_LEN];
	if ( !parse_file_into_str(file_name, str, MAX_SHADER_LEN) ) 
	{
		return false; // error msg in sub-function is verbose enough already
	}
	// C type casts are disgusting (char* to const char**)
	const char* p = (const char*)str;
	glShaderSource(s, 1, &p, NULL);
	glCompileShader(s);
	// check for compile errors
	int params = -1;
	glGetShaderiv(s, GL_COMPILE_STATUS, &params);
	if ( GL_TRUE != params ) 
	{
		fprintf(stderr,
			"ERROR: GL shader index %i (%s) did not compile\n", s, file_name);
		print_shader_info_log(s);
		return false; // or exit or something
	}
	return true;
}

void print_program_info_log(unsigned int m_programme_idx) 
{
	int max_length = 2048;
	int actual_length = 0;
	char log[2048];
	glGetProgramInfoLog(m_programme_idx, max_length, &actual_length, log);
	printf("program info log for GL index %u\n%s\n", (int)m_programme_idx, log);
}

bool link_sp(unsigned int sp) 
{
	printf("linking shader program...\n");
	glLinkProgram(sp);
	// check if link was successful
	int params = -1;
	glGetProgramiv(sp, GL_LINK_STATUS, &params);
	if ( GL_TRUE != params ) 
	{
		fprintf(stderr,
			"ERROR: could not link shader program GL index %i\n", (int)sp
			);
		print_program_info_log(sp);
		return false;
	}
	//_print_all_shader_info (sp);
	return true;
}