#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
{
	initializeSceneGraph();
	initializeGeometry();
	initializeShaderPrograms();
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

void ApplicationSolar::initializeSceneGraph() {
	//TODO is this the correct way???
	//How to set the root node of the SceneGraph?
	//What about the PointLightNode?

	// init singleton
	scenegraph_ = SceneGraph::getInstance();

	// call model_loader
	model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);
	auto root = scenegraph_->getRoot();

	// init all planets and holer with transformations as well as geometry
	auto point_light = std::make_shared<LightNode>("point_light", root, 1.0, color{ 10,10,10 });
	root->addChildren(point_light);
	auto sun_geom = std::make_shared<GeometryNode>("sun_geom", point_light, planet_model);
	point_light->addChildren(sun_geom);
	sun_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.5f,0.5f,0.5f }));

	auto earth_hold = std::make_shared<Node>("earth_hold", root);
	root->addChildren(earth_hold);
	earth_hold->setLocalTransform(glm::translate({}, glm::fvec3{1.3f,0.0f,0.0f}));
	auto earth_geom = std::make_shared<GeometryNode>("earth_geom", earth_hold, planet_model);
	earth_hold->addChildren(earth_geom);
	earth_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.1f,0.1f,0.1f }));

	auto moon_hold = std::make_shared<Node>("moon_hold", earth_hold);
	earth_hold->addChildren(moon_hold);
	moon_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 0.15f,0.0f,0.0f }));
	auto moon_geom = std::make_shared<GeometryNode>("moon_geom", moon_hold, planet_model);
	moon_hold->addChildren(moon_geom);
	moon_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.05f,0.05f,0.05f }));

	auto merc_hold = std::make_shared<Node>("merc_hold", root);
	root->addChildren(merc_hold);
	merc_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 1.6f,0.0f,0.0f }));
	auto merc_geom = std::make_shared<GeometryNode>("merc_geom", merc_hold, planet_model);
	merc_hold->addChildren(merc_geom);
	merc_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.125f,0.125f,0.125f }));

	auto venus_hold = std::make_shared<Node>("venus_hold", root);
	root->addChildren(venus_hold);
	venus_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 1.0f,0.0f,0.0f }));
	auto venus_geom = std::make_shared<GeometryNode>("venus_geom", venus_hold, planet_model);
	venus_hold->addChildren(venus_geom);
	venus_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.075f,0.075f,0.075f }));

	auto mars_hold = std::make_shared<Node>("mars_hold", root);
	root->addChildren(mars_hold);
	mars_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 2.0f,0.0f,0.0f }));
	auto mars_geom = std::make_shared<GeometryNode>("mars_geom", mars_hold, planet_model);
	mars_hold->addChildren(mars_geom);
	mars_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.175f,0.175f,0.175f }));

	auto jupit_hold = std::make_shared<Node>("jupit_hold", root);
	root->addChildren(jupit_hold);
	jupit_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 2.5f,0.0f,0.0f }));
	auto jupit_geom = std::make_shared<GeometryNode>("jupit_geom", jupit_hold, planet_model);
	jupit_hold->addChildren(jupit_geom);
	jupit_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.2f,0.2f,0.2f }));

	auto sat_hold = std::make_shared<Node>("sat_hold", root);
	root->addChildren(sat_hold);
	sat_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 2.6f,0.0f,0.0f }));
	auto sat_geom = std::make_shared<GeometryNode>("sat_geom", sat_hold, planet_model);
	sat_hold->addChildren(sat_geom);
	sat_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.15f,0.15f,0.15f }));

	auto uran_hold = std::make_shared<Node>("uran_hold", root);
	root->addChildren(uran_hold);
	uran_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 0.7f,0.0f,0.0f }));
	auto uran_geom = std::make_shared<GeometryNode>("uran_geom", uran_hold, planet_model);
	uran_hold->addChildren(uran_geom);
	uran_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.125f,0.125f,0.125f }));
	
	auto nept_hold = std::make_shared<Node>("nept_hold", root);
	root->addChildren(nept_hold);
	jupit_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 2.8f,0.0f,0.0f }));
	auto nept_geom = std::make_shared<GeometryNode>("nept_geom", nept_hold, planet_model);
	nept_hold->addChildren(nept_geom);
	nept_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.1f,0.1f,0.1f }));

	// init camera node
	auto camera = std::make_shared<CameraNode>("camera_1",root,true,true,m_view_projection);//utils::calculate_projection_matrix(initial_aspect_ratio));
	root->addChildren(camera);

	//test SceneGraph and node methods
	auto root_test = scenegraph_->getRoot();
	auto sg_name = scenegraph_->getName();
	auto sg_print = scenegraph_->printGraph();
	auto earth_geom_path = earth_geom->getPath();
	auto earth_geom_depth = earth_geom->getDepth();
	auto moon_geom_path = moon_geom->getPath();
	auto moon_geom_depth = moon_geom->getDepth();
	auto test_hold = std::make_shared<Node>("Test_Hold", moon_geom);
	moon_geom->addChildren(test_hold);
	auto test_geom = std::make_shared<GeometryNode>("Test_Geom", test_hold, planet_model);
	test_hold->addChildren(test_geom);
	auto get_result = root->getChildren("Test_Hold");
	auto rem_result = root->removeChildren("Test_Hold");
}

