#include "light_node.hpp"

LightNode::LightNode(std::string name,  
	double strength, 
	color clr) :
	Node(name), strength_(strength), color_(clr)
{
}

LightNode::LightNode(std::string name, 
	std::shared_ptr<Node> parent, 
	double strength,
	color clr) :
	Node(name, parent), strength_(strength), color_(clr)
{
}

LightNode::LightNode(std::string name,
	std::shared_ptr<Node> parent,
	glm::mat4x4 localTransform,
	double strength,
	color clr):
	Node(name, parent, localTransform), strength_(strength), color_(clr)
{
}

color LightNode::getColor() {
	return color_;
}

void LightNode::setColor(color clr) {
	color_ = clr;
}

double LightNode::getStrength() {
	return strength_;
}

void LightNode::setStrength(double strength) {
	strength_ = strength;
}
