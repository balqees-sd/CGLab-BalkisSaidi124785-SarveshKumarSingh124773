#include "light_node.hpp"

LightNode::LightNode(std::string name,  
	double lightIntensity, 
	color lightClr) :
	Node(name), lightIntensity_(lightIntensity), lightColor_(lightClr)
{
}

LightNode::LightNode(std::string name, 
	std::shared_ptr<Node> parent, 
	double lightIntensity,
	color lightClr) :
	Node(name, parent), lightIntensity_(lightIntensity), lightColor_(lightClr)
{
}

LightNode::LightNode(std::string name,
	std::shared_ptr<Node> parent,
	glm::mat4x4 localTransform,
	double lightIntensity,
	color lightClr):
	Node(name, parent, localTransform), lightIntensity_(lightIntensity), lightColor_(lightClr)
{
}

color LightNode::getLightColor() {
	return lightColor_;
}

void LightNode::setLightColor(color lightClr) {
	lightColor_ = lightClr;
}

double LightNode::getLightIntensity() {
	return lightIntensity_;
}

void LightNode::setLightIntensity(double lightIntensity) {
	lightIntensity_ = lightIntensity;
}
