#include <time.h>
#include "math_funcs.h"
#include "constants.h"
#include <stdlib.h>
#include <cmath>

#define Platform_BORDER 2.2

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

#define DEATH_TIME 1000

#define CAMERA_PRECISION_STEP (double) 0.01
#define CAMERA_NORMAL_HEIGHT 11
#define INITIAL_CAMERA_HEIGHT 100

#define MAX_LOOKING_DOWN -100
#define MAX_LOOKING_UP 5

#define PLATFORM_SHAKING_STRENGTH 5
#define PLATFORM_FALLING_STEP 2

#define COIN_HEIGHT 0.3

#define FINAL_COIN_SCALE 0.4
#define GRID_SIZE 7
#define GRID_LIMIT (  ((int)GRID_SIZE*(int)GRID_SIZE) - 1)


int level_coin_number_left;
int level_coin_number_total;
int GAME_LEVEL;

double perspective_pipe_factor = 0;
double randomiser_2 = 1;


// Headers
void init_grid();
void shake_platform(int indice, double strengh);
void killing_process(int indice);
void kill_platform(int indice);
void move_player(int n);
void coin_movement(int indice);
mat4 get_platform_position(int indice);
void update_grid();
void init_player();
mat4 get_player_position();
void player_movement();
void update_player();
mat4 get_coin_position(int indice);
mat4 get_pipe_position();
mat4 get_pipe_rotation();
void init_lighting();
void update_lighting();
void init_camera();
void camera_movement();
void update_camera();

// ** Structs for the game elements

// Game Element structures
struct Platform 
{
	mat4 position_matrix;
	bool alive;
	double health;
	time_t start_death;
	mat4 shaking_matrix;
	int shaking_value;
};

struct Player 
{
	int position_player;
	mat4 position_matrix;
	vec3 heading_to_vector;
	mat4 rotation_matrix;
	bool alive;
};

struct Coin 
{
	bool exists;
	bool taken;	
	time_t start_catch;
	mat4 position_matrix;
	mat4 rotation_matrix;
	// Matrix used when the coin is catched
	mat4 offset_matrix;
};

struct Pipe 
{
	mat4 position_matrix;
	mat4 rotation_matrix;
};

struct Camera 
{
	mat4 view_matrix, perspective_matrix, perspective_matrix_pipe;
	vec3 heading_to;
	vec3 eye, look, up;

	double rotation_angle = 0.0;
	double look_height = -100.0;
	double camera_height = 20; 
	double ypos = WINDOW_HEIGHT / 2.0;
	double xpos = WINDOW_WIDTH / 2.0;
};

struct Lighting
{
	float specular_exp;	
	vec3 background_light;
};


// Game Elements instances
Platform grid[(int)GRID_SIZE*(int)GRID_SIZE];
Coin coin_grid [(int)GRID_SIZE*(int)GRID_SIZE];
Player player;
Lighting lighting;
Pipe pipe_data = { identity_mat4(), identity_mat4() };
Camera camera;


//** GRID & COINS


