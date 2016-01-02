#version 330 // OpenGL 3.3

uniform mat4 transform;     // transformation matrix

in vec3 position;    // in vertex position
in vec2 coord;       // in vertex texture coordinates
in vec3 normal;      // in vertex normal

out vec2 vCoord;     // out vertex texture coord for frag

void main(void)
{
	gl_Position = transform * vec4(position, 1.0);
	vCoord = coord;
}