#version 410

in vec3 n_eye; // normals from vertex shader
in vec3 p_eye;
out vec4 frag_colour;

uniform int ext_color_state; // Two different color states for the coins
uniform int color_factor, color_factor2;
vec3 l_dir = normalize (vec3 (0.0f, 0.0f, -0.1f));

// Light
vec3 l_a = vec3 (0.6, 0.6, 0.6); // ambiant color
vec3 l_d = vec3 (0.8, 0.8, 0.8); // Diffuse color
vec3 l_s = vec3 (1.0, 1.0, 1.0);// Specular color


float spec_exp = 20.0f;
vec3 k_a = vec3 (1.0, 0.5 , 0.0); // ambient
vec3 k_d = vec3 (1.0, 0.5 , 0.0); // diff
vec3 k_s = vec3 (0.2, 0.1 , 0.0); // spec

void main () {


	if(ext_color_state == 0) // Color 1
	{
		k_a = vec3 (0.3, 0.4, 0.8); // ambient
		k_d = vec3 (0.3, 0.4, 0.8); // diff
		k_s = vec3 (0.3, 0.4, 0.8); // spec
	}
	if(ext_color_state == 1) // Color 2
	{
		k_a = vec3 (0.7, 0.2, 0.1); // ambient
		k_d = vec3 (0.7, 0.2, 0.1); // diff
		k_s = vec3 (0.7, 0.2, 0.1); // spec
	}




	vec3 n = normalize (n_eye);
	
	// Ambient equation
	vec3 I = vec3 (0.0, 0.0, 0.0);
	I += l_a * k_a;

	// Diffuse equation
	I += l_d * k_d * max (dot (-l_dir, n), 0.0);

	// Specular equation : blinn phong
	vec3 r = reflect (l_dir, n);
	vec3 v = normalize (-p_eye);
	vec3 h = normalize (v + -l_dir); 
	
	I += l_s * k_s * pow (max (dot (h, n), 0.0), spec_exp);
	
	frag_colour = vec4 (I, 1.0);
}
