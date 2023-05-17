#ifndef LIGHT_NODE_HPP
#define LIGHT_NODE_HPP

#include"node.hpp"
#include"model.hpp"
#include"color.hpp"

class LightNode : public Node {
	private:
		double strength_;
		color color_;
	public:
		LightNode(std::string name, 
			double strength, 
			color clr);
		LightNode(std::string name, 
			std::shared_ptr<Node> parent, 
			double strength, 
			color clr);
		LightNode(std::string name, 
			std::shared_ptr<Node> parent, 
			glm::mat4x4 localTransform, 
			double strength, 
			color clr);
		color getColor();
		void setColor(color clr);
		double getStrength();
		void setStrength(double strength);
};

#endif // !LIGHT_NODE_HPP
