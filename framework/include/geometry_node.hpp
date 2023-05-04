#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include<node.hpp>
#include<model.hpp>

class GeometryNode : public Node {
	private:
		model geometry_;
	public:
		GeometryNode(std::string name, model geometry);
		GeometryNode(std::string name, std::shared_ptr<Node> parent, model geometry);
		GeometryNode(std::string name, std::shared_ptr<Node> parent, glm::mat4x4 localTransform, model geometry);
		model getGeometry();
		void setGeometry(model geometry);
};

#endif // !GEOMETRY_NODE_HPP
