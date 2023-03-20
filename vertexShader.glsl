#version 430

layout (location = 0) in vec3 aPos;
out vec4 circlePosition;

void main(void)
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	circlePosition = gl_Position;
}
