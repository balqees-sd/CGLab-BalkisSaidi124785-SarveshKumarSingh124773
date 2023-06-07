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
 ,stars_object{}
 ,planet_object{}
 ,orbits_object{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
 ,m_current_planet_shader{"planet"}
{
	initializeSceneGraph();
	initializeStars();
	initializePlanets();
	initializeOrbits();
	initializeShaderPrograms();
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);

  glDeleteBuffers(1, &stars_object.vertex_BO);
  glDeleteBuffers(1, &stars_object.element_BO);
  glDeleteVertexArrays(1, &stars_object.vertex_AO);

  glDeleteBuffers(1, &orbits_object.vertex_BO);
  glDeleteBuffers(1, &orbits_object.element_BO);
  glDeleteVertexArrays(1, &orbits_object.vertex_AO);
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

	// init a node for all stars
	auto stars_container = std::make_shared<Node>("stars_container", root);
	root->addChildren(stars_container); 

	// init all planets and holer with transformations as well as geometry
	auto point_light = std::make_shared<LightNode>("point_light", root, 1.0, color{ 255,255,255 });
	root->addChildren(point_light);
	auto sun_geom = std::make_shared<GeometryNode>("sun_geom", point_light, planet_model);
	point_light->addChildren(sun_geom);
	sun_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.5f,0.5f,0.5f }));

	auto earth_hold = std::make_shared<Node>("earth_hold", root);
	root->addChildren(earth_hold);
	earth_hold->setLocalTransform(glm::translate({}, glm::fvec3{3.9f,0.0f,0.0f}));
	auto earth_geom = std::make_shared<GeometryNode>("earth_geom", earth_hold, planet_model);
	earth_hold->addChildren(earth_geom);
	earth_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.1f,0.1f,0.1f }));

	auto moon_hold = std::make_shared<Node>("moon_hold", earth_hold);
	earth_hold->addChildren(moon_hold);
	moon_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 0.45f,0.0f,0.0f }));
	auto moon_geom = std::make_shared<GeometryNode>("moon_geom", moon_hold, planet_model);
	moon_hold->addChildren(moon_geom);
	moon_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.05f,0.05f,0.05f }));

	auto merc_hold = std::make_shared<Node>("merc_hold", root);
	root->addChildren(merc_hold);
	merc_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 4.8f,0.0f,0.0f }));
	auto merc_geom = std::make_shared<GeometryNode>("merc_geom", merc_hold, planet_model);
	merc_hold->addChildren(merc_geom);
	merc_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.125f,0.125f,0.125f }));

	auto venus_hold = std::make_shared<Node>("venus_hold", root);
	root->addChildren(venus_hold);
	venus_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 3.0f,0.0f,0.0f }));
	auto venus_geom = std::make_shared<GeometryNode>("venus_geom", venus_hold, planet_model);
	venus_hold->addChildren(venus_geom);
	venus_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.075f,0.075f,0.075f }));

	auto mars_hold = std::make_shared<Node>("mars_hold", root);
	root->addChildren(mars_hold);
	mars_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 6.0f,0.0f,0.0f }));
	auto mars_geom = std::make_shared<GeometryNode>("mars_geom", mars_hold, planet_model);
	mars_hold->addChildren(mars_geom);
	mars_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.175f,0.175f,0.175f }));

	auto jupit_hold = std::make_shared<Node>("jupit_hold", root);
	root->addChildren(jupit_hold);
	jupit_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 10.2f,0.0f,0.0f }));
	auto jupit_geom = std::make_shared<GeometryNode>("jupit_geom", jupit_hold, planet_model);
	jupit_hold->addChildren(jupit_geom);
	jupit_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.2f,0.2f,0.2f }));

	auto sat_hold = std::make_shared<Node>("sat_hold", root);
	root->addChildren(sat_hold);
	sat_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 7.5f,0.0f,0.0f }));
	auto sat_geom = std::make_shared<GeometryNode>("sat_geom", sat_hold, planet_model);
	sat_hold->addChildren(sat_geom);
	sat_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.1f,0.1f,0.1f }));

	auto uran_hold = std::make_shared<Node>("uran_hold", root);
	root->addChildren(uran_hold);
	uran_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 2.1f,0.0f,0.0f }));
	auto uran_geom = std::make_shared<GeometryNode>("uran_geom", uran_hold, planet_model);
	uran_hold->addChildren(uran_geom);
	uran_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.125f,0.125f,0.125f }));
	
	auto nept_hold = std::make_shared<Node>("nept_hold", root);
	root->addChildren(nept_hold);
	nept_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 8.4f,0.0f,0.0f }));
	auto nept_geom = std::make_shared<GeometryNode>("nept_geom", nept_hold, planet_model);
	nept_hold->addChildren(nept_geom);
	nept_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.1f,0.1f,0.1f }));

	// init camera node
	auto camera = std::make_shared<CameraNode>("camera_1",root,true,true,m_view_projection);//utils::calculate_projection_matrix(initial_aspect_ratio));
	root->addChildren(camera);

}

