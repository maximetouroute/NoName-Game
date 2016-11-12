#version 410

in vec3 n_eye; // normals from vertex shader
in vec3 p_eye;

out vec4 frag_colour;
uniform float health;
uniform float spec;
float platform_health;

vec3 l_dir = normalize (vec3 (0.0f, 0.0f, -0.1f));


//Phung lighting again.
vec3 l_a = vec3 (0.0, 0.0, 0.0); // ambiant color
vec3 l_d = vec3 (0.1, 0.1, 0.1); // Diffuse color
vec3 l_s = vec3 (0.5, 0.5, 0.5);// Specular color


float spec_exp = 50.0f; 
vec3 k_a = vec3 (0.5, 0.5 , 0.5); // ambient
vec3 k_d = vec3 (1.0, 1.0 , 1.0); // diff
vec3 k_s = vec3 (0.8, 0.8 , 0.8); // spec

void main () {

	spec_exp = spec/10;
	platform_health = health ;

	// Default platform color
	 k_a = vec3 (0.0, 0.0 , 0.0); // ambient
	 k_d = vec3 (0.6, 0.6 , 0.6); // diff
	 k_s = vec3 (1.0, 1.0, 1.0); // spec

	if( health > 0) // If the platform is dying
	{
	// Go DEFAULT_COLOUR to RED.
	k_a.y *= platform_health+0.1;
	k_d.y *= platform_health+0.1;
	k_s.y *= platform_health+0.1;


	k_a.z *= platform_health+0.1;
	k_d.z *= platform_health+0.1;
	k_s.z *= platform_health+0.1;
	}
	else // Colour of the dead platform
	{
		k_a = vec3 (0.4, 0.0 , 0.0); // ambient
		k_d = vec3 (0.6, 0.0 , 0.0); // diff
		k_s = vec3 (0.8, 0.0 , 0.0); // spec
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
