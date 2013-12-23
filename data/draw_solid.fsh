#version 120
uniform sampler2D tex;

void main()
{
//    gl_FragData[0] = vec4(gl_TexCoord[2].xyz*0.5+0.5,0);
	gl_FragData[0] = clamp( vec4( normalize( gl_TexCoord[1].xyz * 0.5 + 0.5 )*1.5, 1 ), 0, 1)  * (1.0-texture2D(tex, gl_TexCoord[0].xy));
    gl_FragData[1] = gl_TexCoord[2] * 0.5 + 0.5;
}