void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);

  glUseProgram(m_shaders.at("stars").handle);
  
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("stars").u_locs.at("ModelViewMatrix"),
  					 1, GL_FALSE, glm::value_ptr(view_matrix));

  
  glUseProgram(m_shaders.at("toon").handle);

  glUniformMatrix4fv(m_shaders.at("toon").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("planet").handle);

  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("orbits").handle);

  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("orbits").u_locs.at("ViewMatrix"),
	  1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
  
  // bind shader to which to upload unforms
  glUseProgram(m_shaders.at("stars").handle);
  
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("stars").u_locs.at("ProjectionMatrix"),
  					 1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("toon").handle);

  glUniformMatrix4fv(m_shaders.at("toon").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("planet").handle);

  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));


  glUseProgram(m_shaders.at("orbits").handle);

  glUniformMatrix4fv(m_shaders.at("orbits").u_locs.at("ProjectionMatrix"),
	  1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
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

  // store toon shader
  m_shaders.emplace("toon", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/toon.vert"},
										  {GL_FRAGMENT_SHADER, m_resource_path + "shaders/toon.frag"}}});

  // request uniform locations for shader program
  m_shaders.at("toon").u_locs["NormalMatrix"] = -1;
  m_shaders.at("toon").u_locs["ModelMatrix"] = -1;
  m_shaders.at("toon").u_locs["ViewMatrix"] = -1;
  m_shaders.at("toon").u_locs["ProjectionMatrix"] = -1;

  // store star shader
  m_shaders.emplace("stars", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/vao.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/vao.frag"}}});
  
  // request uniform locations for shader program
  m_shaders.at("stars").u_locs["ModelViewMatrix"] = -1;
  m_shaders.at("stars").u_locs["ProjectionMatrix"] = -1;


  // store orbit shader
  m_shaders.emplace("orbits", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/orb.vert"},
										    {GL_FRAGMENT_SHADER, m_resource_path + "shaders/orb.frag"}}});

  // request uniform locations for shader program
  m_shaders.at("orbits").u_locs["ModelMatrix"] = -1;
  m_shaders.at("orbits").u_locs["ViewMatrix"] = -1;
  m_shaders.at("orbits").u_locs["ProjectionMatrix"] = -1;
}


void ApplicationSolar::initializeStars(){

	std::vector<GLfloat> stars;
	unsigned int const numberStars = 10000;
	stars.reserve(numberStars*6*sizeof(float));

	for(unsigned int i =0; i<numberStars; i++)
	{
		float rand_x = static_cast<float>(std::rand() % 4100)/100.0f - 20;
		stars.push_back(rand_x);
		float rand_y = static_cast<float>(std::rand() % 4100)/100.0f - 20;
		stars.push_back(rand_y);
		float rand_z = static_cast<float>(std::rand() % 4100)/100.0f - 20;
		stars.push_back(rand_z);
		float rand_r = static_cast<float>(std::rand()%255)/255.0f;
		stars.push_back(rand_r);
		float rand_g = static_cast<float>(std::rand()%255)/255.0f;
		stars.push_back(rand_g);
		float rand_b = static_cast<float>(std::rand()%255)/255.0f;
		stars.push_back(rand_b);

	}

	glGenVertexArrays(1, &stars_object.vertex_AO);
	glBindVertexArray(stars_object.vertex_AO);

	glGenBuffers(1, &stars_object.vertex_BO);
	glBindBuffer(GL_ARRAY_BUFFER, stars_object.vertex_BO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*stars.size(), stars.data(), GL_STATIC_DRAW);

	// first attribArray for positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3,GL_FLOAT, GL_FALSE, GLsizei(6*sizeof(float)),0);

	// second attribArray for colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3,GL_FLOAT, GL_FALSE, GLsizei(6*sizeof(float)),(void*)(sizeof(float)*3));
	//the index of the vertexattribarray corresponds to layout position

	stars_object.draw_mode = GL_POINTS;
	stars_object.num_elements = GLsizei(numberStars);

}