//Init platforms and coins
void init_grid()
{
	// Load game levels from text files
	FILE *file;
	char c;
	int n = 0;
	level_coin_number_left = level_coin_number_total = 0;
	
	char filename[256] = "./Levels/Level-";
	char level = (char)(((int)'0') + GAME_LEVEL);
	filename[15] = level;
	filename[16] = '\0';

	file = fopen(filename, "r");

	if ( file == NULL )
	{
		perror("Error reading level file");
	}
	else
	{
		//Setting up the starting point to draw the grid
		float start_position_x = 0.0 - (float)Platform_BORDER * (((int)GRID_SIZE - 1) / 2);
		float start_position_z = 0.0 - (float)Platform_BORDER * (((int)GRID_SIZE - 1) / 2);
		mat4 start_point_position_matrix = translate(identity_mat4(), vec3(start_position_x, 0.0f, start_position_z));

		//Variables updated
		// Reading the first player before going to the loop
		c = getc(file);

		for ( int i = 0; i < (int)GRID_SIZE; i++, c = getc(file) ) //Columns
		{
			for ( int j = 0; j < (int)GRID_SIZE; j++, c = getc(file) ) //Rows
			{
				grid[(int)GRID_SIZE*i + j] = 
				{ translate(start_point_position_matrix, 
					vec3((float)Platform_BORDER*j,
						0.0f, 
						(float)Platform_BORDER*i)),
				true,
				1.0, 0, 
				identity_mat4(),
				(int)PLATFORM_SHAKING_STRENGTH };
				//By default, there is no coin
				coin_grid[(int)GRID_SIZE*i + j] = { 
					false,
					false,
					0,
					translate(grid[(int)GRID_SIZE*i + j].position_matrix,
						vec3(0.0f,
							(float)COIN_HEIGHT, 0.0f)),
					identity_mat4() 
				};

				if ( c == 'X' ) // Dead Platform
				{
					//printf("%i;%i DEAD \n", i, j);
					grid[(int)GRID_SIZE*i + j].alive = false;
					grid[(int)GRID_SIZE*i + j].health = 0.0;
					grid[(int)GRID_SIZE*i + j].position_matrix.m[13] = -400.0; // Already far away.
				}
				else if ( c == 'C' ) // Platform with coin
				{
					//printf("%i;%i COIN \n", i, j);
					coin_grid[(int)GRID_SIZE*i + j].exists = true;
					//Coin number updated
					level_coin_number_left++;
					level_coin_number_total++;
				}
				else if ( c == 'D' ) // Start point Platform.
				{
					//printf("%i;%i PLAYER \n", i, j);
					player.position_player = (int)GRID_SIZE*i + j;
				}
				else // Normal platform. Alive. No coins.
				{
					//printf("%i;%i Normal platform : %c\n", i, j, c);
				}	
			}
		}
		fclose(file);
	}
}

// Handle platform shaking before platform death
void shake_platform(int indice, double strengh)
{
	grid[indice].shaking_matrix = rotate_x_deg(identity_mat4(), 5 * strengh * grid[indice].shaking_value);
	grid[indice].shaking_matrix = rotate_y_deg(identity_mat4(), 5 * strengh * grid[indice].shaking_value);
	grid[indice].shaking_matrix = rotate_z_deg(identity_mat4(), 5 * strengh * grid[indice].shaking_value);
	grid[indice].shaking_value *= -1; // Reversing the rotation each frame.
}


// Killing process of a platform
void killing_process(int indice)
{
	// If < to DEATH_TIME. Shake it and change its health (change colour)
	if ( ((double)clock() - (double)grid[indice].start_death) < (double)DEATH_TIME )
	{
		//Changement de couleur
		shake_platform(indice, (((double)clock() - (double)grid[indice].start_death) / (double)DEATH_TIME));
		grid[indice].health = 1.0 - (((double)clock() - (double)grid[indice].start_death) / (double)DEATH_TIME);
	}
	else // Kill it.
	{
		grid[indice].health = 0; // Health 
		grid[indice].alive = false;
	}
}

// Start the killing process of a platform
void kill_platform(int indice)
{
	if ( grid[indice].start_death == 0 ) // killing process starts only if there is no killing in process already
	{
		if ( grid[indice].alive ) // If Platform still alive
		{
			grid[indice].start_death = clock();
			killing_process(indice);
		}
		else
		{
			//If platform dead by default, arrives here.
			//printf("ERROR: Platform %f already dead\n", indice);
		}
	}
}

