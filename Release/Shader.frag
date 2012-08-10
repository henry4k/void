#version 120

varying vec2 TexCoord;

uniform sampler2D DiffuseMap;

void main()
{
	gl_FragColor.rgb = texture2D(DiffuseMap, TexCoord).rgb;
}