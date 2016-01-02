uniform mat4 transform;
attribute vec3 vertex;
attribute vec2 coord;
attribute vec4 color;

varying vec2 vCoord;        // out vertex texture coord for frag
varying vec4 vColor;        // out vertex texture color for frag

void main()
{
    gl_Position = transform * vec4(vertex, 1.0);
	vCoord = coord;
	vColor = color;
}