/*
Handles player movement via position_player variable
Pre-processor defined movement constant values 0 1 2 3 Left Right Up Down
*/
void move_player(int n)
{
	// Move only if the player is still alive, if the movement is possible, and if the platform is not falling yet
	if ( grid[player.position_player].alive )
	{
		if ( n == LEFT && player.position_player > 0 && (player.position_player % (int)GRID_SIZE != 0) )
		{
			kill_platform(player.position_player);
			player.position_player--;
		}
		if ( n == RIGHT && player.position_player < (int)GRID_LIMIT && ((player.position_player + 1) % (int)GRID_SIZE) != 0 )
		{
			kill_platform(player.position_player);
			player.position_player++;
		}
		if ( n == UP && (player.position_player - (int)GRID_SIZE) >= 0 )
		{
			kill_platform(player.position_player);
			player.position_player -= (int)GRID_SIZE;
		}
		if ( n == DOWN && (player.position_player + (int)GRID_SIZE) <= (int)GRID_LIMIT )
		{
			kill_platform(player.position_player);
			player.position_player += (int)GRID_SIZE;
		}
		else
		{
			//printf("ERROR Charactor movement. position_player : %i\n", position_player);
		}
	}
	else
	{
		//printf("LOCKED Movement. position_player : %i\n", position_player);
	}
}

// Start the catching process of a coin
void coin_movement(int indice)
{
	// if coin is not taken, nothing new.
	if ( !coin_grid[indice].taken && coin_grid[indice].start_catch == 0 ) 
	{
		coin_grid[indice].position_matrix = grid[indice].position_matrix;
	}
	
	// If coin has just been taken
	else if ( coin_grid[indice].taken && coin_grid[indice].start_catch == 0 ) 
	// else if coin has just been taken
	{
		// A bit of random
		if (indice % 2 == 0)
		{
			randomiser_2 = -1;
		}	
		else
		{
			randomiser_2 = 1;
		}

		coin_grid[indice].start_catch = clock();
		
		// Init of the offset matrix to create the tornado
		coin_grid[indice].offset_matrix = translate(identity_mat4(), vec3((double)indice*randomiser_2 / ((double)GRID_LIMIT*0.4), (double) indice /((double)GRID_SIZE*30.0), -randomiser_2*(double)indice / ((double)GRID_LIMIT*0.4)));
		
		// Brutal move of coin to create a long and smooth approach of the player
		// Scaling down the coin to improve visibility
		coin_grid[indice].position_matrix = scale(coin_grid[indice].position_matrix, vec3((float)FINAL_COIN_SCALE, (float)FINAL_COIN_SCALE, (float)FINAL_COIN_SCALE));
		coin_grid[indice].position_matrix = translate(coin_grid[indice].position_matrix, vec3((double)indice / 3.0, (double)indice / 10.0, (double)indice / 3.0));
	}

	// If coin has been catched already
	else if ( coin_grid[indice].taken && coin_grid[indice].start_catch != 0 ) 
	{
		// Coin heading to the player
		// Y movement locked.
		{
			double direction_x = player.position_matrix.m[12] - coin_grid[indice].position_matrix.m[12];
			//double direction_y = player.position_matrix.m[13] - coin_grid[indice].position_matrix.m[13];
			double direction_z = player.position_matrix.m[14] - coin_grid[indice].position_matrix.m[14];

			coin_grid[indice].position_matrix.m[12] = coin_grid[indice].position_matrix.m[12] + (double)(5 / (double)FPS) *direction_x;
			//coin_grid[indice].position_matrix.m[13] = coin_grid[indice].position_matrix.m[13] + (double)(5 / (double)FPS) *direction_y;
			coin_grid[indice].position_matrix.m[14] = coin_grid[indice].position_matrix.m[14] + (double)(5 / (double)FPS) *direction_z;
			
			// Rotating the whole thing -> Tornado effect !
			coin_grid[indice].rotation_matrix = rotate_x_deg(coin_grid[indice].rotation_matrix,( 0.2*(level_coin_number_total) / (level_coin_number_left-0.2)));
			coin_grid[indice].rotation_matrix = rotate_y_deg(coin_grid[indice].rotation_matrix, ((level_coin_number_total) / (level_coin_number_left-0.2)));
			coin_grid[indice].rotation_matrix = rotate_z_deg(coin_grid[indice].rotation_matrix, (0.2*(level_coin_number_total) / (level_coin_number_left-0.2)));
		}
	}
}

