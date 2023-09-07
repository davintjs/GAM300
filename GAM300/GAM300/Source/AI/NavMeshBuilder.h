#pragma once

#include "Recast.h"

struct NavMeshConfigs
{
	float mAgentHeight = 1.5f;
	float mAgentRadius = 0.3f;
	//float mAgentClimbHeight = 0.f;
	float mCellSize = 0.2f;
	float mCellHeight = 0.2f;
	//float mMaxEdgeLength = 4.0f;
	//float mMaxSimplificationError = 1.3f;
	//float mMinRegionSize = 3.0f;
	//float mRegionMergeSize = 20.0f;
	//float mDetailMeshSampleDistanceFactor = 1.0f;
	//float mDetailMeshSampleErrorFactor = 1.0f;
};

class NavMeshBuilder
{
public:
	NavMeshBuilder();
	~NavMeshBuilder();

	bool Build(const recastConfig)

private:

};