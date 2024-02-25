#version 330 core

out vec4 color;  // Resulting pixel color

uniform sampler2D previousFrame;
in vec4 gl_FragCoord;     // current pixel location
uniform vec2 uResolution; // screen dimensions in pixels

#define margin 10
#define iterations 1000

vec4 getValue(vec2 pixel) {
	// Boundary Conditions
	if(pixel.x <= margin) {
		return vec4(.9);
	}
	if(pixel.x >= (uResolution.x - margin)) {
		return vec4(0);
	}
	if(pixel.y <= margin) {
		return vec4(.9);
	}
	if(pixel.y >= (uResolution.y - margin)) {
		return vec4(0);
	}

	// Interior, return current color
	vec4 previousColor = texture(previousFrame, pixel / uResolution);
	return previousColor;
}

void main() {
	vec2 pixelLoc = gl_FragCoord.xy;

	// Find neighbors' colors
	vec4 left = getValue(pixelLoc - vec2(1, 0));
	vec4 right = getValue(pixelLoc + vec2(1, 0));
	vec4 up = getValue(pixelLoc + vec2(0, 1));
	vec4 down = getValue(pixelLoc - vec2(0, 1));

	// Average
	vec4 newSelfColor = (left + right + up + down) / 4.0;

	color = newSelfColor;  // Output color
};