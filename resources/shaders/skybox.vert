#version 150 
#extension GL_ARB_explicit_attrib_location : require
layout (location = 0) in vec3 in_Position;

out vec3 TexCoords;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main() {

	//remove translation
	mat4 view = mat4(mat3(ViewMatrix));
	TexCoords = in_Position;
	vec4 pos = ProjectionMatrix * view * vec4(in_Position,1.0);
	gl_Position = pos.xyww;
}