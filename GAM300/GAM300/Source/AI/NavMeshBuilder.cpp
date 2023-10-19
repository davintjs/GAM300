/*!***************************************************************************************
\file			NavMeshBuilder.cpp
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definitions of the following:
	1. NavMeshBuilder singleton class
		a. Builds the navmesh given the ground vertices and indices
		b. Add holes and obstacles to the navmesh
		c. Triangulation by ear clipping method

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"

#include "NavMeshBuilder.h"
#include "NavMesh.h"
#include "Scene/SceneManager.h"

void NavMeshBuilder::Init()
{
	EVENTS.Subscribe(this, &NavMeshBuilder::CallbackContactAdd); // For obstacle contact with floor
	EVENTS.Subscribe(this, &NavMeshBuilder::CallbackContactRemove); // For obstacle removed from floor
}

void NavMeshBuilder::BuildNavMesh()
{
	std::pair<std::vector<glm::vec3>, std::vector<glm::ivec3>> mResPair = GetAllGrounds();
	std::vector<Triangle3D> GroundTriangles = GetGroundTriangles(mResPair.first, mResPair.second);
	mRegion = ComputeRegions(GroundTriangles); // Compute the regions of the given ground
	OffsetRadius(0.2f); // Offset to account for the agent radius

	mNavMesh = CreateNavMesh(); // Create the navmesh
	mNavMesh->LinkAllTriangles();
}

std::pair<std::vector<glm::vec3>, std::vector<glm::ivec3>> NavMeshBuilder::GetAllGrounds()
{
	std::vector<glm::vec3> mGroundVertices;
	std::vector<glm::ivec3> mGroundIndices;

	for (auto& entity : MySceneManager.GetCurrentScene().GetArray<Entity>())
	{
		Tag& mTag = MySceneManager.GetCurrentScene().Get<Tag>(entity);
		if (mTag.physicsLayerIndex != 5)
		{
			continue;
		}

		const Transform& t = MySceneManager.GetCurrentScene().Get<Transform>(entity);
		const MeshFilter& mesh = MySceneManager.GetCurrentScene().Get<MeshFilter>(entity);

		for (int i = 0; i < mesh.vertices->size(); ++i)
		{
			glm::vec4 temp = glm::vec4((*mesh.vertices)[i], 1.f);
			mGroundVertices.push_back(static_cast<glm::vec3>(t.GetWorldMatrix() * temp));
		}
		for (int j = 0; j < mesh.indices->size(); j += 3)
		{
			mGroundIndices.push_back(glm::ivec3((*mesh.indices)[j], (*mesh.indices)[j + 1], (*mesh.indices)[j + 2]));
		}
	}

	return std::make_pair(mGroundVertices, mGroundIndices);
}

NavMesh* NavMeshBuilder::CreateNavMesh()
{
	ObstacleOffset(0.2f); // Offset the diameter first before removing
	RemoveObstaclesFromMesh(); // Remove holes in the boundary
	NavMesh* _NavMesh = new NavMesh(Triangulate());

	return _NavMesh;
}

void NavMeshBuilder::Rebake()
{
	NAVMESHBUILDER.Exit(); // Clear current NavMesh
	NAVMESHBUILDER.BuildNavMesh(); // Rebuild NavMesh
}

std::vector<Polygon3D>& NavMeshBuilder::GetRegion()
{
	return mRegion;
}

//std::vector<Polygon3D>& NavMeshBuilder::GetHoles()
//{
//	return mHoles;
//}

std::vector<Polygon3D>& NavMeshBuilder::GetObstacles()
{
	return mObstacles;
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

	// {0, 1, 2} and {2, 3, 0}
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

void NavMeshBuilder::OffsetRadius(const float& mRadius)
{
	int i = 0;
	int regionTracker = 0;
	while (regionTracker < mRegion.size())
	{
		float zValue = mRegion[regionTracker].GetBarycenter().z; // Finding the barycenter of connected regions by checking their z value matches
		glm::vec3 baryCenter(0.f, 0.f, 0.f);
		baryCenter += mRegion[regionTracker].GetBarycenter();

		++regionTracker;
		for (; regionTracker < mRegion.size(); ++regionTracker)
		{
			if (mRegion[regionTracker].GetBarycenter().z == zValue)
			{
				baryCenter += mRegion[regionTracker].GetBarycenter();
			}
			else // Different region z value, different barycenter
			{
				break;
			}
		}

		// Reaching here, we have found the barycenter of connected regions
		std::vector<glm::vec3> mVertsOffsetted;

		for (; i < regionTracker - 1; ++i)
		{
			Polygon3D& mCurrentRegion = mRegion[i];
			Polygon3D& mNextRegion = mRegion[i + 1];

			// Find current region and next region's shared points
			std::vector<int> mSharedPointsCurrent;
			std::vector<int> mSharedPointsNext;
			for (int j = 0; j < mCurrentRegion.GetPoints().size(); ++j)
			{
				glm::vec3 mCurrentPoint = mCurrentRegion.GetPoints()[j];

				// If already offsetted, skip this point
				if (std::find(mVertsOffsetted.begin(), mVertsOffsetted.end(), mCurrentPoint) != mVertsOffsetted.end())
				{
					continue;
				}

				for (int k = 0; k < mNextRegion.GetPoints().size(); ++k)
				{
					glm::vec3 mNextPoint = mNextRegion.GetPoints()[k];
					if (mCurrentPoint == mNextPoint)
					{
						// Found a shared point
						mSharedPointsCurrent.push_back(j); // Store this shared point of the current region
						mSharedPointsNext.push_back(k); // Store this shared point of the next region

						break; // Go to next mCurrentPoint
					}
				}
			}

			// Move non-shared points to offset diameter with the region's barycenter
			for (int m = 0; m < mCurrentRegion.GetPoints().size(); ++m)
			{
				glm::vec3& _point = mCurrentRegion.GetPoints()[m];
				if (std::find(mSharedPointsCurrent.begin(), mSharedPointsCurrent.end(), m) == mSharedPointsCurrent.end() // Means this point is not shared
					&& std::find(mVertsOffsetted.begin(), mVertsOffsetted.end(), _point) == mVertsOffsetted.end()) // Means this point is not offsetted yet 
				{
					// Find length of v from barycenter to this point
					glm::vec3 v = _point - baryCenter;
					float vLength = glm::length(v);

					// Subtract the diameter from vLength
					float mDiameterLength = sqrt(mRadius * mRadius + mRadius * mRadius);
					float newLength = vLength - mDiameterLength;

					// Normalize v
					glm::vec3 vNormalized = glm::normalize(v);

					// newPoint = barycenter + newLength * vNormalized (Parametric equation)
					_point = baryCenter + newLength * vNormalized;

					mVertsOffsetted.push_back(_point);
				}
			}

			// Move shared points to offset diameter (Move first to last and last to first)

			// Current region
			// First shared point
			glm::vec3& _firstSharedPoint = mCurrentRegion.GetPoints()[mSharedPointsCurrent[0]];
			const glm::vec3& _firstSharedPointNext = mCurrentRegion.GetPoints()[mSharedPointsCurrent[1]];

			glm::vec3 _vFirstShared = _firstSharedPoint - _firstSharedPointNext;
			float _vFirstSharedLength = glm::length(_vFirstShared);
			float _vNewFirstSharedLength = _vFirstSharedLength - mRadius;
			glm::vec3 _vFirstSharedNormalized = glm::normalize(_vFirstShared);
			_firstSharedPoint = _firstSharedPointNext + _vNewFirstSharedLength * _vFirstSharedNormalized;

			// Last shared point
			glm::vec3& _lastSharedPoint = mCurrentRegion.GetPoints()[mSharedPointsCurrent[mSharedPointsCurrent.size() - 1]];
			const glm::vec3& _lastSharedPointPrev = mCurrentRegion.GetPoints()[mSharedPointsCurrent[mSharedPointsCurrent.size() - 2]];

			glm::vec3 _vLastShared = _lastSharedPoint - _lastSharedPointPrev;
			float _vLastSharedLength = glm::length(_vLastShared);
			float _vNewLastSharedLength = _vLastSharedLength - mRadius;
			glm::vec3 _vLastSharedNormalized = glm::normalize(_vLastShared);
			_lastSharedPoint = _lastSharedPointPrev + _vNewLastSharedLength * _vLastSharedNormalized;

			// Next region just need to set first and last to already calculated values from current
			mNextRegion.GetPoints()[mSharedPointsNext[0]] = _firstSharedPoint;
			mNextRegion.GetPoints()[mSharedPointsNext[mSharedPointsNext.size() - 1]] = _lastSharedPoint;

			for (int n = 0; n < mSharedPointsNext.size(); ++n) // Add the rest of the shared point to offsetted
			{
				mVertsOffsetted.push_back(mNextRegion.GetPoints()[mSharedPointsNext[n]]);
			}
		}

		// Account for the last region
		Polygon3D& mLastRegion = mRegion[mRegion.size() - 1];

		for (int a = 0; a < mLastRegion.GetPoints().size(); ++a)
		{
			glm::vec3& _point = mLastRegion.GetPoints()[a];
			if (std::find(mVertsOffsetted.begin(), mVertsOffsetted.end(), _point) == mVertsOffsetted.end())
			{
				// Find length of v from barycenter to this point
				glm::vec3 v = _point - baryCenter;
				float vLength = glm::length(v);

				// Subtract the diameter from vLength
				float mDiameterLength = sqrt(mRadius * mRadius + mRadius * mRadius);
				float newLength = vLength - mDiameterLength;

				// Normalize v
				glm::vec3 vNormalized = glm::normalize(v);

				// newPoint = barycenter + newLength * vNormalized (Parametric equation)
				_point = baryCenter + newLength * vNormalized;

				mVertsOffsetted.push_back(_point); // Add this offsetted vertex to the vertex for checking
			}
		}
	}
}

void NavMeshBuilder::ObstacleOffset(const float& mRadius)
{
	for (auto& obstacle : mObstacles)
	{
		std::vector<glm::vec3> mHoldVec;
		for (int i = 0; i < obstacle.GetPoints().size(); ++i)
		{
			// Get our 3 points for a triangle
			glm::vec3 firstPoint = obstacle.GetPoints()[i - 1 < 0 ? obstacle.GetPoints().size() - 1 : i - 1];
			glm::vec3 secondPoint = obstacle.GetPoints()[i];
			glm::vec3 thirdPoint = obstacle.GetPoints()[i + 1 == obstacle.GetPoints().size() ? 0 : i + 1];

			// Find the midpoint of triangle's opposite side of second point
			glm::vec3 midPoint = (firstPoint + thirdPoint) / 2.f;

			// Get the vector from the midpoint to our second point
			glm::vec3 mVector = secondPoint - midPoint;

			// Extrapolate with the agent radius
			float mRadiusLength = sqrt(mRadius * mRadius + mRadius * mRadius);
			float mVectorCurrLength = glm::length(mVector);
			float mVectorNewLength = mVectorCurrLength + mRadiusLength;

			// Set new point of this corner of hole with parametric equation
			glm::vec3 mNormalizedVec = glm::normalize(mVector);
			mHoldVec.push_back(midPoint + mVectorNewLength * mNormalizedVec);
		}

		for (int j = 0; j < obstacle.GetPoints().size(); ++j)
		{
			obstacle.GetPoints()[j] = mHoldVec[j];
		}
	}
}

void NavMeshBuilder::AddObstacle(Polygon3D* mObstacle)
{
	mObstacles.push_back(*mObstacle);
}

void NavMeshBuilder::RemoveObstaclesFromMesh()
{
	// Collect all obstacles in the current scene here first


	std::sort(mObstacles.begin(), mObstacles.end(), [](Polygon3D& p1, Polygon3D& p2)
		{
			return p1.GetMaxPoint().y > p2.GetMaxPoint().y;
		});

	for (auto& polygon : mRegion)
	{
		for (auto& obstacle : mObstacles)
		{
			if (polygon.HoleInPolygon(obstacle)) // Check if obstacle is in this polygon
			{
				// Switch the orientation of the obstacle
				if (polygon.GetOrientation() == obstacle.GetOrientation())
				{
					obstacle.SwitchOrientation();
				}
				
				polygon.JoinPolygon(obstacle); // Add the obstacle into the polygon
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

		polygon.CalculateNormalBarycenter(vertices);

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

					for (int k = 0; k < vertices.size(); k++)
					{
						if (vertices[k] == secondPoint)
						{
							vertices.erase(vertices.begin() + k);
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
	//mHoles.clear();
	mObstacles.clear();
	mRegion.clear();

	delete mNavMesh;
}

void NavMeshBuilder::CallbackContactAdd(ContactAddedEvent* pEvent)
{
	Tag mTagRb1 = MySceneManager.GetCurrentScene().Get<Tag>(pEvent->rb1->EUID());
	Tag mTagRb2 = MySceneManager.GetCurrentScene().Get<Tag>(pEvent->rb2->EUID());

	Tag* fNavMesh;
	Tag* fObstacle;

	if (mTagRb1.physicsLayerIndex == 5 && mTagRb2.physicsLayerIndex == 6)  // Navmesh, obstacle
	{
		fNavMesh = &mTagRb1;
		fObstacle = &mTagRb2;
	}
	else if (mTagRb1.physicsLayerIndex == 6 && mTagRb2.physicsLayerIndex == 5) // Obstacle, navmesh
	{
		fObstacle = &mTagRb1;
		fNavMesh = &mTagRb2;
	}
	else
	{
		return;
	}

	glm::vec3 obstacleMidPoint = MySceneManager.GetCurrentScene().Get<Transform>(*fObstacle).translation;

	glm::vec3 obstacleBottomFace = obstacleMidPoint;
	glm::vec3 scaledVec = { MySceneManager.GetCurrentScene().Get<BoxCollider>(*fObstacle).x,
							MySceneManager.GetCurrentScene().Get<BoxCollider>(*fObstacle).y,
							MySceneManager.GetCurrentScene().Get<BoxCollider>(*fObstacle).z };
	scaledVec.x *= MySceneManager.GetCurrentScene().Get<Transform>(*fObstacle).scale.x;
	scaledVec.y *= MySceneManager.GetCurrentScene().Get<Transform>(*fObstacle).scale.y;
	scaledVec.z *= MySceneManager.GetCurrentScene().Get<Transform>(*fObstacle).scale.z;

	obstacleBottomFace.y = MySceneManager.GetCurrentScene().Get<BoxCollider>(*fNavMesh).y * MySceneManager.GetCurrentScene().Get<Transform>(*fNavMesh).scale.y / 2.f;

	glm::vec3 obstacleMinPoint = { obstacleBottomFace.x - (scaledVec.x / 2.f), obstacleBottomFace.y, obstacleBottomFace.z - (scaledVec.z / 2.f) };
	glm::vec3 obstacleMinPointTop = { obstacleBottomFace.x - (scaledVec.x / 2.f), obstacleBottomFace.y, obstacleBottomFace.z + (scaledVec.z / 2.f) };
	glm::vec3 obstacleMaxPoint = { obstacleBottomFace.x + (scaledVec.x / 2.f), obstacleBottomFace.y, obstacleBottomFace.z + (scaledVec.z / 2.f) };
	glm::vec3 obstacleMaxPointBottom = { obstacleBottomFace.x + (scaledVec.x / 2.f), obstacleBottomFace.y, obstacleBottomFace.z - (scaledVec.z / 2.f) };

	// Counter-clockwise
	std::vector<glm::vec3> obstaclePosition;
	obstaclePosition.push_back(obstacleMinPoint);
	obstaclePosition.push_back(obstacleMaxPointBottom);
	obstaclePosition.push_back(obstacleMaxPoint);
	obstaclePosition.push_back(obstacleMinPointTop);

	std::vector<Polygon3D> obstacleHolder = mObstacles;
	Exit();
	obstacleHolder.push_back(Polygon3D(obstaclePosition));
	mObstacles = obstacleHolder;
	BuildNavMesh();
}

void NavMeshBuilder::CallbackContactRemove(ContactRemovedEvent* pEvent)
{



}