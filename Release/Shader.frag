varying vec3 TexCoord;

uniform sampler2D ColorMap;

void main()
{
	gl_FragColor.rgb = texture2D(ColorMap, TexCoord).rgb;
}