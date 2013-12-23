#version 120
#extension GL_EXT_gpu_shader4 : enable

varying vec3 o_position; // view space position
varying vec2 o_texcoord; // copied through from aTexcoord

/*----------------------------------------------------------------------------*/
void main() {
	o_position = gl_Vertex.xyz;
	o_texcoord = gl_MultiTexCoord0.xy;

	gl_Position = gl_Vertex;
}