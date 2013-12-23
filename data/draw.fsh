uniform sampler2D tex;

void main()
{
	gl_FragColor = gl_Color * vec4(gl_TexCoord[1].xyz * 0.5 + 0.5, 1.0) * texture2D(tex, gl_TexCoord[0].xy);
}
