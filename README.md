# Falling Platforms Game

Catch all the coins, move fast, and don't fall !

This 3D game was made as part of a Computer Graphics course focused on low-level OpenGL4 development. It is developped in pure OpenGL4 with GLSL shaders, no framework, no engine, no nothing.


# How does it look

The start screen

![](https://github.com/maximetouroute/Video-Stabilisation-For-Soccer-Game/blob/master/img/backproj_full_B.png)


In game

![](https://github.com/maximetouroute/Video-Stabilisation-For-Soccer-Game/blob/master/img/backproj_full_B.png)
![](https://github.com/maximetouroute/Video-Stabilisation-For-Soccer-Game/blob/master/img/backproj_full_B.png)
![](https://github.com/maximetouroute/Video-Stabilisation-For-Soccer-Game/blob/master/img/backproj_full_B.png)

The score panel

![](https://github.com/maximetouroute/Video-Stabilisation-For-Soccer-Game/blob/master/img/backproj_full_B.png)
![](https://github.com/maximetouroute/Video-Stabilisation-For-Soccer-Game/blob/master/img/backproj_full_B.png)
![](https://github.com/maximetouroute/Video-Stabilisation-For-Soccer-Game/blob/master/img/backproj_full_B.png)


# How does it work

As I mentionned it, this game is developped in pure OpenGL4. Each game element has its own GLSL program :
* the player
* the coins
* the tunnel
* the menu screens
* the score 


Game levels are actually parsed from text files, so you can easily create your own levels
Level 2, as seen on above screenshots : 
```
C C C C C C C
C X X O X X C
C X X O X X C
C C C D C C C
C X X O X X C
C X X O X X C
C C C C C C C
```


# How to play
First, you'll need an openGL4 compliant computer and OS. Then, execute the script in the root folder

```
./run.sh
```
Enjoy !




# Used technologies & libraries
* OpenGL4
* glfw3
* glew
* a few libraries made by Anton Gerdelan http://antongerdelan.net/
* the stb_image library to load TGA textures https://github.com/nothings/stb




