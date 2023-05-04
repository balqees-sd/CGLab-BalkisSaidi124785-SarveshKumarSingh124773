#include "camera_node.hpp"

CameraNode::CameraNode(std::string name, bool isPerspective, bool isEnabled, glm::mat4x4 projectionMatrix) :
	Node(name),
	isPerspective_(isPerspective),
	isEnabled_(isEnabled),
	projectionMatrix_(projectionMatrix)
{}

CameraNode::CameraNode(std::string name, std::shared_ptr<Node> parent, bool isPerspective, bool isEnabled, glm::mat4x4 projectionMatrix):
	Node(name,parent),
	isPerspective_(isPerspective),
	isEnabled_(isEnabled),
	projectionMatrix_(projectionMatrix)
{}

CameraNode::CameraNode(std::string name, std::shared_ptr<Node> parent, glm::mat4x4 localTransform, bool isPerspective, bool isEnabled, glm::mat4x4 projectionMatrix):
	Node(name,parent,localTransform),
	isPerspective_(isPerspective),
	isEnabled_(isEnabled),
	projectionMatrix_(projectionMatrix)
{}

bool CameraNode::getPerspective()
{
	return isPerspective_;
}

bool CameraNode::getEnabled()
{
	return isEnabled_;
}

void CameraNode::setEnabled(bool enabled)
{
	isEnabled_ = enabled;
}

glm::mat4x4 CameraNode::getProjectionMatrix()
{
	return projectionMatrix_;
}

void CameraNode::setProjectionMatrix(glm::mat4x4 projectionMatrix)
{
	projectionMatrix_ = projectionMatrix;
}
