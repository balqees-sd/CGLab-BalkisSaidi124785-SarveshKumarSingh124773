#include "scene_graph.hpp"

bool SceneGraph::instanceFlag = false;
SceneGraph* SceneGraph::instance = NULL;

std::ostream& operator << (std::ostream& out, std::shared_ptr<Node> node) {

	return out;
}

SceneGraph::SceneGraph(){
	name_ = "SceneGraph_Solar";
	root_ = std::make_shared<Node>("root");
}

SceneGraph* SceneGraph::getInstance() {
	if (!instanceFlag) {
		instance = new SceneGraph();
		instanceFlag = true;
		return instance;
	} else {
		return instance;
	}
}

//TODO delete nodes?
SceneGraph::~SceneGraph() {
	instanceFlag = false;
}

std::string SceneGraph::getName() {
	return name_;
}

std::shared_ptr<Node> SceneGraph::getRoot() {
	return root_;
}

//TODO implement print out
std::string SceneGraph::printGraph() {
	std::string result = "root ";
	for (auto child : root_->getChildren()) {
		result = result + child->getName() + " ";
		result = result + child->printChildren();
	};
	return result;
}

void SceneGraph::setName(std::string name) {
	name_ = name;
}

void SceneGraph::setRoot(std::shared_ptr<Node> root) {
	root_ = root;
}