void ApplicationSolar::render() const {

	//traverse scenegraph until a geometry node is found
	//if a geometry node is found compute the worldtransform and add the object and draw it
	auto root = scenegraph_->getRoot();
	std::string planets[] = {
		"sun_geom",
		"uran_geom",
		"venus_geom",
		"earth_geom",
		"moon_geom",
		"merc_geom",
		"mars_geom", 
		"jupit_geom", 
		"sat_geom", 
		"nept_geom" };

	// render every planet
	for (int i = 0; i < 10; ++i) {
		auto planet = root->getChildren(planets[i]);
		glm::mat4 pl = planet->getParent()->getLocalTransform();
		glm::mat4 rm = {};
		if (i != 4) {
			rm = glm::rotate(glm::mat4x4{}, 0.0005f + (float)(10-i)*0.00001f, glm::fvec3{ 0.0f,1.0f,0.0f });
		}
		else {
			rm = glm::rotate(glm::mat4x4{}, 0.005f, glm::fvec3{ 0.0f,1.0f,0.0f });
		}
		planet->getParent()->setLocalTransform(rm*pl);
	
		//glm::mat4 rot_hold = planet->getParent()->getLocalTransform();
		//planet->getParent()->setLocalTransform(glm::rotate(rot_hold, float(glfwGetTime()), glm::fvec3{ 0.0f,1.0f,0.0f }));
		glm::mat4x4 model_matrix = planet->getWorldTransform();
		//glm::fmat4 model_matrix = glm::rotate(world_transform, float(glfwGetTime() + i), glm::fvec3{ 0.0f,1.0f,0.0f });

		// bind shader to upload uniforms
		glUseProgram(m_shaders.at("planet").handle);

		//glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{ 0.0f, 1.0f, 0.0f });
		//model_matrix = glm::translate(model_matrix, glm::fvec3{ 0.0f-i, 0.0f, -1.0f });
		glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
			1, GL_FALSE, glm::value_ptr(model_matrix));

		// extra matrix for normal transformation to keep them orthogonal to surface
		glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
		glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
			1, GL_FALSE, glm::value_ptr(normal_matrix));

		// bind the VAO to draw
		glBindVertexArray(planet_object.vertex_AO);

		// draw bound vertex array using bound shader
		glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
	}
}

void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
  // bind shader to which to upload unforms
  glUseProgram(m_shaders.at("planet").handle);
  // upload uniform values to new locations
  uploadView();
  uploadProjection();
}

///////////////////////////// intialisation functions /////////////////////////
// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/simple.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

   // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());
}

///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {

// differentiate inputs and apply corresponding translation
  if (key == GLFW_KEY_W  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    uploadView();
  }
  else if (key == GLFW_KEY_S  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    uploadView();
  }

  if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)){
  	m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.1f,0.0f,0.0f});
  	uploadView();
  }
  else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)){
  	m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.1f,0.0f,0.0f});
  	uploadView();
  }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {

// calculate pan and tilt from mouse input  
  float angle_pan = -(float) pos_x/10;
  float angle_tilt = -(float) pos_y/10;

// use the higher value and apply the rotation around this value
  if(std::abs(angle_pan)>std::abs(angle_tilt))
  {
	m_view_transform = glm::rotate(m_view_transform, glm::radians(angle_pan),glm::vec3{0,1,0});
  } else {
	m_view_transform = glm::rotate(m_view_transform ,glm::radians(angle_tilt),glm::vec3{1,0,0});
  }

  // update ViewMatrix
	uploadView();
}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
  // recalculate projection matrix for new aspect ration
  m_view_projection = utils::calculate_projection_matrix(float(width) / float(height));
  // upload new projection matrix
  uploadProjection();
}


// exe entry point
int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}