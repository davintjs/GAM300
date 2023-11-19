/*!***************************************************************************************
\file			Pathfinder.cpp
\project
\author         Davin Tan

\par			Course: GAM300
\date           19/11/2023

\brief
	This file contains the definitions of the following:
	1. PathNode class
		a. A node of the navmesh generated for AI to find its waypoint
	2. AStarPather class
		a. A* pathfinding for AI to find its waypoint to goal with portaling and funneling
		b. Calculates heuristic using euclidean method
		c. Helper functions

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "Geometry.h"

bool PathNode::operator<(const PathNode& rhs)
{
	return this->mFinalCost < rhs.mFinalCost;
}

bool AStarPather::ComputePath(const Triangle3D*& mStart, const Triangle3D*& mEnd)
{
	while (!mQueue.empty())
	{
		Triangle3D* mParentNode = mQueue.top();

		// Reached the end
		if (mParentNode->GetMidPoint() == mEnd->GetMidPoint())
		{
			while (mParentNode != nullptr)
			{
				mTriangleWayPoint.insert(mTriangleWayPoint.begin(), mParentNode);
				mParentNode = mParentNode->GetParent();
			}
			return true;
		}

		// Have not reached the end
		mParentNode->SetList(OnList::CLOSED_LIST);
		for (Triangle3D* tri : mParentNode->GetNeighbours())
		{
			float mNewGiven = std::sqrtf(static_cast<float>(std::pow((tri->GetMidPoint().x - mParentNode->GetMidPoint().x), 2)) +
										 static_cast<float>(std::pow((tri->GetMidPoint().y - mParentNode->GetMidPoint().y), 2)) +
										 static_cast<float>(std::pow((tri->GetMidPoint().z + mParentNode->GetMidPoint().z), 2))) +
							  mParentNode->GetGivenCost();

			float mNewHeu = CalculateHeuristic(tri, mEnd);
			float mNewFinal = mNewGiven + mNewHeu;
			if (tri->GetList() != OnList::OPEN_LIST && tri->GetList() != OnList::CLOSED_LIST)
			{
				tri->SetParent(mParentNode);
				tri->SetFinalCost(mNewFinal);
				tri->SetHeuCost(mNewHeu);
				tri->SetGivenCost(mNewGiven);
				tri->SetList(OnList::OPEN_LIST);
				mVisitedNodes.push_back(tri);
				mQueue.push(tri);
			}
			else if (tri->GetFinalCost() > mNewFinal)
			{
				tri->SetParent(mParentNode);
				tri->SetFinalCost(mNewFinal);
				tri->SetHeuCost(mNewHeu);
				tri->SetGivenCost(mNewGiven);
				if (tri->GetList() == OnList::CLOSED_LIST)
				{
					tri->SetList(OnList::OPEN_LIST);
					mQueue.push(tri);
				}
				mVisitedNodes.push_back(tri);
			}
		}

		mQueue.pop();
	}

	return false;
}

std::vector<glm::vec3> AStarPather::PathPostProcess(const glm::vec3& mStart, const glm::vec3& mEnd)
{
	std::vector<std::pair<glm::vec3, glm::vec3>> portals = GetPortals(mStart, mEnd);
	std::vector<glm::vec3> mWay = Funnel(mStart, mEnd, portals); // The vector will contain the points that the agent will need to walk to until he reach the goal

	return mWay;
}

float AStarPather::CalculateHeuristic(const Triangle3D* mCurrNode, const Triangle3D*& mEnd)
{
	return std::sqrtf(static_cast<float>(std::pow(mCurrNode->GetMidPoint().x - mEnd->GetMidPoint().x, 2)) +
					  static_cast<float>(std::pow(mCurrNode->GetMidPoint().y - mEnd->GetMidPoint().y, 2)) +
					  static_cast<float>(std::pow(mCurrNode->GetMidPoint().z - mEnd->GetMidPoint().z, 2)));
}

std::vector<std::pair<glm::vec3, glm::vec3>> AStarPather::GetPortals(const glm::vec3& mStart, const glm::vec3& mEnd)
{
	std::vector<std::pair<glm::vec3, glm::vec3>> resVec;

	// First determine which is the left and right point of the first portal
	const Triangle3D& firstTri = *(this->mTriangleWayPoint[0]);
	const Triangle3D& secondTri = *(this->mTriangleWayPoint[1]);
	std::vector<glm::vec3> firstPortal;
	for (int j = 0; j < 3; ++j)
	{
		const glm::vec3& currPoint = firstTri[j];
		if (secondTri[0] == currPoint || secondTri[1] == currPoint || secondTri[2] == currPoint)
		{
			firstPortal.push_back(currPoint);
		}
	}

	// Form a line between the start point and one of the portal edges
	// If the other portal edge lies to the right of this line, it is right side of the channel
	Line3D mFirstLine(firstPortal[0], firstPortal[1] - firstPortal[0]);
	if (IsAPointLeftOfVectorOrOnTheLine(mStart, firstPortal[0], firstPortal[1]) > 0)
	{
		// firstPortal[1] is on the left of the line
		resVec.push_back(std::make_pair(firstPortal[1], firstPortal[0]));
	}
	else
	{
		// firstPortal[0] is on the left of the line
		resVec.push_back(std::make_pair(firstPortal[0], firstPortal[1]));
	}

	int previousPortalIndex = 0;
	// For all other portals, compare to the previous portal to determine left or right since they all share a point
	for (int i = 1; i < this->mTriangleWayPoint.size() - 1; ++i)
	{
		const Triangle3D& currTri = *(this->mTriangleWayPoint[i]);
		const Triangle3D& nextTri = *(this->mTriangleWayPoint[i + 1]);

		// Find the 2 shared points between these 2 triangles
		std::vector<glm::vec3> currPortal;
		int count = 0;
		for (int j = 0; j < 3; ++j)
		{
			const glm::vec3& currPoint = currTri[j];
			if (nextTri[0] == currPoint || nextTri[1] == currPoint || nextTri[2] == currPoint)
			{
				currPortal.push_back(currTri[j]);
				++count;
			}
			if (count == 2)
			{
				break;
			}
		}

		// Reaching here means we have the 2 points of our next portal
		// Sort them based on left or right
		if (currPortal[1] != resVec[previousPortalIndex].first && currPortal[1] != resVec[previousPortalIndex].second)
		{
			// Means currPortal[0] is the shared point with the previous portal
			if (currPortal[0] == resVec[previousPortalIndex].first)
			{
				resVec.push_back(std::make_pair(currPortal[0], currPortal[1]));
			}
			else
			{
				resVec.push_back(std::make_pair(currPortal[1], currPortal[0]));
			}
		}
		else
		{
			// Means currPortal[1] is the shared point with the previous portal
			if (currPortal[1] == resVec[previousPortalIndex].first)
			{
				resVec.push_back(std::make_pair(currPortal[1], currPortal[0]));
			}
			else
			{
				resVec.push_back(std::make_pair(currPortal[0], currPortal[1]));
			}
		}
		++previousPortalIndex; // Increment the previous portal tracker index
	}

	// Insert the goal point as a zero-sized portal
	resVec.push_back(std::make_pair(mEnd, mEnd));

	return resVec;
}

std::vector<glm::vec3> AStarPather::Funnel(const glm::vec3& mStart, const glm::vec3& mEnd, const std::vector<std::pair<glm::vec3, glm::vec3>>& mPortals)
{
	std::vector<glm::vec3> resVec;

	// Create the initial left and right lines of funnel
	glm::vec3 mCurrStart = mStart;
	Segment3D leftLine(mStart, mPortals[0].first);
	Segment3D rightLine(mStart, mPortals[0].second);

	int apexIndex = 0, leftIndex = 0, rightIndex = 0, mNumPoints = 0;

	resVec.push_back(mStart); // Add the start point first
	++mNumPoints;

	for (int i = 1; i < mPortals.size() && mNumPoints < mPortals.size(); ++i)
	{
		const glm::vec3& leftPoint = mPortals[i].first;
		const glm::vec3& rightPoint = mPortals[i].second;

		if (TriangleArea(mCurrStart, rightLine.point2, rightPoint) <= 0.f) // Update right vertex
		{
			if (mCurrStart == rightLine.point2 || TriangleArea(mCurrStart, leftLine.point2, rightPoint) > 0.f)
			{
				// Tighten the funnel
				rightLine.point2 = rightPoint;
				rightIndex = i;
			}
			else
			{
				// Right overlaps left, insert left to path and restart scan from portal left point
				resVec.push_back(leftLine.point2);
				++mNumPoints;

				// Make current left the new start
				mCurrStart = leftLine.point2;
				apexIndex = leftIndex;

				// Reset portal
				leftLine.point2 = mCurrStart;
				rightLine.point2 = mCurrStart;
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				i = apexIndex;

				continue;
			}
		}

		if (TriangleArea(mCurrStart, leftLine.point2, leftPoint) >= 0.f) // Update left vertex
		{
			if (mCurrStart == leftLine.point2 || TriangleArea(mCurrStart, rightLine.point2, leftPoint) < 0.f)
			{
				// Tighten the funnel
				leftLine.point2 = leftPoint;
				leftIndex = i;
			}
			else
			{
				// Left overlaps right, insert right to path and restart scan from portal right point
				resVec.push_back(rightLine.point2);
				++mNumPoints;

				// Make current right the new start
				mCurrStart = rightLine.point2;
				apexIndex = rightIndex;

				// Reset portal
				leftLine.point2 = mCurrStart;
				rightLine.point2 = mCurrStart;
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				i = apexIndex;

				continue;
			}
		}
	}

	// Append last point to path
	if (mNumPoints < mPortals.size())
	{
		resVec.push_back(mEnd);
	}

	return resVec;
}

float AStarPather::IsAPointLeftOfVectorOrOnTheLine(const glm::vec3 & l1, const glm::vec3 & l2, const glm::vec3 & p)
{
	return (l2.x - l1.x) * (p.y - l1.y) - (l2.y - l1.y) * (p.x - l1.x);
}

float AStarPather::TriangleArea(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	const float ax = p2[0] - p1[0];
	const float ay = p2[1] - p1[1];
	const float bx = p3[0] - p1[0];
	const float by = p3[1] - p1[1];

	return bx * ay - ax * by;
}