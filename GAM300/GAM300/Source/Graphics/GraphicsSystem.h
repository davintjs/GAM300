#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "Core/SystemInterface.h"
#include "GraphicStructsAndClass.h"




class Ray3D;

ENGINE_SYSTEM(GraphicsSystem)
{
public:
	void Init();
	void Update(float dt);
	void Draw();
	void Draw_Meshes(GLuint vaoid, unsigned int instance_count,
		unsigned int prim_count, GLenum prim_type, LightProperties LightSource,
		glm::vec4 Albe, glm::vec4 Spec, glm::vec4 Diff, glm::vec4 Ambi, float Shin);
		//Materials Mat);
	void Exit();

	bool Raycasting(Ray3D& _ray);
};
#endif // !GRAPHICS_SYSTEM_H