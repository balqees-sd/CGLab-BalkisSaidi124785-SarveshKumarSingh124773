#ifndef NODE_HPP
#define NODE_HPP

#include<string>
#include<list>
#include<glm/mat4x4.hpp>
#include<memory>

class Node : std::enable_shared_from_this<Node> {
	private:
		std::string name_;
		std::shared_ptr<Node> parent_;
		std::list<std::shared_ptr<Node>> children_;

		std::string path_;
		int depth_ = 0;
		glm::mat4x4 localTransform_;
		glm::mat4x4 worldTransform_;
	public:
		Node(std::string name);
		Node(std::string name, std::shared_ptr<Node> parent);
		Node(std::string name, std::shared_ptr<Node> parent, glm::mat4x4 localTransform);
		~Node();
		std::shared_ptr<Node> getParent();
		void setParent(std::shared_ptr<Node> parent);
		std::shared_ptr<Node> getChildren(std::string name);
		std::list<std::shared_ptr<Node>> getChildren();
		std::string getName();
		std::string getPath();
		int getDepth();
		glm::mat4x4 getLocalTransform();
		void setLocalTransform(glm::mat4x4 localTransform);
		glm::mat4x4 getWorldTransform();
		void setWorldTransform(glm::mat4x4 worldTransform);
		void addChildren(std::shared_ptr<Node> child);
		std::shared_ptr<Node> removeChildren(std::string name);
		std::string printChildren();
};

#endif