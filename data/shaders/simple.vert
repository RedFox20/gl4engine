#version 150 // OpenGL 3.2

uniform mat4 transform;     // transformation matrix

attribute vec3 position;    // vertex position
attribute vec2 coord;       // vertex texture coordinates
attribute vec3 normal;      // vertex normal

varying vec2 vCoord;        // out vertex texture coord for frag

void main(void)
{
	gl_Position = transform * vec4(position, 1.0);
	vCoord = coord;
}