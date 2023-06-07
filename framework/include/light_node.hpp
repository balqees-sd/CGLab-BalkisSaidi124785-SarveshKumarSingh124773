#ifndef LIGHT_NODE_HPP
#define LIGHT_NODE_HPP

#include"node.hpp"
#include"model.hpp"
#include"color.hpp"

class LightNode : public Node {
	private:
		double lightIntensity_;
		color lightColor_;
	public:
		LightNode(std::string name, 
			double lightIntensity, 
			color lightClr);
		LightNode(std::string name, 
			std::shared_ptr<Node> parent, 
			double lightIntensity, 
			color lightClr);
		LightNode(std::string name, 
			std::shared_ptr<Node> parent, 
			glm::mat4x4 localTransform, 
			double lightIntensity, 
			color lightClr);
		color getLightColor();
		void setLightColor(color lightClr);
		double getLightIntensity();
		void setLightIntensity(double lightIntensity);
};

#endif // !LIGHT_NODE_HPP
