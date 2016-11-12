#version 410

in vec3 n_eye; // normals from vertex shader
in vec3 p_eye;

out vec4 frag_colour;
vec3 l_dir = normalize (vec3 (0.0f, 0.0f, 0.0f));

// Color of the light : constant dimmed light for the background.
// Phung lighting is a bit useless here but I prefer to keep it, so further complex color modifications stay possible.

vec3 l_a = vec3 (0.2, 0.2, 0.2); // ambiant color
vec3 l_d = vec3 (0.2, 0.2, 0.2); // Diffuse color
vec3 l_s = vec3 (0.2, 0.2, 0.2);// Specular color

// Two color variables to create nice colour scale.
uniform int color_factor;
uniform int color_factor2;

float spec_exp = 5.0f;
vec3 k_a = vec3 (0.4, 0.2 , 0.5); // ambient
vec3 k_d = vec3 (0.4, 0.2 , 0.5); // diff
vec3 k_s = vec3 (0.4, 0.2 , 0.5); // spec

void main () {


	// Two types of colour shades. 
	if(color_factor2 < 5)
	{
		k_a = vec3 (0.02*color_factor, 0.2*color_factor2, 0.3 + 0.1*color_factor2); // ambient
		k_d = vec3 (0.02*color_factor, 0.2*color_factor2, 0.3 + 0.1*color_factor2); // diff
		k_s = vec3 (0.02*color_factor, 0.2*color_factor2, 0.3 + 0.1*color_factor2); // spec
	}
	else
	{
		k_a = vec3 (0.03*color_factor, 0.4 + 0.05*color_factor2, 0.2 + 0.2*color_factor2); // ambient
		k_d = vec3 (0.03*color_factor, 0.4 + 0.05*color_factor2, 0.2 + 0.2*color_factor2); // diff
		k_s = vec3 (0.03*color_factor, 0.4 + 0.05*color_factor2, 0.2 + 0.2*color_factor2); // spec
	}
	
	if(color_factor == -100) //  Menus Color
	{
		k_a = vec3 (1.5, 1.5, 1.5); // ambient
		k_d = vec3 (1.5, 1.5, 1.5); // diff
		k_s = vec3 (1.5, 1.5, 1.5); // spec
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
