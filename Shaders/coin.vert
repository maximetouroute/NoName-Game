#version 410

in vec3 vp; // points
in vec3 vn; // normals (will just use as colours)

uniform mat4 M, V, P; // model matrix

out vec3 n_eye;
out vec3 p_eye;

void main () {
	// send normals to fragment shader
	n_eye = (V * M * vec4 (vn, 0.0)).xyz;
	p_eye = (V * M * vec4 (vp, 1.0)).xyz;
	gl_Position = P * vec4 (p_eye, 1.0);
}
