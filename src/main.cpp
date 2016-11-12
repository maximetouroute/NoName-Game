#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#include "math_funcs.h"
#include "obj_parser.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#define STBI_ASSERT(x)
#include "stb_image.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_SHADER_LEN 4096
#include <math.h>
#include "gameIntelligence.cpp"
#include "obj_parser.cpp"
#include "gl_utils.cpp"

GLFWwindow* window;

// dimensions of the window drawing surface
int gl_width = WINDOW_WIDTH;
int gl_height = WINDOW_HEIGHT;


// tiny method used to display the score digits
int get_digit(int number, int digit)
{
	number /= pow( 10.0, digit );
	return number % 10;
}

int init_context()
{
	window = NULL;
	if ( !glfwInit() ) 
	{
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 0;
	}

// OSX support. OpenGL4 doesnt seem render textures properly
#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(gl_width, gl_height, "NoName Game", NULL, NULL);
	if ( !window ) 
	{
		fprintf(stderr, "ERROR: opening OS window\n");
		return 0;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	return 1;
}

void loadTGATexture(const char* path, const int bufferIndex)
{
	glActiveTexture(GL_TEXTURE0 + bufferIndex);
	glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0 + bufferIndex);
		// Few variables for the stbi_load_from_file method.
	int a = 3, b = 3, c = 3;
	int* comp = &a;
	int* x = &b;
	int* y = &c;

	unsigned char* texturedata;
	FILE* texturefile = fopen(path, "r");
	texturedata = stbi_load_from_file(texturefile, x, y, comp, 4);

			// Few printf to verify if the 
			//printf("Error log opening texture file : %s\n", stbi_failure_reason());
	fclose(texturefile);
			//printf("Dimensions :\n");
			//printf("X : %i\n", *x);
			//printf("Y : %i\n", *y);
			//printf("Number of channels : %i\n", *comp);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)*x, (GLsizei)*y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texturedata);
	glBufferData(GL_TEXTURE_2D, sizeof(*texturedata), texturedata, GL_STATIC_DRAW);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);		
}

