void main()
{
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = vec4(gl_Vertex.xyz,0);
	gl_Position = vec4(gl_Vertex.xyz * 32.0 - 16.0, gl_Vertex.w);
}