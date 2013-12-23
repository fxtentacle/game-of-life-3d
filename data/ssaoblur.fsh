uniform sampler2D uColorTex;
uniform sampler2D uSsaoTex;

const int uBlurSize = 3;
uniform vec2 texelSize;

void main() {
//	ideally use a fixed size noise and blur so that this loop can be unrolled
	vec4 fResult = vec4(0.0);
	vec2 hlim = vec2(float(-uBlurSize) * 0.5 + 0.5);
	for (int x = 0; x < uBlurSize; ++x) {
		for (int y = 0; y < uBlurSize; ++y) {
			vec2 offset = vec2(float(x), float(y));
			offset += hlim;
			offset *= texelSize;
					
			fResult += texture2D(uSsaoTex, gl_TexCoord[0].xy + offset);
		}
	}

	gl_FragColor = texture2D(uColorTex, gl_TexCoord[0].xy) * fResult * (1.0 / float(uBlurSize * uBlurSize));
	//gl_FragColor = texture2D(uSsaoTex, gl_TexCoord[0].xy);
}