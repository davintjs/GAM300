#version 450 core
layout (location = 0) in vec3 aVertexPosition;
layout (location = 1) in vec3 aVertexNormal;

layout (location = 2) in vec3 aVertexTangent;
layout (location = 3) in vec2 aVertexTexCoord; //UVs info
layout (location = 4) in vec4 aVertexColor; // This can throw

layout (location = 6) in mat4 SRT;


layout (location = 10) in vec4 Albedo;
layout (location = 11) in vec4 Specular;
layout (location = 12) in vec4 Diffuse;
layout (location = 13) in vec4 Ambient;
layout (location = 14) in float Shininess;
layout (location = 15) in float texture_index;





layout (location = 0) out vec4 vColor;
layout (location = 1) out vec3 FragmentPos;
layout (location = 2) out vec3 Normal;
layout (location = 3) out vec2 frag_TexCoord;


layout (location = 10) out vec4 frag_albedo;
layout (location = 11) out vec4 frag_specular;
layout (location = 12) out vec4 frag_diffuse;
layout (location = 13) out vec4 frag_ambient;
layout (location = 14) out float frag_shininess;
layout (location = 15) out float frag_texture_index;







uniform mat4 persp_projection;
uniform mat4 View;
// uniform mat4 SRT;

void main()
{
	vColor = aVertexColor;
	gl_Position = persp_projection * View * SRT * vec4(aVertexPosition, 1.0f);

	FragmentPos = vec3(SRT * vec4(aVertexPosition, 1.0));

	// DO THIS TRANSPOSE INVERSE IN CPU THEN UNIFORM INTO SHADER
    Normal = mat3(transpose(inverse(SRT))) * aVertexNormal;

	frag_albedo = Albedo;
	frag_specular = Specular;
	frag_diffuse = Diffuse;
	frag_ambient = Ambient;
	frag_shininess = Shininess;
	frag_TexCoord = aVertexTexCoord;
	frag_texture_index = texture_index;
} 