#ifndef CAMERA_NODE_HPP
#define CAMERA_NODE_HPP

#include "node.hpp"

class CameraNode : public Node {
	private:
		bool isPerspective_;
		bool isEnabled_;
		glm::mat4x4 projectionMatrix_;

	public:
		CameraNode(std::string name, bool isPerspective, bool isEnabled, glm::mat4x4 projectionMatrix);
		CameraNode(std::string name, std::shared_ptr<Node> parent, bool isPerspective, bool isEnabled, glm::mat4x4 projectionMatrix);
		CameraNode(std::string name, std::shared_ptr<Node> parent, glm::mat4x4 localTransform, bool isPerspective, bool isEnabled, glm::mat4x4 projectionMatrix);
		bool getPerspective();
		bool getEnabled();
		void setEnabled(bool enabled);
		glm::mat4x4 getProjectionMatrix();
		void setProjectionMatrix(glm::mat4x4 projectionMatrix);
};

#endif // !CAMERA_NODE_HPP
