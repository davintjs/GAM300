#include "Precompiled.h"

#include "NavMeshBuilder.h"

void NavMeshBuilder::BuildNavMesh(const std::vector<glm::vec3>& GroundVertices, const std::vector<glm::ivec3>& GroundIndices)
{
	std::vector<Triangle3D> GroundTriangles = GetGroundTriangles(GroundVertices, GroundIndices);
	mRegion = ComputeRegions(GroundTriangles); // Compute the regions of the given ground
	mNavMesh = CreateNavMesh(); // Create the navmesh
	mNavMesh->LinkAllTriangles();
}

NavMesh* NavMeshBuilder::CreateNavMesh()
{
	RemoveHoles(); // Remove holes in the boundary
	NavMesh* mNavMesh = new NavMesh(Triangulate());

	return mNavMesh;
}

std::vector<Polygon3D>& NavMeshBuilder::GetRegion()
{
	return mRegion;
}

std::vector<Polygon3D>& NavMeshBuilder::GetHoles()
{
	return mHoles;
}

std::vector<Polygon3D> NavMeshBuilder::ComputeRegions(const std::vector<Triangle3D>& GroundTriangles)
{
	std::vector<Triangle3D> copyTri = GroundTriangles;
	std::vector<Polygon3D> resPolygons;

	while (copyTri.size() > 1)
	{
		Triangle3D& mCurrTriangle = copyTri[0];
		std::vector<Triangle3D> triVector;
		triVector.push_back(mCurrTriangle);

		for (int i = 0; i < mCurrTriangle.GetNeighbours().size(); ++i)
		{
			Triangle3D& mNeighbourTriangle = *mCurrTriangle.GetNeighbours()[i];
			if (Parallel(mCurrTriangle.GetNormal(), mNeighbourTriangle.GetNormal()))
			{
				bool isIn = false;
				for (auto& mTri : triVector)
				{
					if (mTri.GetID() == mNeighbourTriangle.GetID())
					{
						isIn = true;
						break;
					}
				}
				if (isIn)
				{
					continue;
				}
				triVector.push_back(mNeighbourTriangle);
				FindAllNeighboursWithNormal(triVector, mNeighbourTriangle, mCurrTriangle.GetNormal());
			}
		}

		for (auto& tri : triVector) // Erasing all added triangles in triVector from copyTri
		{
			for (int k = 0; k < copyTri.size(); ++k)
			{
				if (tri.GetMidPoint() == copyTri[k].GetMidPoint())
				{
					copyTri.erase(copyTri.begin() + k);
					--k;
				}
			}
		}

		std::vector<glm::vec3> triPos;
		for (auto k : triVector)
		{
			triPos.push_back(glm::vec3(k[0].x, k[0].y, k[0].z));
			triPos.push_back(glm::vec3(k[1].x, k[1].y, k[1].z));
			triPos.push_back(glm::vec3(k[2].x, k[2].y, k[2].z));
		}

		// Sort the vector and remove duplicates
		for (int a = 0; a < triPos.size() - 1; ++a)
		{
			glm::vec3 mCurrPos = triPos[a];
			for (int b = a + 1; b < triPos.size(); ++b)
			{
				glm::vec3 mComparingPos = triPos[b];
				if (mCurrPos == mComparingPos)
				{
					triPos.erase(triPos.begin() + b);
					--b;
				}
			}
		}
		resPolygons.push_back(Polygon3D(triPos));
	}
	return resPolygons;
}

std::vector<Triangle3D> NavMeshBuilder::GetGroundTriangles(const std::vector<glm::vec3>& GroundVertices, const std::vector<glm::ivec3>& GroundIndices)
{
	std::vector<Triangle3D> resTri;

	for (int i = 0; i < GroundIndices.size(); ++i)
	{
		glm::vec3 firstPoint = GroundVertices[GroundIndices[i].x];
		glm::vec3 secondPoint = GroundVertices[GroundIndices[i].y];
		glm::vec3 thirdPoint = GroundVertices[GroundIndices[i].z];

		Triangle3D mTri(firstPoint, secondPoint, thirdPoint);
		mTri.SetTriID(++mTriCount);

		resTri.push_back(mTri);
	}

	InitializeGroundTriangles(resTri); // Initialize their neighbours

	return resTri;
}

void NavMeshBuilder::SetBoundary(Polygon3D* boundary)
{
	mBoundary = boundary;
}

