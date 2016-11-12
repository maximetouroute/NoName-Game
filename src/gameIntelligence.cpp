#define _CRT_SECURE_NO_WARNINGS
#include "gameElements.cpp"

#ifdef __APPLE__
        #include <sys/uio.h>
#else
        #include <sys/io.h>
#endif
#include <unistd.h>

#define GAME_STATE_START_GAME 0
#define GAME_STATE_INSTRUCTIONS_SCREEN 1
#define GAME_STATE_PLAY_LEVEL 2
#define GAME_STATE_END_LEVEL 3
#define GAME_STATE_END_GAME 4

#define TIME_END_LEVEL 1800


int GAME_STATE;
time_t level_end_clock;

double player_total_score;
int total_coins;
int total_left_coins;
double level_score;
int score_number_value = 0;

/* Variables for keyboard lock */
#define TIME_KEYBOARD_LOCK  100 // Keyboard duration lock after a key is pressed.

time_t keyboard_start_lock; // Has to be unique for each platform
bool keyboard_lock = false; // true : key pressed, false: key released

void init_level();
void lock_keyboard();
void keyboard_control();
void init_game();
void end_level();
void game_step();



void init_level()
{
	char filename[256] = "./Levels/Level-";
	char level = (char)(((int)'0') + GAME_LEVEL);
	filename[15] = level;
	filename[16] = '\0';

	if (  access(filename, 0) != -1 )  // if the file exists
	{
		score_number_value = 0;
		GAME_STATE = (int)GAME_STATE_PLAY_LEVEL;
		init_camera(); // in first to avoid a glitch with the character
		init_lighting();
		init_player(); // CHARACTER BEFORE GRID ! à cause de la lecture de fichier
		init_grid();

		player.position_matrix = translate(identity_mat4(), vec3(0.0f, 0.0f, 0.0f));
		update_camera();
	}
	else  // No more levels, end of game.
	{
		score_number_value = 0;
		player_total_score = 100*( total_coins - total_left_coins)/ total_coins;
		GAME_STATE = GAME_STATE_END_GAME;
	}
}


/*
Locks the keyboard for an amount of time.
*/
void lock_keyboard()
{
	keyboard_start_lock = clock();
	keyboard_lock = true;
}

/*
Handle keyboard controls
And update the keyboard lock status
*/
void keyboard_control()
{
	// If in the middle of a play: handle player movements
	if (  GAME_STATE == GAME_STATE_PLAY_LEVEL )
	{
		if ( keyboard_lock == false )
		{
			if ( glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS )
			{
				move_player(LEFT);
				lock_keyboard();
			}

			else if ( glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS )
			{
				move_player(RIGHT);
				lock_keyboard();
			}

			else if ( glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS )
			{
				move_player(UP);
				lock_keyboard();
			}
			else if ( glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS )
			{
				move_player(DOWN);
				lock_keyboard();
			}
		}

		// Update and unlock keyboard control
		else if ( (double)clock() - keyboard_start_lock > TIME_KEYBOARD_LOCK )
		{
			keyboard_lock = false;
		}
	}

	else if ( GAME_STATE == GAME_STATE_START_GAME )
	{
		if ( glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS ) GAME_STATE = GAME_STATE_INSTRUCTIONS_SCREEN;
	}
	else if ( GAME_STATE == GAME_STATE_INSTRUCTIONS_SCREEN )
	{
		if ( glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ) init_level();
	}
	else if ( GAME_STATE == GAME_STATE_END_LEVEL)
	{
		if ( glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS ) init_level();
	}
	else if ( GAME_STATE == GAME_STATE_END_GAME )
	{
		if ( glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS )
		{
			// Start over the game
			GAME_LEVEL = 1;
			player_total_score = 0;
			total_coins = 0;
			total_left_coins = 0;
			init_level();
		}
	}
}


void init_game()
{
	GAME_LEVEL = 1;
	init_level(); // Init elements and camera. We need the camera matrices at this point.
	GAME_STATE = GAME_STATE_START_GAME;
}

void end_level()
{
	// If mevel just ended: update score
	if ( level_end_clock == 0 ) 
	{
		level_score = 100 * (level_coin_number_total - level_coin_number_left) / level_coin_number_total;

		total_coins += level_coin_number_total;
		total_left_coins += level_coin_number_left;

		level_end_clock = clock();
	}
		
	// Then leaves the player enjoying a free fall
	else if ( clock() - level_end_clock < TIME_END_LEVEL )
	{
		// Free fall.
	}

	// FInally goes to the score screen and get ready for next level
	else 
	{
		level_end_clock = 0;
		GAME_LEVEL++;
		GAME_STATE = GAME_STATE_END_LEVEL;
		//if the file level +1 exist, then start a new level otherwise -> END_GAME.
		//init_game();
	}
}

/*
 Called at each frame to update the whole game
*/
void game_step()
{
	if ( GAME_STATE == (int)GAME_STATE_PLAY_LEVEL )
	{
		keyboard_control(); // Order matters
		update_grid();
		update_player();
		update_lighting();
		update_camera();

		if ( !player.alive ) // If character died
		{
			end_level();
		}
	}
	else if ( GAME_STATE == GAME_STATE_START_GAME 
		|| GAME_STATE == GAME_STATE_END_LEVEL 
		|| GAME_STATE == GAME_STATE_END_GAME 
		|| GAME_STATE == GAME_STATE_INSTRUCTIONS_SCREEN )
	{
		keyboard_control(); 
	}
}