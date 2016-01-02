#version 330 // OpenGL 3.3
 
uniform sampler2D diffuseTex; // diffuse texture
in vec2 vCoord;               // vertex texture coords

out vec4 fragColor; // output pixel color 

void main(void)
{
	fragColor = texture(diffuseTex, vCoord);
}