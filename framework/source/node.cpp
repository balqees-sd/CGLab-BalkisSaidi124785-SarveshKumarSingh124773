#include "node.hpp"

Node::~Node() {}

Node::Node(std::string name) :
	name_(name),
	parent_(nullptr),
	path_("/"+name)
{
	depth_ = 0;
}

// constructor and implicitly appending to node
Node::Node(std::string name, std::shared_ptr<Node> parent): 
	name_(name), 
	parent_(parent)
{
	path_ = getPath();
	depth_ = parent->getDepth() + 1;
}

// constructor, appending to node and set local transform
Node::Node(std::string name, std::shared_ptr<Node> parent, glm::mat4x4 localTransform) : 
	name_(name), 
	parent_(parent), 
	localTransform_(localTransform)
{
	depth_ = parent->getDepth() + 1;
	path_ = getPath();
}

std::shared_ptr<Node> Node::getParent()
{
	return parent_;
}

// append node to another node
void Node::setParent(std::shared_ptr<Node> parent)
{
	parent_ = parent;
	depth_ = parent_->getDepth() + 1;
	path_ = parent_->getPath() + "/" + name_;
}

// recursively gather all childrens from given node; returns the node their children
std::shared_ptr<Node> Node::getChildren(std::string name)
{
	for (auto child : children_) {
		if (child->name_ == name) {
			return child;
		} else {
			auto result = child->getChildren(name);
			if (result != nullptr) {
				return child->getChildren(name);
			}
		}
	}
	return nullptr;
}

std::list<std::shared_ptr<Node>> Node::getChildren()
{
	return children_;
}

std::string Node::getName()
{
	return name_;
}

// returns path by recursively call getPath from parent
std::string Node::getPath()
{
	if (parent_ == nullptr)
		return path_;
	else {
		path_ = parent_->getPath() + path_;
		return path_;
	}
}

// returns depth by recursively call getDepth from parent
int Node::getDepth()
{
	if (parent_ == nullptr)
		return 0;
	else {
		depth_ = parent_->getDepth() + 1;
		return depth_;
	}
}

glm::mat4x4 Node::getLocalTransform()
{
	return localTransform_;
}

void Node::setLocalTransform(glm::mat4x4 localTransform)
{
	localTransform_ = localTransform;
}

// calculates the worldtransform by multiplying the local transforms from root to leave
glm::mat4x4 Node::getWorldTransform()
{
	if (parent_ == nullptr)
		return localTransform_;
	else {
		return parent_->getWorldTransform()*localTransform_;
	}
}

void Node::setWorldTransform(glm::mat4x4 worldTransform)
{
	worldTransform_ = worldTransform;
}

void Node::addChildren(std::shared_ptr<Node> child)
{
	children_.push_back(child);
}

// uses getChildren to remove the node
std::shared_ptr<Node> Node::removeChildren(std::string name)
{
	std::shared_ptr<Node> result = this->getChildren(name);
	if (result != nullptr) {
		std::shared_ptr<Node> parent = result->getParent();
		if (parent != nullptr) {
			parent->children_.remove(result);
		}
	}
	return result;
}

std::string Node::printChildren() {
	std::string result = "";
	for (auto child : children_) {
		result = result + child->getName() + " ";
		result = result + child->printChildren();
	};
	return result;
}
