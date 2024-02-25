#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

void main() {
	gl_Position = position;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 gl_FragCoord;

void main() {
	vec2 loc(.5f, .5);
	float distance = gl_FragCoord - loc;
	
	color = vec4(gl_FragCoord.x / 1000, gl_FragCoord.y / 1000,0,1);
};