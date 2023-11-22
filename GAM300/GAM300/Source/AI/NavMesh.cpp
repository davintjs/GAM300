/*!***************************************************************************************
\file			NavMesh.cpp
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definitions of the following:
	1. NavMesh class
		a. Linking of triangles by initializing the neighbours
		b. Getter functions
		c. Run time update of the navmesh

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"

#include "NavMesh.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"

const std::vector<Triangle3D> NavMesh::GetNavMeshTriangles() const
{
	return mTriangles;
}

const std::vector<glm::vec3> NavMesh::GetPoints() const
{
	return mPoints;
}

const std::vector<unsigned int> NavMesh::GetIndices() const
{
	return mIndices;
}

bool NavMesh::FindPath(NavMeshAgent& mAgent, const glm::vec3& mEnd)
{
	Scene& tempScene = MySceneManager.GetCurrentScene();
	Transform& agentTransform = tempScene.Get<Transform>(mAgent);

	Triangle3D* mTri1 = TriangleContainingPoint(agentTransform.GetTranslation());
	Triangle3D* mTri2 = TriangleContainingPoint(mEnd);

	if (mTri1 == nullptr || mTri2 == nullptr) // Out of bounds of navmesh triangles
	{
		return false;
	}
	else if (mTri1 == mTri2) // Start and end is same triangle
	{
		mAgent.mPoints.push_back(mEnd - agentTransform.GetTranslation());
		return true;
	}

	AStarPather pather; // Temporary
	if (pather.ComputePath(mTri1, mTri2))
	{
		mAgent.mPoints = pather.PathPostProcess(agentTransform.GetTranslation(), mEnd); // The vector will contain the points that the agent will need to walk to until he reach the goal
		for (const auto& i : mAgent.mPoints)
		{
			std::cout << "Found Path: " << i.x << " " << i.y << " " << i.z << std::endl;
		}
		return true;
	}

	return false;
}

void NavMesh::LinkTriangles(Triangle3D* mTri1, Triangle3D* mTri2)
{
	mTri1->AddNeighbour(mTri2);
	mTri2->AddNeighbour(mTri1);
}

Triangle3D* NavMesh::TriangleContainingPoint(const glm::vec3& mPoint)
{
	for (auto& tri : mTriangles)
	{
		if (tri.ContainsPoint(mPoint))
		{
			return &tri;
		}
	}

	return nullptr;
}

void NavMesh::LinkAllTriangles()
{
	for (int i = 0; i < mTriangles.size(); ++i)
	{
		for (int j = i + 1; j < mTriangles.size(); ++j)
		{
			int count = 0;
			for (int k = 0; k < 3; ++k)
			{
				for (int l = 0; l < 3; ++l)
				{
					if (mTriangles[i][k] == mTriangles[j][l])
					{
						++count;
					}
					if (count > 1)
					{
						break;
					}
				}
				if (count > 1)
				{
					break;
				}
			}
			if (count > 1)
			{
				LinkTriangles(&mTriangles[i], &mTriangles[j]);
			}
		}
	}
}

void NavMesh::UpdateNavMesh()
{

}