void ApplicationSolar::initializePlanets(){
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
  // transfer number of (void*)(sizeof(float)*3)indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());
}

void ApplicationSolar::initializeOrbits(){
	auto root = scenegraph_->getRoot();
	//specify the number of planets and the number of points per orbit aswell as the angle between each of these points
	int num_planets = 10;
	int num_orbit_points = 65;
	float angle = 0.1f;
	//specify the names of plante nodes that should get an orbit
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
	
	std::vector<GLfloat> orbit;
	//for each planet calculate the position of the points on the orbit
	for (auto pl : planets) {
		orbit.clear();
		auto planet_geom = root->getChildren(pl);
		auto planet_hold = planet_geom->getParent();
		//create a rotation matrix to calculate the points on the orbit
		auto rot_mat = glm::rotate(glm::mat4x4{}, angle, glm::fvec3{ 0.0f,1.0f,0.0f });
		auto point = planet_hold->getLocalTransform()*glm::fvec4{ 0.0f,0.0f,0.0f,1.0f};
		//push the new calculated points nack
		for (int i = 0; i < num_orbit_points; ++i) {
			orbit.push_back(point.x);
			orbit.push_back(point.y);
			orbit.push_back(point.z);
			point = rot_mat * point;
		}
		//create a model using the vertices of the orbit
		model orbit_model{};
		orbit_model.data = orbit;
		orbit_model.vertex_num = num_orbit_points;
		//create a new geometry node using the orbit model
		auto orbit_node = std::make_shared<GeometryNode>(pl + "_orbit", orbit_model);
		//add the new node to the scenegraph
		planet_hold->getParent()->addChildren(orbit_node);
		orbit_node->setParent(planet_hold->getParent());
	}

	//create a new VertexArray
	glGenVertexArrays(1, &orbits_object.vertex_AO);
	glBindVertexArray(orbits_object.vertex_AO);

	//generate a new Buffer and bind it to the new VertexArray
	glGenBuffers(1, &orbits_object.vertex_BO);
	glBindBuffer(GL_ARRAY_BUFFER, orbits_object.vertex_BO);
	//specify the size of the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*orbit.size(), orbit.data(), GL_STATIC_DRAW);

	// first attribArray for positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(3 * sizeof(float)), 0);

	//specify the draw mode and the number of elements
	orbits_object.draw_mode = GL_LINE_STRIP;
	orbits_object.num_elements = GLsizei(num_orbit_points);
}

void ApplicationSolar::render()const{

	renderStars();
	renderPlanets(m_current_planet_shader);
	renderOrbits();
}

void ApplicationSolar::renderStars()const{

	glUseProgram(m_shaders.at("stars").handle);	

	// bind the VAO to draw
	glBindVertexArray(stars_object.vertex_AO);

	// draw bound vertex array using bound shader
	glDrawArrays(stars_object.draw_mode, GLint(0), stars_object.num_elements);

}