mat4 get_platform_position(int indice)
{
	return grid[indice].position_matrix * grid[indice].shaking_matrix;
}

// Update grid elements - coins and platforms
void update_grid()
{
	// For each platform
	for ( int n = 0; n <= (int)GRID_LIMIT; n++ )
	{
		if ( grid[n].start_death > 0 && grid[n].alive )
		{
			killing_process(n); // If killing has started and not over yet, we continue for this frame
		}
		else if ( !grid[n].alive ) // if platform dead, platform falls.
		{ 
			grid[n].position_matrix = translate(grid[n].position_matrix, vec3(0.0f, -(float)PLATFORM_FALLING_STEP, 0.0f)); 
			grid[n].shaking_matrix = rotate_y_deg(grid[n].shaking_matrix, 1 + 1 * n);; //* rotate_x_deg(grid[n].shaking_matrix, 5); // shakking_matrix reused
		} 

		// If the coin of this platform exists
		if( coin_grid[n].exists )
		{
			//if coin existing on this platform, and player on the platform
			if ( (n == player.position_player && !coin_grid[n].taken) )
			{
				coin_grid[n].taken = true;
				level_coin_number_left--;
			}

			coin_movement(n);
		}		
	}

	if ( level_coin_number_left == 0 ) // If no coins left - KILL EVERYONE ! (to end the level)
	{
		for ( int n = 0; n <= (int)GRID_LIMIT; n++ ) 
		{
			kill_platform(n);
		}
	}
}

//** PLAYER

void init_player()
{
	player = { 
		(int)((int)GRID_LIMIT / 2),
		identity_mat4(),
		vec3(0.0f, 0.0f, 0.0f),
		identity_mat4(),
		true 
	};
}

mat4 get_player_position()
{
	return player.position_matrix * player.rotation_matrix;
}

// Makes the player move smoothly
void player_movement()
{
	
	double direction_x = player.heading_to_vector.v[0] - player.position_matrix.m[12];
	double direction_y = player.heading_to_vector.v[1] - player.position_matrix.m[13];
	double direction_z = player.heading_to_vector.v[2] - player.position_matrix.m[14];

	if ( abs(direction_x) > (double)CAMERA_PRECISION_STEP || abs(direction_z) > (double)CAMERA_PRECISION_STEP || abs(direction_y) > (double)CAMERA_PRECISION_STEP )
	{
		player.position_matrix.m[12] = player.position_matrix.m[12] + (double)(10 / (double)FPS) * direction_x;
		player.position_matrix.m[13] = player.position_matrix.m[13] + (double)(2 / (double)FPS) * direction_y;
		player.position_matrix.m[14] = player.position_matrix.m[14] + (double)(10 / (double)FPS) * direction_z;
	}
}

// Update the player position
void update_player()
{
	mat4 temp = get_platform_position(player.position_player);
	player.heading_to_vector = vec3(temp.m[12], temp.m[13], temp.m[14]); // x y z .

	player_movement();	

	// If the player is falling
	if ( player.heading_to_vector.v[1] < 0 )
	{
		//printf("player dead\n");
		player.alive = false;
		player.rotation_matrix = rotate_y_deg(player.rotation_matrix, -5); // rotates during the free fall.
	}
}

// ** COINS

mat4 get_coin_position(int indice)
{

	if ( coin_grid[indice].taken )
	{
		return coin_grid[indice].position_matrix * coin_grid[indice].rotation_matrix * coin_grid[indice].offset_matrix;
	}
	//If not taken, no rotation.
	else
	{
		return coin_grid[indice].position_matrix;// *coin_grid[indice].rotation_matrix;
	}
}

// ** PIPE

mat4 get_pipe_position()
{
	//pipe.rotation_matrix = rotate_y_deg(pipe.rotation_matrix, 0.1* ((level_coin_number_total) / (level_coin_number_left + 1)));
	pipe_data.position_matrix = get_player_position();
	pipe_data.position_matrix.m[13] = 0; // The pipe is following the player, but doesn't fall with him.
	return pipe_data.position_matrix;
}

