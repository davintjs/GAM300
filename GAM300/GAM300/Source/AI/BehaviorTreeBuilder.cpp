#include "Precompiled.h"
#include "BehaviorTreeBuilder.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include <filesystem>

void BehaviorTreeBuilder::Init()
{
	BuildTrees();
}

void BehaviorTreeBuilder::Update()
{

}

void BehaviorTreeBuilder::Exit()
{

}

void BehaviorTreeBuilder::BuildTrees()
{
	const std::filesystem::path treesFile("GAM300/Source/AI/Trees");
	if (!std::filesystem::exists(treesFile))
	{
		std::cout << "Behavior tree file does not exist! Check if proper filepath is entered." << std::endl;
		exit(0);
	}

	for (const auto& dir : std::filesystem::recursive_directory_iterator(treesFile))
	{
		std::string filePath = dir.path().generic_string();
		std::string fileName;

		// Getting the file name for behavior tree name
		for (size_t i = filePath.find_last_of('/') + 1; i != filePath.find_last_of('.'); ++i)
		{
			fileName += filePath[i];
		}

		// Reading the file data here to build the tree and store into our vector of trees
		std::ifstream ifs(filePath);
		if (!ifs)
		{
			std::cout << "Error opening behavior tree file to read data!" << std::endl;
			exit(0);
		}

		std::stringstream buffer;
		buffer << ifs.rdbuf();
		ifs.close();

		rapidjson::Document doc;
		const std::string data(buffer.str());
		doc.Parse(data.c_str());

		// Root node
		const rapidjson::Value& tree = doc[fileName.c_str()][0]; // We can put 0 here because there can only be 1 root node

		std::string rootName = tree["NodeName"].GetString();
		std::string rootType = tree["NodeType"].GetString();
		int rootDepth = tree["NodeDepth"].GetInt();

		// Deserialize the root node first


		// Deserialize the children nodes
		const rapidjson::Value& _children = tree["Children"];

		for (int i = 0; i < _children.Size(); ++i)
		{
			// Recursively build tree here from the root node
		}


		//MoveToPosition* move = new MoveToPosition("Move");
		//Inverter* notNear = new Inverter(move);
		//Sequencer* sequence = new Sequencer();

		//sequence->addChild(notNear);

		//BehaviorTree* tempTree = new BehaviorTree("Temp tree", sequence);
		
	}
}