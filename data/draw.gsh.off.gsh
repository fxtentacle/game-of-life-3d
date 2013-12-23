#extension GL_EXT_geometry_shader4:require

void main()
{
	
    vec4 centerPosition = vec4(gl_PositionIn[0].xyz, 1.0);
    float siz = 0.5 * gl_PositionIn[0].w;
    float siz2 = siz / 32.0;


    gl_TexCoord[2] = vec4(0,0,-1, 0);
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] = vec4(0,0,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(-1, -1, -1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(-1, -1, -1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(1,0,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(1, -1, -1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(1, -1, -1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(0,1,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(-1, 1, -1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(-1, 1, -1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(1,1,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(1, 1, -1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(1, 1, -1,0));
	EmitVertex();
	EndPrimitive();

    gl_TexCoord[2] = vec4(0,0,1, 0);
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] = vec4(0,0,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(-1, -1, 1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(-1, -1, 1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(1,0,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(1, -1, 1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(1, -1, 1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(0,1,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(-1, 1, 1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(-1, 1, 1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(1,1,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(1, 1, 1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(1, 1, 1,0));
	EmitVertex();
	EndPrimitive();



    gl_TexCoord[2] = vec4(-1,0,0, 0);
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] = vec4(0,0,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(-1, -1, -1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(-1, -1, -1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(1,0,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(-1, -1, 1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(-1, -1, 1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(0,1,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(-1, 1, -1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(-1, 1, -1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(1,1,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(-1, 1, 1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(-1, 1, 1,0));
	EmitVertex();
	EndPrimitive();

    gl_TexCoord[2] = vec4(1,0,0, 0);
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] = vec4(0,0,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(1, -1, -1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(1, -1, -1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(1,0,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(1, -1, 1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(1, -1, 1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(0,1,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(1, 1, -1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(1, 1, -1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(1,1,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(1, 1, 1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(1, 1, 1,0));
	EmitVertex();
	EndPrimitive();



    gl_TexCoord[2] = vec4(0,1,0, 0);
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] = vec4(0,0,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(-1, 1, -1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(-1, 1, -1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(1,0,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(-1, 1, 1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(-1, 1, 1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(0,1,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(1, 1, -1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(1, 1, -1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(1,1,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(1, 1, 1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(1, 1, 1,0));
	EmitVertex();
	EndPrimitive();


    gl_TexCoord[2] = vec4(0,-1,0, 0);
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] = vec4(0,0,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(-1, -1, -1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(-1, -1, -1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(1,0,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(-1, -1, 1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(-1, -1, 1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(0,1,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(1, -1, -1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(1, -1, -1,0));
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] =  vec4(1,1,0,0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (centerPosition + siz*vec4(1, -1, 1,0));
	gl_TexCoord[1] = (gl_TexCoordIn[0][1] + siz2*vec4(1, -1, 1,0));
	EmitVertex();
	EndPrimitive();

}