void ApplicationSolar::renderPlanets(std::string const& planet_shader)const{

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

	std::map<std::string,color> color_map;
	color_map.insert({ "sun_geom",{255, 255, 0} });
	color_map.insert({ "uran_geom",{188, 255, 252}});
	color_map.insert({ "venus_geom",{251, 213, 152} });
	color_map.insert({ "earth_geom",{78, 153, 255} });
	color_map.insert({ "moon_geom",{219, 219, 219} });
	color_map.insert({ "merc_geom",{157, 157, 157} });
	color_map.insert({ "mars_geom",{229, 141, 0} });
	color_map.insert({ "jupit_geom",{255, 207, 128} });
	color_map.insert({ "sat_geom",{229, 212, 186} });
	color_map.insert({ "nept_geom",{99, 204, 251} });

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
	
		glm::mat4x4 model_matrix = planet->getWorldTransform();

		// bind shader to upload uniforms
		glUseProgram(m_shaders.at(planet_shader).handle);

		glUniformMatrix4fv(m_shaders.at(planet_shader).u_locs.at("ModelMatrix"),
			1, GL_FALSE, glm::value_ptr(model_matrix));

		// extra matrix for normal transformation to keep them orthogonal to surface
		glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
		glUniformMatrix4fv(m_shaders.at(planet_shader).u_locs.at("NormalMatrix"),
			1, GL_FALSE, glm::value_ptr(normal_matrix));

		// bind the VAO to draw
		glBindVertexArray(planet_object.vertex_AO);

		int planetColorLocation = glGetUniformLocation(m_shaders.at(planet_shader).handle, "planetColor");
		color planetColor = color_map[planets[i]];
		glUniform3f(planetColorLocation,planetColor.r/255.0f, planetColor.g/255.0f, planetColor.b/255.0f);

		//update the position, intensity and color of the point light
		auto light_node = root->getChildren("point_light");
		auto light = std::static_pointer_cast<LightNode> (light_node);
		color light_color = light->getLightColor();
		float light_intensity = light->getLightIntensity();
		glm::fvec4 light_position = light->getWorldTransform()*glm::fvec4(0.0f,0.0f,0.0f,1.0f);
		
		int lightPositionLocation = glGetUniformLocation(m_shaders.at(planet_shader).handle, "light_position");
		glUniform3f(lightPositionLocation, light_position.x, light_position.y, light_position.z);
		
		int lightIntensityLocation = glGetUniformLocation(m_shaders.at(planet_shader).handle, "light_strength");
		glUniform1f(lightIntensityLocation, light_intensity);

		int lightColorLocation = glGetUniformLocation(m_shaders.at(planet_shader).handle, "light_color");
		glUniform3f(lightColorLocation, light_color.r/255.0f, light_color.g/255.0f, light_color.b/255.0f);

		int ambientStrengthLocation = glGetUniformLocation(m_shaders.at(planet_shader).handle, "ambient_strength");
		
		if (i == 0) {
			glUniform1f(ambientStrengthLocation, 1.0);
		} else {
			glUniform1f(ambientStrengthLocation, 0.1);
		}

		// draw bound vertex array using bound shader
		glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
	}
}

void ApplicationSolar::renderOrbits()const{
	//get the orbit nodes by name
	std::string orbits[] = {
	"sun_geom_orbit",
	"uran_geom_orbit",
	"venus_geom_orbit",
	"earth_geom_orbit",
	"moon_geom_orbit",
	"merc_geom_orbit",
	"mars_geom_orbit",
	"jupit_geom_orbit",
	"sat_geom_orbit",
	"nept_geom_orbit" };

	//declare the shader we want to use
	glUseProgram(m_shaders.at("orbits").handle);

	//for every orbit of a planet draw it
	for (auto& orbit_name : orbits) {

		//get the geometry of the orbit that is stored in the node
		auto orbit = scenegraph_->getRoot()->getChildren(orbit_name);
		auto orbit_geom = std::static_pointer_cast<GeometryNode> (orbit);
		auto orbit_world_transform = orbit->getWorldTransform();
		model orbit_model = orbit_geom->getGeometry();
		std::vector<GLfloat> orbit_data = (*orbit_geom).getGeometry().data;
		
		//create the ModelMatrix using the WorldTransform of the orbit
		glUniformMatrix4fv(m_shaders.at("orbits").u_locs.at("ModelMatrix"),
			1, GL_FALSE, glm::value_ptr(orbit_world_transform));
		
		//bind the buffer of the orbit to set the data of the current orbit
		glBindBuffer(GL_ARRAY_BUFFER, orbits_object.vertex_BO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*orbit_data.size(), orbit_data.data(), GL_STATIC_DRAW);
		
		//bind the VertexArray for drawing
		glBindVertexArray(orbits_object.vertex_AO);
		
		//draw the array
		glDrawArrays(orbits_object.draw_mode, GLint(0), orbits_object.num_elements);
	}
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

  if (key == GLFW_KEY_1 && (action == GLFW_PRESS)){
  	m_current_planet_shader = "planet";
  	uploadView();
  }
  else if (key == GLFW_KEY_2 && (action == GLFW_PRESS)){
  	m_current_planet_shader = "toon";
  	uploadView();
  }
}


//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {

	float x = -(float)pos_x / 10;
	float y = -(float)pos_y / 10;

	// mouse handling
	m_view_transform = glm::rotate(m_view_transform, glm::radians(x), glm::vec3{ 0.0f, 1.0f, 0.0f });
	m_view_transform = glm::rotate(m_view_transform, glm::radians(y), glm::vec3{ 1.0f, 0.0f, 0.0f });

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