#version 150 // OpenGL 3.2
 
varying vec2 vCoord;          // vertex texture coordinates

uniform sampler2D texture;

void main(void)
{
	gl_FragColor = texture2D(texture, vCoord);
}