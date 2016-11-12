#version 410

in vec3 n_eye; // normals from vertex shader
in vec3 p_eye;

out vec4 frag_colour;

vec3 l_dir = normalize (vec3 (0.0f, 0.0f, -1.0f));

// Color of the light
vec3 l_a = vec3 (0.3, 0.3, 0.3); // ambiant color
vec3 l_d = vec3 (0.4, 0.4, 0.4); // Diffuse color
vec3 l_s = vec3 (0.5, 0.5, 0.5);// Specular color



float spec_exp = 20.0f; // Specular light strength
vec3 k_a = vec3 (1.0, 0.8 , 0.8); // ambient
vec3 k_d = vec3 (1.0, 0.8 , 0.8); // diff
vec3 k_s = vec3 (1.0, 0.8 , 0.8); // spec

void main () 
{

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
