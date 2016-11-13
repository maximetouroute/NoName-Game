# NoName Game - A falling platforms Game

Catch all the coins, move fast, and don't fall !

This 3D game was made as part of a Computer Graphics course focused on low-level OpenGL4 development. It's developped in pure OpenGL4 with GLSL shaders, no framework, no engine, no nothing.


# How does it look

## The start screen

![](https://github.com/maximetouroute/NoName-Game/blob/master/readme_imgs/startgame.jpg)

## In game
You're the light pink ring. You move accros the platforms trying to catch those red/blue coins. Platforms shake and fall behind you. If you move too slowly, you'll be blocked on a platform surrounded by void and be forced to jump through the infinite tunnel to end your quest.

![](https://github.com/maximetouroute/NoName-Game/blob/master/readme_imgs/ingame_1.jpg)
![](https://github.com/maximetouroute/NoName-Game/blob/master/readme_imgs/ingame_2.jpg)
![](https://github.com/maximetouroute/NoName-Game/blob/master/readme_imgs/ingame_3.jpg)

## The score screen 

It tells you your score after each level :
![](https://github.com/maximetouroute/NoName-Game/blob/master/readme_imgs/score_1.jpg)
![](https://github.com/maximetouroute/NoName-Game/blob/master/readme_imgs/score_2.jpg)
![](https://github.com/maximetouroute/NoName-Game/blob/master/readme_imgs/score_3.jpg)


# How does it work

As I mentionned it, this game is developped in pure OpenGL4. Each game element has its own GLSL program :
* the player
* the coins
* the tunnel
* the menu screens
* the score 

Effects such as color background and score digits on the score screen, color gradient on the infinite tunnel, or platform color are all controlled by GLSL shader programs.

Game levels are actually parsed from text files, so you can easily create your own levels.

The Level 2, as seen on above screenshots, looks like this : 
```
C C C C C C C
C X X O X X C
C X X O X X C
C C C D C C C
C X X O X X C
C X X O X X C
C C C C C C C
```
X is void, C is platform with coin, O is empty platform, and D is your starting point


# How to play

First, you'll need an openGL4 compliant computer and OS. Then, execute the script in the root folder
```
./run.sh
```


# Used technologies & libraries
* OpenGL4
* glfw3
* glew
* a few libraries made by Anton Gerdelan http://antongerdelan.net/
* the stb_image library to load TGA textures https://github.com/nothings/stb
