#version 410

in vec2 texture_coordinates; // From the vertex shader
out vec4 frag_colour;		// the first output variable is used as the fragment colour

uniform sampler2D score_texture;

uniform int ext_score_digit; // digit to draw on screen

void main () 
{
	// An offset is applied on the X axis of the texture coordinate depending on the digit wanted.
	vec4 texel = texture (score_texture, vec2(texture_coordinates.x+ ext_score_digit*0.1, texture_coordinates.y ) ); 
	frag_colour = texel;
}