int main() 
{
	/* Initialisation */

	if ( !init_context() )
	{
		printf("err: could not init opengl context");
		return 0;
	}

	// get OpenGL version info 
	const GLubyte* renderer;
	const GLubyte* version;
	renderer = glGetString(GL_RENDERER);
	version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);


	/* Load Game meshes data from obj files */

	GLuint platform_program;
	GLuint platform_vao;
	GLuint platform_points_vbo, platform_normals_vbo;
	int platform_vertex_count = 0; // number of vertex points loaded

	GLuint player_program;
	GLuint player_vao;
	GLuint player_points_vbo, player_normals_vbo;
		int player_vertex_count = 0; // number of vertex points loaded

		GLuint coin_program;
		GLuint coin_vao;
		GLuint coin_points_vbo, coin_normals_vbo;
		int coin_vertex_count = 0; // number of vertex points loaded

		GLuint pipe_program;
		GLuint pipe_vao;
		GLuint pipe_points_vbo, pipe_normals_vbo;
		int pipe_vertex_count = 0; // number of vertex points loaded


	// ** The platform mesh

	// Load obj file, bind buffers
		{
		GLfloat* vp = NULL; // array of vertex points
		GLfloat* vn = NULL; // array of vertex normals
		GLfloat* vt = NULL; // array of texture coordinates

		if ( !load_obj_file("./Meshes/platform.obj", vp, vt, vn, platform_vertex_count) )
		{
			fprintf(stderr, "ERROR: could not find mesh file...\n");
			return 0;
		}

		glGenBuffers (1, &platform_points_vbo);
		glBindBuffer (GL_ARRAY_BUFFER, platform_points_vbo);
		// copy our points from the header file into our VBO on graphics hardware
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * platform_vertex_count , vp, GL_STATIC_DRAW);
		// and grab the normals
		glGenBuffers (1, &platform_normals_vbo);
		glBindBuffer (GL_ARRAY_BUFFER, platform_normals_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * platform_vertex_count, vn, GL_STATIC_DRAW);

		glGenVertexArrays (1, &platform_vao);
		glBindVertexArray (platform_vao);
		glEnableVertexAttribArray (0);
		glBindBuffer (GL_ARRAY_BUFFER, platform_points_vbo);
		glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (1);
		glBindBuffer (GL_ARRAY_BUFFER, platform_normals_vbo);
		glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		// free allocated memory
		free(vp);
		free(vn);
		free(vt);
	}

	// Load shaders, compile, link uniforms
	{
		GLuint vs, fs;
		vs = glCreateShader(GL_VERTEX_SHADER);
		fs = glCreateShader(GL_FRAGMENT_SHADER);
		compile_shader(vs, "./Shaders/platform.vert");
		compile_shader(fs, "./Shaders/platform.frag");
		platform_program = glCreateProgram();
		glAttachShader(platform_program, fs);
		glAttachShader(platform_program, vs);
		glBindAttribLocation(platform_program, 0, "vp");
		glBindAttribLocation(platform_program, 1, "vn");
		link_sp(platform_program);
	}


	// ** The player mesh

	// Load obj file, bind buffers
	{
		GLfloat* vp = NULL; // array of vertex points
		GLfloat* vn = NULL; // array of vertex normals
		GLfloat* vt = NULL; // array of texture coordinates

		if ( !load_obj_file("./Meshes/character.obj", vp, vt, vn, player_vertex_count) ) 
		{
			fprintf(stderr, "ERROR: could not find mesh file...\n");
			return 0;
		}

		glGenBuffers (1, &player_points_vbo);
		glBindBuffer (GL_ARRAY_BUFFER, player_points_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * player_vertex_count , vp, GL_STATIC_DRAW);
		glGenBuffers (1, &player_normals_vbo);
		glBindBuffer (GL_ARRAY_BUFFER, player_normals_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * player_vertex_count, vn, GL_STATIC_DRAW);
		glGenVertexArrays (1, &player_vao);
		glBindVertexArray (player_vao);
		glEnableVertexAttribArray (0);
		glBindBuffer (GL_ARRAY_BUFFER, player_points_vbo);
		glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (1);
		glBindBuffer (GL_ARRAY_BUFFER, player_normals_vbo);
		glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		free(vp);
		free(vn);
		free(vt);
	}

	// Load shaders, compile, link uniforms
	{
		GLuint vs, fs;
		vs = glCreateShader(GL_VERTEX_SHADER);
		fs = glCreateShader(GL_FRAGMENT_SHADER);
		compile_shader(vs, "./Shaders/character.vert");
		compile_shader(fs, "./Shaders/character.frag");
		player_program = glCreateProgram();
		glAttachShader(player_program, fs);
		glAttachShader(player_program, vs);
		glBindAttribLocation(player_program, 0, "vp");
		glBindAttribLocation(player_program, 1, "vn");
		link_sp(player_program);
	}

	// ** The coin mesh

	// Load obj file, bind buffers
	{
		GLfloat* vp = NULL; // array of vertex points
		GLfloat* vn = NULL; // array of vertex normals
		GLfloat* vt = NULL; // array of texture coordinates

		if ( !load_obj_file("./Meshes/coin.obj", vp, vt, vn, coin_vertex_count) ) 
		{
			fprintf(stderr, "ERROR: could not find mesh file...\n");
			return 0;
		}
		
		glGenBuffers(1, &coin_points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, coin_points_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * coin_vertex_count, vp, GL_STATIC_DRAW);
		glGenBuffers(1, &coin_normals_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, coin_normals_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * coin_vertex_count, vn, GL_STATIC_DRAW);
		glGenVertexArrays(1, &coin_vao);
		glBindVertexArray(coin_vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, coin_points_vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, coin_normals_vbo);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		free(vp);
		free(vn);
		free(vt);
	}

	// Load shaders, compile, link uniforms
	{
		GLuint vs, fs;
		vs = glCreateShader(GL_VERTEX_SHADER);
		fs = glCreateShader(GL_FRAGMENT_SHADER);
		compile_shader(vs, "./Shaders/coin.vert");
		compile_shader(fs, "./Shaders/coin.frag");
		coin_program = glCreateProgram();
		glAttachShader(coin_program, fs);
		glAttachShader(coin_program, vs);
		glBindAttribLocation(coin_program, 0, "vp");
		glBindAttribLocation(coin_program, 1, "vn");
		link_sp(coin_program);
	}

	// ** The tunnel/pipe mesh

	// Load obj file, bind buffers
	{
		GLfloat* vp = NULL; // array of vertex points
		GLfloat* vn = NULL; // array of vertex normals
		GLfloat* vt = NULL; // array of texture coordinates

		if ( !load_obj_file("./Meshes/pipe.obj", vp, vt, vn, pipe_vertex_count) )  
		{
			fprintf(stderr, "ERROR: could not find mesh file...\n");
			return 0;
		}

		glGenBuffers(1, &pipe_points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, pipe_points_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * pipe_vertex_count, vp, GL_STATIC_DRAW);
		glGenBuffers(1, &pipe_normals_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, pipe_normals_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * pipe_vertex_count, vn, GL_STATIC_DRAW);
		glGenVertexArrays(1, &pipe_vao);
		glBindVertexArray(pipe_vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, pipe_points_vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, pipe_normals_vbo);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		free(vp);
		free(vn);
		free(vt);
	}

	// Load shaders, compile, link uniforms
	{
		GLuint vs, fs;
		vs = glCreateShader(GL_VERTEX_SHADER);
		fs = glCreateShader(GL_FRAGMENT_SHADER);
		compile_shader(vs, "./Shaders/pipe.vert");
		compile_shader(fs, "./Shaders/pipe.frag");
		pipe_program = glCreateProgram();
		glAttachShader(pipe_program, fs);
		glAttachShader(pipe_program, vs);
		glBindAttribLocation(pipe_program, 0, "vp");
		glBindAttribLocation(pipe_program, 1, "vn");
		link_sp(pipe_program);
	}


	// **  Game menu screens : made of a simple plane with a texture on it
	// displayed texture is controlled by a uniform

	GLuint menus_program;
	GLuint menus_vao;
	GLuint menus_vbo_points;
	GLuint menus_vbo_texcoords;

	{
		GLfloat points[] = {
			-1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f
		};

	// Texcoords 
		GLfloat texcoords[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		};

		// ask GL to generate a VBO, and give us a handle to refer to it with
		glGenBuffers(1, &menus_vbo_points);
		// "bind" the VBO. all future VBO operations affect this VBO until we bind a different one
		glBindBuffer(GL_ARRAY_BUFFER, menus_vbo_points);
		// copy our points from RAM into our VBO on graphics hardware
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
		// ask GL to generate a VAO, and give us a handle to refer to it with
		// Table to associate one texture coordinate per vertex
		glGenBuffers(1, &menus_vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, menus_vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

		glGenVertexArrays(1, &menus_vao);
		glBindVertexArray(menus_vao);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, menus_vbo_points);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, menus_vbo_texcoords);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL); // Texture coordinates

		// Load texture files, load texture data in buffers...
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		// START MENU TEXTURE
		loadTGATexture("./Textures/start_game.tga", 0);
		// INSTRUCTIONS GAME TEXTURE
		loadTGATexture("./Textures/instructions_screen.tga", 2);
		// END LEVEL TEXTURE
		loadTGATexture("./Textures/end_level.tga", 4);
		// END GAME TEXTURE
		loadTGATexture("./Textures/end_game.tga", 6);
	}

	// Load shaders, compile, link uniforms
	{

		GLuint vs, fs;
		vs = glCreateShader(GL_VERTEX_SHADER);
		fs = glCreateShader(GL_FRAGMENT_SHADER);
		compile_shader(vs, "./Shaders/menus.vert");
		compile_shader(fs, "./Shaders/menus.frag");
		menus_program = glCreateProgram();
		glAttachShader(menus_program, fs);
		glAttachShader(menus_program, vs);
		glBindAttribLocation(menus_program, 0, "vp");
		glBindAttribLocation(menus_program, 1, "vn");
		link_sp(menus_program);
	}


	// ** The score : an object with a 0 to 9 digits texture. 
	//The shader offset the texture position to display the right digit

	// SCORE TEXTURE
	loadTGATexture("./Textures/score_texture.tga", 8);

	GLuint score_program;
	GLuint score_vao;

	GLuint score_vbo_points;
	GLuint score_vbo_texcoords;

	{
		GLfloat points[] = {
			-0.2f, 0.2f, 0.0f,
			0.2f, 0.2f, 0.0f,
			-0.2f, -0.2f, 0.0f,
			0.2f, 0.2f, 0.0f,
			-0.2f, -0.2f, 0.0f,
			0.2f, -0.2f, 0.0f
		};
		GLfloat texcoords[] = {
			0.0f, 0.0f,
			0.1f, 0.0f,
			0.0f, 1.0f,
			0.1f, 0.0f,
			0.0f, 1.0f,
			0.1f, 1.0f
		};

		glGenBuffers(1, &score_vbo_points);
		glBindBuffer(GL_ARRAY_BUFFER, score_vbo_points);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
		glGenBuffers(1, &score_vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, score_vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
		glGenVertexArrays(1, &score_vao);
		glBindVertexArray(score_vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, score_vbo_points);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, score_vbo_texcoords);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL); 
	}

	// Load shaders, compile, link uniforms
	{

		GLuint vs, fs;
		vs = glCreateShader(GL_VERTEX_SHADER);
		fs = glCreateShader(GL_FRAGMENT_SHADER);
		compile_shader(vs, "./Shaders/score.vert");
		compile_shader(fs, "./Shaders/score.frag");
		score_program = glCreateProgram();
		glAttachShader(score_program, fs);
		glAttachShader(score_program, vs);
		glBindAttribLocation(score_program, 0, "vp");
		glBindAttribLocation(score_program, 1, "vn");
		link_sp(score_program);
	}



	// ** Initialise the game logic and all GLSL programs

	// Init the camera and game elements
	init_game();

	/// Get all the matrices/variables locations, and update values
	glUseProgram(platform_program);
	int M_loc = glGetUniformLocation(platform_program, "M");
	int V_loc = glGetUniformLocation(platform_program, "V");
	int P_loc = glGetUniformLocation(platform_program, "P");
	int health_loc = glGetUniformLocation(platform_program, "health");
	int spec_exp_loc = glGetUniformLocation(platform_program, "spec");
	glUniformMatrix4fv(M_loc, 1, GL_FALSE, identity_mat4().m);
	glUniformMatrix4fv(V_loc, 1, GL_FALSE, camera.view_matrix.m);
	glUniformMatrix4fv(P_loc, 1, GL_FALSE, camera.perspective_matrix.m);



	glUseProgram(player_program);
	int player_M_loc = glGetUniformLocation(player_program, "M");
	int player_V_loc = glGetUniformLocation(player_program, "V");
	int player_P_loc = glGetUniformLocation(player_program, "P");
	mat4 player_rotation_matrix = identity_mat4();
	mat4 player_rotation_matrix2 = identity_mat4();
	glUniformMatrix4fv(player_M_loc, 1, GL_FALSE, get_player_position().m);
	glUniformMatrix4fv(player_V_loc, 1, GL_FALSE, camera.view_matrix.m);
	glUniformMatrix4fv(player_P_loc, 1, GL_FALSE, camera.perspective_matrix.m);



	glUseProgram(coin_program);
	int coin_M_loc = glGetUniformLocation(coin_program, "M");
	int coin_V_loc = glGetUniformLocation(coin_program, "V");
	int coin_P_loc = glGetUniformLocation(coin_program, "P");
	int coin_color_state_loc = glGetUniformLocation(coin_program, "ext_color_state");
	glUniformMatrix4fv(coin_M_loc, 1, GL_FALSE, identity_mat4().m);
	glUniformMatrix4fv(coin_V_loc, 1, GL_FALSE, camera.view_matrix.m);
	glUniformMatrix4fv(coin_P_loc, 1, GL_FALSE, camera.perspective_matrix.m);

	// Hierarchichal transformation on the coins
	mat4 mat_coins_origin_T = identity_mat4();
	mat4 mat_coins_origin_R = identity_mat4();
	mat4 mat_coins_origin = identity_mat4();
	// Then, the two coins have their own transformation matrix
	mat4 mat_coin1_T = translate(identity_mat4(), vec3(0.12f, 0.0f, 0.0f));
	mat4 mat_coin1_R = identity_mat4();
	mat4 mat_coin1 = identity_mat4();	
	mat4 mat_coin2_T = translate(identity_mat4(), vec3(-0.12f, 0.0f, 0.0f));
	mat4 mat_coin2_R = identity_mat4();
	mat4 mat_coin2 = identity_mat4();



	glUseProgram(pipe_program);
	int pipe_M_loc = glGetUniformLocation(pipe_program, "M");
	int pipe_V_loc = glGetUniformLocation(pipe_program, "V");
	int pipe_P_loc = glGetUniformLocation(pipe_program, "P");
	int pipe_color_factor_loc = glGetUniformLocation(pipe_program, "color_factor");
	int pipe_color_factor2_loc = glGetUniformLocation(pipe_program, "color_factor2");
	mat4 pipe_rotation = identity_mat4();
	glUniformMatrix4fv(pipe_M_loc, 1, GL_FALSE, identity_mat4().m);
	glUniformMatrix4fv(pipe_V_loc, 1, GL_FALSE, camera.view_matrix.m);
	glUniformMatrix4fv(pipe_P_loc, 1, GL_FALSE, camera.perspective_matrix_pipe.m);



	glUseProgram(menus_program);
	int menus_M_loc = glGetUniformLocation(menus_program, "M");
	GLint menus_tex_loc = glGetUniformLocation(menus_program, "menu_texture");
	glUniformMatrix4fv(menus_M_loc, 1, GL_FALSE, identity_mat4().m);



	glUseProgram(score_program);
	int score_M_loc = glGetUniformLocation(score_program, "M");	
	GLint score_tex_loc = glGetUniformLocation(score_program, "score_texture");
	GLint score_digit_loc = glGetUniformLocation(score_program, "ext_score_digit");
	glUniformMatrix4fv(score_M_loc, 1, GL_FALSE, identity_mat4().m);
	


	// ** Rendering

	// enable depth-testing
	glEnable(GL_DEPTH_TEST); 
	// depth-testing will interpret a smaller value as "closer"
	glDepthFunc(GL_LESS); 


	lighting.background_light.v[0] = 0.5;
	lighting.background_light.v[1] = 0.2;

