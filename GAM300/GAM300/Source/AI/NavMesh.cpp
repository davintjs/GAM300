#include "Precompiled.h"

#include "NavMesh.h"

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

bool NavMesh::FindPath(glm::vec3& mStart, glm::vec3& mEnd)
{
	UNREFERENCED_PARAMETER(mStart);
	UNREFERENCED_PARAMETER(mEnd);
	return false;
}

void NavMesh::LinkTriangles(Triangle3D* mTri1, Triangle3D* mTri2)
{
	mTri1->AddNeighbour(mTri2);
	mTri2->AddNeighbour(mTri1);
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