mat4 get_pipe_rotation()
{
	pipe_data.rotation_matrix = rotate_y_deg(pipe_data.rotation_matrix, 0.1* ((level_coin_number_total) / (level_coin_number_left + 1.01)));
	return pipe_data.rotation_matrix;
}


// ** LIGHTING
#define SPEC_FINAL_VALUE 200 // Normal level of specular light

void init_lighting()
{
	lighting = { (double)SPEC_FINAL_VALUE, vec3(0.0f, 0.0f, 0.0f) };
}

void update_lighting()
{
	double step = ((double)level_coin_number_left) / (double) (level_coin_number_total);
}

// ** CAMERA

void init_camera()
{
	camera.camera_height = (double) INITIAL_CAMERA_HEIGHT;
	camera.perspective_matrix = perspective(90.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1, 200.0);
	camera.eye = { 0.0f, 0.0f, 0.0f };
	camera.look = { 1.0, 0.0f, 0.0f }; // initial : the look point is in front of the camera.
	camera.up = { 0.0, 0.0, -1.0 };  // Up vector to look straight.
	camera.view_matrix = look_at(camera.eye, camera.look, camera.up);
}

void camera_movement()
{
	// Smooth camera movement heading to the player position
	{
		double direction_x = camera.heading_to.v[0] - camera.eye.v[0];
		double direction_y = camera.heading_to.v[1]+CAMERA_NORMAL_HEIGHT - camera.eye.v[1];// The camera go back to normal height if no movement
		double direction_z = camera.heading_to.v[2] - camera.eye.v[2];

		if ( abs(direction_x) > (double)CAMERA_PRECISION_STEP || abs(direction_z) > (double)CAMERA_PRECISION_STEP || abs(direction_y) > (double) CAMERA_PRECISION_STEP )
		{
			camera.eye.v[0] = camera.eye.v[0] + (double)(3 / (double)FPS) *direction_x;
			camera.camera_height = camera.camera_height + (double)(3 / (double)FPS) *direction_y;
			camera.eye.v[2] = camera.eye.v[2] + (double)(3 / (double)FPS) *direction_z;	

			//perspective_matrix = perspective(90.0*(1+direction_z*100), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1, 200.0);
			//if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) perspective_matrix = perspective(120.0, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1, 200.0);
			if ( player.position_matrix.m[13] < 0 )
			{
				perspective_pipe_factor -= 0.1;
				if ( perspective_pipe_factor <= -4.5 ) 
				{
					perspective_pipe_factor = -4.5;
				}
			}
			if ( player.position_matrix.m[13] == 0 )
			{
				perspective_pipe_factor = 0;
			}

			camera.perspective_matrix_pipe = perspective(30.0*(1 - perspective_pipe_factor), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1, 200.0);
			
		}
		// If camera went down but has to go back up
		else  
		{
			//camera_height += (double)(1 / (double)FPS) * (double)CAMERA_NORMAL_HEIGHT;
		}
	}

	// Updating vectors and matrices 
	// Code from my previous assignment on virtual camera

	camera.look = camera.eye.operator+(heading_to_direction(camera.rotation_angle));
	
	if ( camera.look_height < (int)MAX_LOOKING_DOWN ) 
	{
		camera.look_height = (int)MAX_LOOKING_DOWN;
	}
	if ( camera.look_height > (int) MAX_LOOKING_UP )
	{
		camera.look_height = (int)MAX_LOOKING_UP;
	}

	// Updating camera position
	camera.look.v[1] = camera.camera_height +camera.look_height;
	camera.eye.v[1] = camera.camera_height;
	camera.view_matrix = look_at(camera.eye, camera.look, camera.up);
}

void update_camera()
{
	mat4 temp = player.position_matrix;
	camera.heading_to = vec3(temp.m[12], temp.m[13], temp.m[14]); // x y z .
	//printf("%f,%f,%f\n", temp.m[12], temp.m[13], temp.m[14]);
	camera_movement();
}