// TODO: add this in constants
	// Control on the background_light. Not used yet.
	glClearColor(lighting.background_light.v[0], lighting.background_light.v[1], lighting.background_light.v[2], 1.0);

	// Timer to control FPS	
	time_t start_render_time;
	int randomise = 1;


	// Rendering loop
	while ( !glfwWindowShouldClose(window) ) 
	{
		start_render_time = clock();	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, gl_width * VIEWPORT_SCALE_FACTOR, gl_height * VIEWPORT_SCALE_FACTOR);
		
		
		// One scene rendering per game state
		if ( GAME_STATE == GAME_STATE_PLAY_LEVEL )
		{

			glClearColor(lighting.background_light.v[0], lighting.background_light.v[1], lighting.background_light.v[2], 1.0);

			/*** GRID  ***/
			glUseProgram(platform_program);
			glBindVertexArray(platform_vao);
			glUniformMatrix4fv(V_loc, 1, GL_FALSE, (camera.view_matrix).m);
			glUniform1f(spec_exp_loc, lighting.specular_exp);
			mat_coin1_R = rotate_z_deg(mat_coin1_R, 0.2 * FRAME_STEP);
			mat_coin2_R = rotate_y_deg(mat_coin2_R, -0.1 * FRAME_STEP);
			
			// Drawing the platforms an coins
			for ( int i = 0; i <= GRID_LIMIT; i++ )
			{
				glUseProgram(platform_program);
				glBindVertexArray(platform_vao);
				glUniform1f(health_loc, grid[i].health); // Send platform health to shader
				glUniformMatrix4fv(M_loc, 1, GL_FALSE, get_platform_position(i).m); // Updating position
				glDrawArraysInstanced(GL_TRIANGLES, 0, platform_vertex_count, 1);
				
				if ( coin_grid[i].exists ) // If a coin exists here
				{
					glUseProgram(coin_program);
					glBindVertexArray(coin_vao);
					glUniformMatrix4fv(coin_V_loc, 1, GL_FALSE, (camera.view_matrix).m);

					/** Creation of a moving 4-cubes structure with hierarchichal animation and Fancy random movements **/

					mat_coins_origin = get_coin_position(i);

					// A bit of random
					if( i %2 == 0 )
					{
						randomise = -1;
					}
					else
					{
						randomise = 1;
					}

					
					// First two cubes
					glUniform1i(coin_color_state_loc, 0); // Two different colours in the shader controled by 0-1 values
					mat_coin1 = mat_coin1_R * mat_coin1_T;
					mat_coin2 = mat_coin2_R * mat_coin2_T;
					glUniformMatrix4fv(coin_M_loc, 1, GL_FALSE, (translate(identity_mat4(), vec3(0.0f, 0.4f, 0.0f)) * mat_coins_origin * rotate_z_deg(identity_mat4(), (double)i*10.0*(double)randomise)* mat_coin1).m);
					glDrawArraysInstanced(GL_TRIANGLES, 0, coin_vertex_count, 1);
					glUniformMatrix4fv(coin_M_loc, 1, GL_FALSE, (translate(identity_mat4(), vec3(0.0f, 0.4f, 0.0f)) * mat_coins_origin * rotate_y_deg(identity_mat4(), (double)i*10.0*(double)randomise * (double) randomise)* mat_coin2).m);
					glDrawArraysInstanced(GL_TRIANGLES, 0, coin_vertex_count, 1);
					glUniform1i(coin_color_state_loc, 1);
					glUniformMatrix4fv(coin_M_loc, 1, GL_FALSE, (translate(identity_mat4(), vec3(0.0f, 0.4f, 0.0f)) * mat_coins_origin * rotate_y_deg(identity_mat4(), (double)i*10.0*(double)randomise) * mat_coin1).m);
					glDrawArraysInstanced(GL_TRIANGLES, 0, coin_vertex_count, 1);
					// Two other cubes
					mat_coin1 = inverse(mat_coin1_R) * inverse(mat_coin1_T);
					mat_coin2 = inverse(mat_coin2_R) * inverse(mat_coin2_T);

					glUniformMatrix4fv(coin_M_loc, 1, GL_FALSE, (translate(identity_mat4(), vec3(0.0f, 0.4f, 0.0f)) * mat_coins_origin * rotate_y_deg(identity_mat4(), (double)i*10.0*(double)randomise) * mat_coin2).m);
					glDrawArraysInstanced(GL_TRIANGLES, 0, coin_vertex_count, 1);
					glUniformMatrix4fv(coin_M_loc, 1, GL_FALSE, identity_mat4().m); // Reinitialization of position for the drawing loop
				}
			}

			/** drawing the player **/
			// a ring composed of a few cubes
			glUseProgram(player_program);
			glBindVertexArray(player_vao);

			player_rotation_matrix = rotate_y_deg(player_rotation_matrix, 1);
			player_rotation_matrix = rotate_z_deg(player_rotation_matrix, 1);
			player_rotation_matrix = rotate_x_deg(player_rotation_matrix, 1);
			player_rotation_matrix2 = rotate_y_deg(player_rotation_matrix2, -1);
			glUniformMatrix4fv(player_V_loc, 1, GL_FALSE, (camera.view_matrix).m);

			for ( int j = 0; j < 3; j++ )
			{
				for ( int i = 0; i < 10; i++ )
				{
					glUniformMatrix4fv(player_M_loc, 1, GL_FALSE, 
						(get_player_position()*player_rotation_matrix2 * rotate_y_deg(identity_mat4(), i * 36) * translate(identity_mat4(), vec3(0.7f, 1.0, 0.2*j))*rotate_x_deg(player_rotation_matrix, 18*i )).m);
					glDrawArraysInstanced(GL_TRIANGLES, 0, player_vertex_count, 1);
				}
			}


			// ** The tunnel/pipe
			// Fancy movements and custom perspective matrix for the pipe

			glUseProgram(pipe_program);
			glBindVertexArray(pipe_vao);
			glUniformMatrix4fv(pipe_P_loc, 1, GL_FALSE, camera.perspective_matrix_pipe.m);
			glUniformMatrix4fv(pipe_V_loc, 1, GL_FALSE, (camera.view_matrix).m);

			mat4 pipe_matrix = get_pipe_position();
			mat4 pipe_rotation = get_pipe_rotation();

			glUniform1i(pipe_color_factor2_loc, GAME_LEVEL); // Color shade control

			for( int i = 0 ; i < 1000 ; i++ )
			{
				glUniform1i(pipe_color_factor_loc, i); // Color shade control
				if( i %2 == 0 )
				{
					glUniformMatrix4fv(pipe_M_loc, 1, GL_FALSE, (translate(rotate_y_deg(pipe_matrix, i * -7) * pipe_rotation, vec3(0.0f, -2.0f*i, 0.0f))).m);
				}
				else
				{
					glUniformMatrix4fv(pipe_M_loc, 1, GL_FALSE, (translate(rotate_y_deg(pipe_matrix, i*5) * inverse(pipe_rotation), vec3(0.0f, -2.0f*i, 0.0f))).m);
				}
				glDrawArraysInstanced(GL_TRIANGLES, 0, pipe_vertex_count, 1);
			}	
		}

		else if ( GAME_STATE == GAME_STATE_START_GAME )
		{ 
			glClearColor(0.7f, 0.7f, 0.7f, 0.7f); 

			
			/**  PIPE  **/
			// A small monochromatic pipe on the start screen
			glUseProgram(pipe_program);
			glBindVertexArray(pipe_vao);
			glUniformMatrix4fv(pipe_P_loc, 1, GL_FALSE, camera.perspective_matrix_pipe.m);
			glUniformMatrix4fv(pipe_V_loc, 1, GL_FALSE, (camera.view_matrix).m);
			mat4 pipe_matrix = get_pipe_position();
			mat4 pipe_rotation = get_pipe_rotation();//rotate_y_deg(pipe_rotation, 0.1);
			glUniform1i(pipe_color_factor_loc, -100);

			for( int i = 0 ; i < 50 ; i++ )
			{				
				if( i %2 == 0 )
				{
					glUniformMatrix4fv(pipe_M_loc, 1, GL_FALSE, (translate(rotate_y_deg(pipe_matrix, i * -7) * pipe_rotation, vec3(0.0f, -2.0f*i, 0.0f))).m);
				}
				else
				{
					glUniformMatrix4fv(pipe_M_loc, 1, GL_FALSE, (translate(rotate_y_deg(pipe_matrix, i*5) * inverse(pipe_rotation), vec3(0.0f, -2.0f*i, 0.0f))).m);
				}
				glDrawArraysInstanced(GL_TRIANGLES, 0, pipe_vertex_count, 1);
			}

			// Drawing the start screen with the right texture
			glUseProgram(menus_program);
			glBindVertexArray(menus_vao);
			glUniform1i(menus_tex_loc, 0); // Send texture "position" in the shader
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
		}


		else if ( GAME_STATE == GAME_STATE_INSTRUCTIONS_SCREEN )
		{ 
			glClearColor(0.7f, 0.7f, 0.7f, 0.7f); 
			// First two cubes
			glUseProgram(coin_program);
			glBindVertexArray(coin_vao);

			mat_coin1_R = rotate_y_deg(mat_coin1_R, 1);
			mat_coin2_R = rotate_y_deg(mat_coin2_R, -1);
			glUniform1i(coin_color_state_loc, 0); // Two different colours in the shader controled by 0-1 values
			glUniformMatrix4fv(coin_M_loc, 1, GL_FALSE, (scale(translate(identity_mat4(), vec3(2.6f, -0.01f, -1.5f)), vec3(30.0f, 30.0f, 30.0f))*rotate_y_deg(identity_mat4(),10)*rotate_z_deg(identity_mat4(), 15)).m);
			glDrawArraysInstanced(GL_TRIANGLES, 0, coin_vertex_count, 1);	
			glUniformMatrix4fv(coin_M_loc, 1, GL_FALSE, (scale(translate(identity_mat4(), vec3(2.55f, 0.02f, -1.55)), vec3(30.0f, 30.0f, 30.0f))*rotate_y_deg(identity_mat4(),10)*rotate_z_deg(identity_mat4(), 15)).m);
			glDrawArraysInstanced(GL_TRIANGLES, 0, coin_vertex_count, 1);	

			glUniform1i(coin_color_state_loc, 1);
			glUniformMatrix4fv(coin_M_loc, 1, GL_FALSE, (scale(translate(identity_mat4(), vec3(2.5f, -0.01f, -1.5f)), vec3(30.0f, 30.0f, 30.0f))*rotate_y_deg(identity_mat4(),10)*rotate_z_deg(identity_mat4(), 15)).m);
			glDrawArraysInstanced(GL_TRIANGLES, 0, coin_vertex_count, 1);
			glUniformMatrix4fv(coin_M_loc, 1, GL_FALSE, (scale(translate(identity_mat4(), vec3(2.7f, 0.0f, -1.55f)), vec3(30.0f, 30.0f, 30.0f))*rotate_y_deg(identity_mat4(),10)*rotate_z_deg(identity_mat4(), 15)).m);
			glDrawArraysInstanced(GL_TRIANGLES, 0, coin_vertex_count, 1);

			glUseProgram(menus_program);
			glBindVertexArray(menus_vao);
			glUniform1i(menus_tex_loc, 0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
		}

		else if ( GAME_STATE == GAME_STATE_END_LEVEL )
		{ 
			glClearColor( 0.6, 0.01 * score_number_value, 0.1f, 1.0); // Background colour linked to the player score

			glUseProgram(score_program);
			glBindVertexArray(score_vao);
			glBindVertexArray(score_vao);
			glUniform1i(score_tex_loc, 3);

//TODO: thid is weird
			if ( score_number_value < level_score ) 
			{
				// two more percents per frame.
				score_number_value++; 
			}
			if ( score_number_value < level_score) 
			{
				score_number_value++; 
			}

			// Sent digit to the shader, translate the mesh at the right place, and draw it.
			glUniform1i(score_digit_loc, get_digit(score_number_value, 0));
			glUniformMatrix4fv(score_M_loc, 1, GL_FALSE, translate(identity_mat4(), vec3(0.4f, 0.0f, 0.0f)).m);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

			glUniform1i(score_digit_loc, get_digit(score_number_value, 1));
			glUniformMatrix4fv(score_M_loc, 1, GL_FALSE, translate(identity_mat4(), vec3(0.0f, 0.0f, 0.0f)).m);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

			glUniform1i(score_digit_loc, get_digit(score_number_value, 2));
			glUniformMatrix4fv(score_M_loc, 1, GL_FALSE, translate(identity_mat4(), vec3(-0.4f, 0.0f, 0.0f)).m);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

			glUseProgram(menus_program);
			glBindVertexArray(menus_vao);
			glUniform1i(menus_tex_loc, 1);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

		}

		else if ( GAME_STATE == GAME_STATE_END_GAME ) // almost the same as end_level
		{
			glClearColor( 0.6, 0.01 * score_number_value, 0.1f, 1.0); 
			glUseProgram(score_program);
			glBindVertexArray(score_vao);
			glBindVertexArray(score_vao);
			glUniform1i(score_tex_loc, 3);


			if ( score_number_value < (int)(player_total_score+0.5))
			{
				score_number_value+= 1;
			} 
			if ( score_number_value < (int)(player_total_score + 0.5) )
			{
				score_number_value += 1;
			} 

			glUniform1i(score_digit_loc, get_digit(score_number_value, 0));
			glUniformMatrix4fv(score_M_loc, 1, GL_FALSE, translate(identity_mat4(), vec3(0.4f, 0.0f, 0.0f)).m);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

			glUniform1i(score_digit_loc, get_digit(score_number_value, 1));
			glUniformMatrix4fv(score_M_loc, 1, GL_FALSE, translate(identity_mat4(), vec3(0.0f, 0.0f, 0.0f)).m);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

			glUniform1i(score_digit_loc, get_digit(score_number_value, 2));
			glUniformMatrix4fv(score_M_loc, 1, GL_FALSE, translate(identity_mat4(), vec3(-0.4f, 0.0f, 0.0f)).m);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

			glUseProgram(menus_program);
			glBindVertexArray(menus_vao);
			glUniform1i(menus_tex_loc, 2);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
		}
		else 
		{}

	time_t end_render_time = clock();

		//FPS limit.
	if ( (double) end_render_time < (double) start_render_time + (double) FRAME_STEP  )
	{
		sleep( ( (double) start_render_time + (double)FRAME_STEP ) - end_render_time  );
	}

	// this just updates window events and keyboard input events (not used yet)
	glfwPollEvents();
	glfwSwapBuffers(window);

	// GAME ENGINE : update all elements and handle keyboard control
	game_step();

	for(GLenum err; (err = glGetError()) != GL_NO_ERROR;)
	{
		 //Process/log potential errors
		printf("error occured : %i", err);
	}
}

return 0;
}