uniform sampler2D texture;
varying vec2 vCoord;          // vertex texture coordinates
varying vec4 vColor;          // vertex texture color

void main()
{
    float a = texture2D(texture, vCoord).r;
    gl_FragColor = vec4(vColor.rgb, vColor.a*a);
}
