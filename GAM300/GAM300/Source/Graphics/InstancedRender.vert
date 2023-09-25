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
layout (location = 15) in vec2 texture_index;
// Out
//layout (location = 0) out vec4 vColor;
layout (location = 0) out vec2 frag_texture_index;
layout (location = 1) out vec3 FragmentPos;
layout (location = 2) out vec3 Normal;

layout (location = 4) out vec2 Tex_Coord;


layout (location = 10) out vec4 frag_albedo;
layout (location = 11) out vec4 frag_specular;
layout (location = 12) out vec4 frag_diffuse;
layout (location = 13) out vec4 frag_ambient;
layout (location = 14) out float frag_shininess;

layout (location = 5) out vec3 TangentLightPos;
layout (location = 15) out vec3 TangentViewPos;
layout (location = 16) out vec3 TangentFragPos;


//layout (location = 15 ) out VS_OUT {
//    vec3 TangentLightPos;
//    vec3 TangentViewPos;
//    vec3 TangentFragPos;
//} vs_out;


uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;


uniform mat4 persp_projection;
uniform mat4 View;
// uniform mat4 SRT;

void main()
{
//	vColor = aVertexColor;
	gl_Position = persp_projection * View * SRT * vec4(aVertexPosition, 1.0f);

	FragmentPos = vec3(SRT * vec4(aVertexPosition, 1.0));

	// DO THIS TRANSPOSE INVERSE IN CPU THEN UNIFORM INTO SHADER
    Normal = mat3(transpose(inverse(SRT))) * aVertexNormal;

	frag_albedo = Albedo;
	frag_specular = Specular;
	frag_diffuse = Diffuse;
	frag_ambient = Ambient;
	frag_shininess = Shininess;
	Tex_Coord = aVertexTexCoord;
	frag_texture_index = texture_index;
	mat3 normalMatrix = transpose(inverse(mat3(SRT)));
    vec3 T = normalize(normalMatrix * aVertexTangent);
    vec3 N = normalize(normalMatrix * aVertexNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));    
    TangentLightPos = TBN * lightPos;
    TangentViewPos  = TBN * camPos;
    TangentFragPos  = TBN * FragmentPos;
//    vs_out.TangentLightPos = TBN * lightPos;
//    vs_out.TangentViewPos  = TBN * camPos;
//    vs_out.TangentFragPos  = TBN * FragmentPos;
//

} 