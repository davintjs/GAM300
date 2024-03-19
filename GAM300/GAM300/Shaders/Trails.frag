#version 450 core

uniform vec3 trailColor;

out vec4 FragColor;

//End

void main()
{
	FragColor = vec4(trailColor, 1.0);
}