#version 410

in vec2 texture_coordinates; 	// From the vertex shader
out vec4 frag_colour;			// the first output variable is used as the fragment colour

uniform sampler2D menu_texture;

void main () 
{
	vec4 texel = texture (menu_texture, texture_coordinates);
	frag_colour = texel; 
}
