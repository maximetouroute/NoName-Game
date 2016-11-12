#version 410

layout(location = 0) in vec3 vp;	// our vertex position input variable, which we set up the data for
layout(location = 1) in vec2 vt;	// per vertex texture coordinates;

out vec2 texture_coordinates; 		//Output coordinates ; will be interpolated for each fragment.
uniform mat4 M;

void main () 
{
	texture_coordinates = vt ; // Initialisation of color
 	gl_Position = M * vec4(vp, 1.0); // the built-in vec4 gl_Position variable must be set in a vertex shader
}