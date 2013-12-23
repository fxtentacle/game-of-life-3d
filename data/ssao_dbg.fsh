#version 120
#extension GL_EXT_gpu_shader4 : enable
#define GL_EXT_gpu_shader4 1

uniform sampler2D texA;
uniform sampler2D texB;
uniform sampler2D texC;


varying vec3 o_position;
varying vec2 o_texcoord;
varying mat3 o_tangentViewMatrix;
noperspective varying vec3 o_viewRay;


void main() {
	gl_FragColor = texture2D(texC, o_texcoord).rgba;
}