void NavMeshBuilder::AddHole(Polygon3D* hole)
{
	mHoles.push_back(*hole);
}

void NavMeshBuilder::RemoveHoles()
{
	std::sort(mHoles.begin(), mHoles.end(), [](Polygon3D& p1, Polygon3D& p2)
		{
			return p1.GetMaxPoint().y > p2.GetMaxPoint().y;
		});

	for (auto& polygon : mRegion)
	{
		for (auto& hole : mHoles)
		{
			if (polygon.HoleInPolygon(hole)) // Check if hole intersects this polygon
			{
				// Switch the orientation of the hole
				if (polygon.GetOrientation() == hole.GetOrientation())
				{
					hole.SwitchOrientation();
				}
				
				polygon.JoinPolygon(hole); // Add the hole into the polygon
			}
		}
	}
}

std::vector<Triangle3D> NavMeshBuilder::Triangulate()
{
	std::vector<Triangle3D> TriangulatedMesh; // The vector that the method will return

	for (auto& polygon : mRegion)
	{
		// Step 1: Store the vertices in a list (Includes the next and prev vertices)
		std::vector<glm::vec3> vertices = polygon.GetPoints();

		// If only 3 points, return it as a triangle
		if (vertices.size() == 3)
		{
			TriangulatedMesh.push_back(Triangle3D(vertices[2], vertices[1], vertices[0]));
			continue;
		}

		polygon.CalculateNormal(vertices);

		// Step 2: Find reflex and convex vertices, and ear vertices
		std::vector<glm::vec3> nonConvexPoints;
		for (int i = 0; i < vertices.size(); ++i)
		{
			glm::vec3 firstPoint = vertices[i - 1 < 0 ? vertices.size() - 1 : i - 1];
			glm::vec3 secondPoint = vertices[i];
			glm::vec3 thirdPoint = vertices[i + 1 == vertices.size() ? 0 : i + 1];

			if (!isFrontFace(firstPoint, secondPoint, thirdPoint))
			{
				nonConvexPoints.push_back(secondPoint);
			}
		}

		// Step 3: Triangulate
		while (vertices.size() > 2)
		{
			if (vertices.size() == 3)
			{
				TriangulatedMesh.push_back(Triangle3D(vertices[2], vertices[1], vertices[0]));
				break;
			}

			bool guard = false;

			for (int i = 0; i < vertices.size(); ++i)
			{
				glm::vec3 firstPoint = vertices[i - 1 < 0 ? vertices.size() - 1 : i - 1];
				glm::vec3 secondPoint = vertices[i];
				glm::vec3 thirdPoint = vertices[i + 1 == vertices.size() ? 0 : i + 1];

				if (!isFrontFace(firstPoint, secondPoint, thirdPoint))
				{
					continue;
				}

				Triangle3D mCurrTriangle(firstPoint, secondPoint, thirdPoint);

				bool containsPoint = false;
				for (const auto& p : nonConvexPoints)
				{
					if (p == mCurrTriangle[0] || p == mCurrTriangle[1] || p == mCurrTriangle[2])
					{
						continue;
					}

					if (mCurrTriangle.ContainsPoint(p))
					{
						containsPoint = true;
						break;
					}
				}

				if (!containsPoint)
				{
					guard = true;
					TriangulatedMesh.push_back(mCurrTriangle);

					glm::vec3 previousPreviousPoint = vertices[i - 2 == -1 ? vertices.size() - 2 : (i - 1 < 0 ? vertices.size() - 1 : i - 1)]; // The previous previous point of the current point of this loop (secondPoint)
					glm::vec3 previousPoint = firstPoint; // Our start
					glm::vec3 nextOfPreviousPoint = secondPoint; // This is just our current point

					if (isFrontFace(previousPreviousPoint, previousPoint, nextOfPreviousPoint)) // Deleting the previous point if it is convex
					{
						for (int j = 0; j < nonConvexPoints.size(); ++j)
						{
							if (nonConvexPoints[j] == previousPoint)
							{
								nonConvexPoints.erase(nonConvexPoints.begin() + j);
								break;
							}
						}
					}

					glm::vec3 previousOfNextPoint = secondPoint; // This is just our current point
					glm::vec3 nextPoint = thirdPoint; // Our start
					glm::vec3 nextNextPoint = vertices[i + 2 == vertices.size() + 1 ? 1 : (i + 1 == vertices.size() ? 0 : i + 1)]; // The next next point of the current point of this loop (secondPoint)

					if (isFrontFace(previousOfNextPoint, nextPoint, nextNextPoint)) // Deleting the next point if it is convex
					{
						for (int j = 0; j < nonConvexPoints.size(); ++j)
						{
							if (nonConvexPoints[j] == nextPoint)
							{
								nonConvexPoints.erase(nonConvexPoints.begin() + j);
								break;
							}
						}
					}

					for (int i = 0; i < vertices.size(); i++)
					{
						if (vertices[i] == secondPoint)
						{
							vertices.erase(vertices.begin() + i);
							break;
						}
					}

					break;
				}
			}
			if (PointsOnLine(vertices, polygon.GetNormal()))
			{
				break;
			}

			if (!guard)
			{
				break;
			}
		}
	}
	return TriangulatedMesh;
}

