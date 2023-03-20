#version 430

out vec4 color;
uniform int drawColor;

void main(void)
{
	if(drawColor == 1)
		color = vec4(0.0, 1.0, 0.0, 1.0);
	else if(drawColor == 2)
		color = vec4(0.0, 0.0, 1.0, 1.0);
	else if(drawColor == 3)
		color = vec4(1.0, 0.0, 0.0, 1.0);
}
