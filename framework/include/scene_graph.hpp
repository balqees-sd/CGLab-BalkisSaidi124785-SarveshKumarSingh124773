#ifndef SCENE_GRAPH_HPP
#define SCENE_GRAPH_HPP
#include <string>
#include "node.hpp"
#include "geometry_node.hpp"
#include "camera_node.hpp"
#include "light_node.hpp"
#include <iostream>

class SceneGraph {
	private: 
		std::string name_;
		std::shared_ptr<Node> root_;
		static bool instanceFlag;
		static SceneGraph* instance;
		SceneGraph();
		void setName(std::string);
		void setRoot(std::shared_ptr<Node>);

	public:
		static SceneGraph* getInstance();
		~SceneGraph();
		std::string getName();
		std::shared_ptr<Node> getRoot();
		std::string printGraph();
};

#endif