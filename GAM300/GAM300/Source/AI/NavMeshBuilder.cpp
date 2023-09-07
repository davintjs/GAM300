#include "Precompiled.h"

#include "NavMeshBuilder.h"

NavMeshBuilder::NavMeshBuilder(const std::vector<glm::vec3>& GroundVertices, const std::vector<glm::ivec3>& GroundIndices)
{
	std::vector<Triangle3D> GroundTriangles = GetGroundTriangles(GroundVertices, GroundIndices);
	mRegion = ComputeRegions(GroundTriangles);
}

NavMeshBuilder::~NavMeshBuilder()
{

}

NavMesh* NavMeshBuilder::CreateNavMesh()
{
	return nullptr;
}

std::vector<Polygon>& NavMeshBuilder::GetRegion()
{
	return mRegion;
}

std::vector<Polygon>& NavMeshBuilder::GetHoles()
{
	return mHoles;
}

std::vector<Polygon> NavMeshBuilder::ComputeRegions(const std::vector<Triangle3D>& GroundTriangles)
{
	std::vector<Polygon> temp;
	return temp;
}

std::vector<Triangle3D> NavMeshBuilder::GetGroundTriangles(const std::vector<glm::vec3>& GroundVertices, const std::vector<glm::ivec3>& GroundIndices)
{
	std::vector<Triangle3D> temp;
	return temp;
}

void NavMeshBuilder::SetBoundary(Polygon* boundary)
{
	mBoundary = boundary;
}

void NavMeshBuilder::AddHole(Polygon* hole)
{
	mHoles.push_back(*hole);
}

void NavMeshBuilder::RemoveHoles()
{

}

std::vector<Triangle3D> NavMeshBuilder::Triangulate()
{
	std::vector<Triangle3D> temp;
	return temp;
}