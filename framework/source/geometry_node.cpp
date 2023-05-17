#include "geometry_node.hpp"

GeometryNode::GeometryNode(std::string name, model geometry) :
	Node(name), geometry_(geometry)
{
}

GeometryNode::GeometryNode(std::string name, std::shared_ptr<Node> parent, model geometry) :
	Node(name, parent), geometry_(geometry)
{
}

GeometryNode::GeometryNode(std::string name, std::shared_ptr<Node> parent, glm::mat4x4 localTransform, model geometry) :
	Node(name, parent, localTransform), geometry_(geometry)
{
}

model GeometryNode::getGeometry()
{
	return geometry_;
}

void GeometryNode::setGeometry(model geometry)
{	
	geometry_ = geometry;
}
