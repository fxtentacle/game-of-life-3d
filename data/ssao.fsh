#version 120
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D uGBufferGeometricTex;
uniform sampler2D uGBufferDepthTex;
uniform sampler2D uNoiseTex;

const mat3 o_tangentViewMatrix = mat3( vec3(0.0, 0.0, -1.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0) );

uniform mat4 uProjectionMatrix; // current projection matrix, for linearized depth
uniform mat4 uInvProjectionMatrix; // current projection matrix, for linearized depth


//	ssao uniforms:
const int MAX_KERNEL_SIZE = 128;
uniform int uKernelSize;
uniform vec3 uKernelOffsets[MAX_KERNEL_SIZE];
const float uRadius = 5.0;
const float uPower = 2.0;
uniform vec2 noiseScale; //vec2(textureSize(uGBufferDepthTex, 0)) / vec2(textureSize(uNoiseTex, 0));


varying vec3 o_position;
varying vec2 o_texcoord;
#if GL_EXT_gpu_shader4
noperspective varying vec3 o_viewRay;
#else
varying vec3 o_viewRay;
#endif


/*----------------------------------------------------------------------------*/
float ssao(in mat3 kernelBasis, in vec3 originPos, in float radius) {
	float occlusion = 0.0;
	for (int i = 0; i < uKernelSize; ++i) {
	//	get sample position:
		vec3 samplePos = kernelBasis * uKernelOffsets[i];
		samplePos = samplePos * radius + originPos;
		
	//	project sample position:
		vec4 offset = uProjectionMatrix * vec4(samplePos, 1.0);
		offset.xy /= offset.w; // only need xy
		offset.xy = offset.xy * 0.5 + 0.5; // scale/bias to texcoords
		
	//	get sample depth:
		float sampleDepth = texture2D(uGBufferDepthTex, offset.xy).r;
		sampleDepth = uProjectionMatrix[3][2] / (sampleDepth - uProjectionMatrix[2][2]);
		
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(originPos.z - sampleDepth));
		occlusion += rangeCheck * step(sampleDepth, samplePos.z);
	}
	
	occlusion = 1.0 - (occlusion / float(uKernelSize));
	return pow(occlusion, uPower);
}

/*----------------------------------------------------------------------------*/
void main() {
//	get noise texture coords:
	vec2 noiseTexCoords = noiseScale;
	noiseTexCoords *= o_texcoord;
	
//	get view space origin:
	float screenDepth = texture2D(uGBufferDepthTex, o_texcoord).r;
    vec4 screenPos = vec4(o_position.xy, screenDepth, 1.0);
	vec4 originPosTmp = uInvProjectionMatrix * screenPos;
    vec3 originPos = originPosTmp.xyz / originPosTmp.w;

//	get view space normal:
	vec3 normal = texture2D(uGBufferGeometricTex, o_texcoord).rgb * 2.0 - 1.0;
		
//	construct kernel basis matrix:
	vec3 rvec = texture2D(uNoiseTex, noiseTexCoords).rgb * 2.0 - 1.0;
	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitangent = cross(tangent, normal);
	mat3 kernelBasis = mat3(tangent, bitangent, normal);
	
    float normalBrightness = clamp( dot(normal, normalize(vec3(0.3,1,-1)) ), 0, 1);
    float bri = normalBrightness * 0.5 + ssao(kernelBasis, originPos, uRadius) * 0.8;
	gl_FragColor = vec4(bri);
    //gl_FragColor = vec4(normal*0.5+0.5, 1);
}