void NavMeshBuilder::InitializeGroundTriangles(std::vector<Triangle3D>& GroundTriangles)
{
	for (int i = 0; i < GroundTriangles.size(); ++i)
	{
		Triangle3D& mCurrTriangle = GroundTriangles[i];

		for (int j = i + 1; j < GroundTriangles.size(); ++j)
		{
			Triangle3D& mComparingTriangle = GroundTriangles[j];
			int count = 0;
			for (int k = 0; k < 3; ++k) // To traverse current triangle's points
			{
				for (int l = 0; l < 3; ++l) // To traverse comparing triangle's points
				{
					if (mCurrTriangle[k] == mComparingTriangle[l])
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
			if (count > 1) // Link the neighbouring triangles here (For both triangles)
			{
				mCurrTriangle.AddNeighbour(&mComparingTriangle);
				mComparingTriangle.AddNeighbour(&mCurrTriangle);
			}
		}
	}
}

void NavMeshBuilder::FindAllNeighboursWithNormal(std::vector<Triangle3D>& triPos, Triangle3D neighbourTri, const glm::vec3 triNormal)
{
	for (int i = 0; i < neighbourTri.GetNeighbours().size(); ++i)
	{
		Triangle3D mCurrTri = *neighbourTri.GetNeighbours()[i];

		bool alreadyIn = false;
		// Check if triangle is already in our vector
		for (auto& m : triPos)
		{
			if (mCurrTri.GetID() == m.GetID())
			{
				alreadyIn = true;
				break;
			}
		}

		if (alreadyIn)
		{
			continue;
		}
		else if (Parallel(mCurrTri.GetNormal(), triNormal)) // Found another neighbour with the same normal
		{
			triPos.push_back(mCurrTri);
			FindAllNeighboursWithNormal(triPos, mCurrTri, triNormal); // Recursive
		}
	}
}

template <typename T>
int NavMeshBuilder::sign(const T& val)
{
	return (T(0) < val) - (val < T(0));
}

int NavMeshBuilder::IsTriangleOrientedClockWise(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& n)
{
	glm::vec3 w = cross((p1 - p2), (p3 - p2));

	if (glm::length(w) == 0.f)
	{
		return 0;
	}
	else if (sign(w.x) != sign(n.x) || sign(w.y) != sign(n.y) || sign(w.z) != sign(n.z))
	{
		return 1;
	}
	return -1;
}

bool NavMeshBuilder::PointsOnLine(std::vector<glm::vec3> points, const glm::vec3& n)
{
	for (int i = 0; i < points.size(); ++i)
	{
		glm::vec3 firstPoint = points[i - 1 < 0 ? points.size() - 1 : i - 1];
		glm::vec3 secondPoint = points[i];
		glm::vec3 thirdPoint = points[i + 1 == points.size() ? 0 : i + 1];

		if (IsTriangleOrientedClockWise(firstPoint, secondPoint, thirdPoint, n) != 0)
		{
			return false;
		}
	}
	return true;
}

bool NavMeshBuilder::Parallel(const glm::vec3& v1, const glm::vec3& v2)
{
	glm::vec3 test = glm::cross(v1, v2);
	if ((test.x == 0) && (test.y == 0) && (test.z == 0))
	{
		return true;
	}
	return false;
}

void NavMeshBuilder::Exit()
{
	mHoles.clear();
	mObstacles.clear();
	mRegion.clear();

	delete mNavMesh;
}
