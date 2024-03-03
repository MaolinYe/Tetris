#version 330 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColor;
out vec4 color;

uniform int xsize;
uniform int ysize;

void main() 
{
	mat4 scale = mat4(2.0/xsize, 0.0, 0.0, 0.0,
					  0.0,  2.0/ysize, 0.0, 0.0,
					  0.0, 0.0, 1.0, 0.0,
					  0.0, 0.0, 0.0, 1.0 );
	mat4 transl = mat4( 1, 0, 0, -xsize/2,
						0, 1, 0, -ysize/2,
						0, 0, 1, 0,
						0, 0, 0, 1);
	gl_Position = vPosition * transl * scale;
    color = vColor;
}