#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "texture_loader.hpp"

#include "pixel_data.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <algorithm>


ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,stars_object{}
 ,orbits_object{}
 ,skybox_object{}
 ,screenquad_object{}
 ,framebuffer_obj{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
 ,m_textures{}
 ,m_current_planet_shader{"planet"}
 ,m_planet_names{
		"sun",
		"uranus",
		"venus",
		"earth",
		"moon",
		"mercury",
		"mars",
		"jupiter",
		"saturn",
		"neptune"
	}
 ,img_width{initial_resolution.x}
 ,img_height{initial_resolution.y}
{
	initializeSceneGraph();
	initializeTextures();
	initializeFramebuffer(initial_resolution.x, initial_resolution.y);
	initializeShaderPrograms();
	initializePlanets();
	initializeStars();
	initializeOrbits();
	initializeSkybox();
	initializeScreenquad();
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

  glDeleteBuffers(1, &skybox_object.vertex_BO);
  glDeleteBuffers(1, &skybox_object.element_BO);
  glDeleteVertexArrays(1, &skybox_object.vertex_AO);

  glDeleteBuffers(1, &screenquad_object.vertex_BO);
  glDeleteBuffers(1, &screenquad_object.element_BO);
  glDeleteVertexArrays(1, &screenquad_object.vertex_AO);
}

void ApplicationSolar::initializeSceneGraph() {
	//TODO is this the correct way???
	//How to set the root node of the SceneGraph?
	//What about the PointLightNode?

	// init singleton
	scenegraph_ = SceneGraph::getInstance();

	// call model_loader
	model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD);
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
	glm::mat4 rm = glm::rotate(glm::mat4x4{}, 5.0f, glm::fvec3{ 0.0f,1.0f,0.0f });
	earth_hold->setLocalTransform(rm*earth_hold->getLocalTransform());
	auto earth_geom = std::make_shared<GeometryNode>("earth_geom", earth_hold, planet_model);
	earth_hold->addChildren(earth_geom);
	earth_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.1f,0.1f,0.1f }));

	auto moon_hold = std::make_shared<Node>("moon_hold", earth_hold);
	earth_hold->addChildren(moon_hold);
	moon_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 0.45f,0.0f,0.0f }));
	rm = glm::rotate(glm::mat4x4{}, 10.0f, glm::fvec3{ 0.0f,1.0f,0.0f });
	moon_hold->setLocalTransform(rm*moon_hold->getLocalTransform());
	auto moon_geom = std::make_shared<GeometryNode>("moon_geom", moon_hold, planet_model);
	moon_hold->addChildren(moon_geom);
	moon_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.05f,0.05f,0.05f }));

	auto merc_hold = std::make_shared<Node>("mercury_hold", root);
	root->addChildren(merc_hold);
	merc_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 4.8f,0.0f,0.0f }));
	rm = glm::rotate(glm::mat4x4{}, 15.0f, glm::fvec3{ 0.0f,1.0f,0.0f });
	merc_hold->setLocalTransform(rm*merc_hold->getLocalTransform());
	auto merc_geom = std::make_shared<GeometryNode>("mercury_geom", merc_hold, planet_model);
	merc_hold->addChildren(merc_geom);
	merc_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.125f,0.125f,0.125f }));

	auto venus_hold = std::make_shared<Node>("venus_hold", root);
	root->addChildren(venus_hold);
	venus_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 3.0f,0.0f,0.0f }));
	rm = glm::rotate(glm::mat4x4{}, 20.0f, glm::fvec3{ 0.0f,1.0f,0.0f });
	venus_hold->setLocalTransform(rm*venus_hold->getLocalTransform());
	auto venus_geom = std::make_shared<GeometryNode>("venus_geom", venus_hold, planet_model);
	venus_hold->addChildren(venus_geom);
	venus_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.075f,0.075f,0.075f }));

	auto mars_hold = std::make_shared<Node>("mars_hold", root);
	root->addChildren(mars_hold);
	mars_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 6.0f,0.0f,0.0f }));
	rm = glm::rotate(glm::mat4x4{}, 25.0f, glm::fvec3{ 0.0f,1.0f,0.0f });
	mars_hold->setLocalTransform(rm*mars_hold->getLocalTransform());
	auto mars_geom = std::make_shared<GeometryNode>("mars_geom", mars_hold, planet_model);
	mars_hold->addChildren(mars_geom);
	mars_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.175f,0.175f,0.175f }));

	auto jupit_hold = std::make_shared<Node>("jupiter_hold", root);
	root->addChildren(jupit_hold);
	jupit_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 10.2f,0.0f,0.0f }));
	rm = glm::rotate(glm::mat4x4{}, 30.0f, glm::fvec3{ 0.0f,1.0f,0.0f });
	jupit_hold->setLocalTransform(rm*jupit_hold->getLocalTransform());
	auto jupit_geom = std::make_shared<GeometryNode>("jupiter_geom", jupit_hold, planet_model);
	jupit_hold->addChildren(jupit_geom);
	jupit_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.2f,0.2f,0.2f }));

	auto sat_hold = std::make_shared<Node>("saturn_hold", root);
	root->addChildren(sat_hold);
	sat_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 7.5f,0.0f,0.0f }));
	rm = glm::rotate(glm::mat4x4{}, 35.0f, glm::fvec3{ 0.0f,1.0f,0.0f });
	sat_hold->setLocalTransform(rm*sat_hold->getLocalTransform());
	auto sat_geom = std::make_shared<GeometryNode>("saturn_geom", sat_hold, planet_model);
	sat_hold->addChildren(sat_geom);
	sat_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.1f,0.1f,0.1f }));

	auto uran_hold = std::make_shared<Node>("uranus_hold", root);
	root->addChildren(uran_hold);
	uran_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 2.1f,0.0f,0.0f }));
	rm = glm::rotate(glm::mat4x4{}, 40.0f, glm::fvec3{ 0.0f,1.0f,0.0f });
	uran_hold->setLocalTransform(rm*uran_hold->getLocalTransform());
	auto uran_geom = std::make_shared<GeometryNode>("uranus_geom", uran_hold, planet_model);
	uran_hold->addChildren(uran_geom);
	uran_geom->setLocalTransform(glm::scale({}, glm::fvec3{ 0.125f,0.125f,0.125f }));
	
	auto nept_hold = std::make_shared<Node>("neptune_hold", root);
	root->addChildren(nept_hold);
	nept_hold->setLocalTransform(glm::translate({}, glm::fvec3{ 8.4f,0.0f,0.0f }));
	rm = glm::rotate(glm::mat4x4{}, 45.0f, glm::fvec3{ 0.0f,1.0f,0.0f });
	nept_hold->setLocalTransform(rm*nept_hold->getLocalTransform());
	auto nept_geom = std::make_shared<GeometryNode>("neptune_geom", nept_hold, planet_model);
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

void ApplicationSolar::uploadView() {

  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);

  glUseProgram(m_shaders.at("stars").handle);
  
  glUniformMatrix4fv(m_shaders.at("stars").u_locs.at("ModelViewMatrix"),
  					 1, GL_FALSE, glm::value_ptr(view_matrix));

  
  glUseProgram(m_shaders.at("toon").handle);

  glUniformMatrix4fv(m_shaders.at("toon").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));


  glUseProgram(m_shaders.at("planet").handle);

  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));


  glUseProgram(m_shaders.at("orbits").handle);

  glUniformMatrix4fv(m_shaders.at("orbits").u_locs.at("ViewMatrix"),
	  				 1, GL_FALSE, glm::value_ptr(view_matrix));


  glUseProgram(m_shaders.at("skybox").handle);

  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ViewMatrix"),
  	  				 1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("simple_quad").handle);

  glUniform1i(m_shaders.at("simple_quad").u_locs.at("horizontalMirroring"),horizontal_mirroring);
  glUniform1i(m_shaders.at("simple_quad").u_locs.at("verticalMirroring"),vertical_mirroring);
  glUniform1i(m_shaders.at("simple_quad").u_locs.at("greyscale"),greyscale);
  glUniform1i(m_shaders.at("simple_quad").u_locs.at("blur"),blur);
  glUniform2f(m_shaders.at("simple_quad").u_locs.at("textureSize"),img_width,img_height);
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


  glUseProgram(m_shaders.at("skybox").handle);

  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ProjectionMatrix"),
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
  m_shaders.at("planet").u_locs["TextureSampler"] = -1;

  // store toon shader
  m_shaders.emplace("toon", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/toon.vert"},
										  {GL_FRAGMENT_SHADER, m_resource_path + "shaders/toon.frag"}}});

  // request uniform locations for shader program
  m_shaders.at("toon").u_locs["NormalMatrix"] = -1;
  m_shaders.at("toon").u_locs["ModelMatrix"] = -1;
  m_shaders.at("toon").u_locs["ViewMatrix"] = -1;
  m_shaders.at("toon").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("planet").u_locs["TextureSampler"] = -1;

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

  // store skybox shader
  m_shaders.emplace("skybox", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/skybox.vert"},
											{GL_FRAGMENT_SHADER, m_resource_path + "shaders/skybox.frag"}}});

  m_shaders.at("skybox").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("skybox").u_locs["ViewMatrix"] = -1;

  //store quad shader for working with extra framebuffer
  m_shaders.emplace("simple_quad", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/simple_quad.vert"},
												 {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple_quad.frag"}}});

  m_shaders.at("simple_quad").u_locs["screenTexture"] = -1;
  m_shaders.at("simple_quad").u_locs["horizontalMirroring"]= 0;
  m_shaders.at("simple_quad").u_locs["verticalMirroring"]= 0;
  m_shaders.at("simple_quad").u_locs["greyscale"]= 0;
  m_shaders.at("simple_quad").u_locs["blur"]= 0;
  m_shaders.at("simple_quad").u_locs["textureSize"] = -1;
}

void ApplicationSolar::initializeScreenquad(){
	//create quad
	std::vector<GLfloat> quad = {
		-1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
   	};

	//create a new VertexArray
	glGenVertexArrays(1, &screenquad_object.vertex_AO);
	glBindVertexArray(screenquad_object.vertex_AO);

	//generate a new Buffer and bind it to the new VertexArray
	glGenBuffers(1, &screenquad_object.vertex_BO);
	glBindBuffer(GL_ARRAY_BUFFER, screenquad_object.vertex_BO);
	//specify the size of the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*quad.size(), quad.data(), GL_STATIC_DRAW);

	// first attribArray for positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, GLsizei(4 * sizeof(float)), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, GLsizei(4 * sizeof(float)), (void*)(2 * sizeof(float)));

	//specify the draw mode and the number of elements
	screenquad_object.draw_mode = GL_TRIANGLE_STRIP;
	screenquad_object.num_elements = GLsizei(quad.size()/4);
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
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD);

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
  // activate third attribute on gpu
  glEnableVertexAttribArray(2);
  // third attribute is 2 floats with no offset & stride
  glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);

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
	
	std::vector<GLfloat> orbit;
	//for each planet calculate the position of the points on the orbit
	for (auto pl : m_planet_names) {
		orbit.clear();
		auto planet_geom = root->getChildren(pl+"_geom");
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
		auto orbit_node = std::make_shared<GeometryNode>(pl+"_geom" + "_orbit", orbit_model);
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

void ApplicationSolar::initializeSkybox() {

	std::vector<GLfloat> skybox;
	skybox.reserve(36*3*sizeof(GLfloat)); 
	std::ifstream cin(m_resource_path + "models/skybox.txt");
    while(!cin.eof()) {
    	float x, y ,z;
    	cin>>x>>y>>z;
    	skybox.push_back((GLfloat)x);
    	skybox.push_back((GLfloat)y);
    	skybox.push_back((GLfloat)z);
    }

	model skybox_model{};
	skybox_model.data = skybox;
	skybox_model.vertex_num = skybox.size();
	
	//create a new geometry node using the orbit model
	auto skybox_node = std::make_shared<GeometryNode>("skybox", skybox_model);
	
	//add the new node to the scenegraph
	scenegraph_->getRoot()->addChildren(skybox_node);
	skybox_node->setParent(scenegraph_->getRoot());

	//create a new VertexArray
	glGenVertexArrays(1, &skybox_object.vertex_AO);
	glBindVertexArray(skybox_object.vertex_AO);

	//generate a new Buffer and bind it to the new VertexArray
	glGenBuffers(1, &skybox_object.vertex_BO);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_object.vertex_BO);
	//specify the size of the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*skybox.size(), skybox.data(), GL_STATIC_DRAW);

	// first attribArray for positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(3 * sizeof(float)), 0);

	//specify the draw mode and the number of elements
	skybox_object.draw_mode = GL_TRIANGLES;
	skybox_object.num_elements = GLsizei(skybox.size());

}
/////////////////////////Rendering////////////////////////////

void ApplicationSolar::render()const{

	//render to texture
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_obj.handle);
	glClearColor(0.1f,0.1f,0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	renderStars();
	renderPlanets(m_current_planet_shader);
	renderOrbits();
	renderSkybox();

	//render to default for display
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	renderQuad();
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

	std::map<std::string, color> color_map;
	color_map.insert({ "sun_geom",{255, 255, 0} });
	color_map.insert({ "uranus_geom",{188, 255, 252}});
	color_map.insert({ "venus_geom",{251, 213, 152} });
	color_map.insert({ "earth_geom",{78, 153, 255} });
	color_map.insert({ "moon_geom",{219, 219, 219} });
	color_map.insert({ "mercury_geom",{157, 157, 157} });
	color_map.insert({ "mars_geom",{229, 141, 0} });
	color_map.insert({ "jupiter_geom",{255, 207, 128} });
	color_map.insert({ "saturn_geom",{229, 212, 186} });
	color_map.insert({ "neptune_geom",{99, 204, 251} });
	unsigned int index = 0;

	// render every planet
	for (std::string planet_name :  m_planet_names) {
		auto planet = root->getChildren(planet_name+"_geom");
		glm::mat4 pl = planet->getParent()->getLocalTransform();
		glm::mat4 rm = {};

		if (!time_stop) {
			if (planet_name != "moon") {
				rm = glm::rotate(glm::mat4x4{}, 0.0005f + (float)(10-index)*0.00001f, glm::fvec3{ 0.0f,1.0f,0.0f });
			} else {
				rm = glm::rotate(glm::mat4x4{}, 0.005f, glm::fvec3{ 0.0f,1.0f,0.0f });
			}
			planet->getParent()->setLocalTransform(rm*pl);
			if (planet_name != "sun") {
				rm = glm::rotate(glm::mat4x4{}, 0.005f + (float)(10 - index)*0.0001f, glm::fvec3{ 0.0f,1.0f,0.0f });
				planet->setLocalTransform(rm*planet->getLocalTransform());
			}
		}

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

		texture_object texture = m_textures.at(planet_name+"_tex");
		texture_object normal_texture = m_textures.at(planet_name + "_normal_tex");

		glActiveTexture(GL_TEXTURE1+2*index);
		// bind texture
		glBindTexture(texture.target, texture.handle);

		// add sampler
		int samplerLocation = glGetUniformLocation(m_shaders.at(planet_shader).handle,"TextureSampler");
		glUniform1i(samplerLocation, texture.handle);

		glActiveTexture(GL_TEXTURE1 + 2 * index + 1);
		glBindTexture(normal_texture.target, normal_texture.handle);
		int normalSamplerLocation = glGetUniformLocation(m_shaders.at(planet_shader).handle, "NormalSampler");
		glUniform1i(normalSamplerLocation, normal_texture.handle);

		
		// add planet color
		int planetColorLocation = glGetUniformLocation(m_shaders.at(planet_shader).handle, "planetColor");
		color planetColor = color_map[planet_name+"_geom"];
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
		
		if (planet_name == "sun") {
			glUniform1f(ambientStrengthLocation, 1.0);
		} else {
			glUniform1f(ambientStrengthLocation, 0.1);
		}

		// draw bound vertex array using bound shader
		glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
		index++;
	}
}

void ApplicationSolar::renderOrbits()const{

	//declare the shader we want to use
	glUseProgram(m_shaders.at("orbits").handle);

	//for every orbit of a planet draw it
	for (auto& planet_name : m_planet_names) {

		//get the geometry of the orbit that is stored in the node
		auto orbit = scenegraph_->getRoot()->getChildren(planet_name+"_geom_orbit");
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

void ApplicationSolar::renderSkybox() const{
	glDepthFunc(GL_EQUAL);
	glUseProgram(m_shaders.at("skybox").handle);

	// bind the VAO to draw
	glBindVertexArray(skybox_object.vertex_AO);
	
	glActiveTexture(active_skymap_texture);

	// bind texture
	glBindTexture(skymap_texture.target, skymap_texture.handle);

	// add sampler
	int samplerLocation = glGetUniformLocation(m_shaders.at("skybox").handle,"TextureSampler");
	glUniform1i(samplerLocation, skymap_texture.handle);

	// draw
	glDrawArrays(GL_TRIANGLES, 0, skybox_object.num_elements);
	glDepthFunc(GL_LESS);
	//glDepthMask(GL_TRUE);
}

void ApplicationSolar::renderQuad()const {

	glUseProgram(m_shaders.at("simple_quad").handle);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,framebuffer_obj.texture_handle);

	// add sampler
	int samplerLocation = glGetUniformLocation(m_shaders.at("simple_quad").handle,"screenTexture");
	glUniform1i(samplerLocation,0);//framebuffer_obj.texture_obj.handle);

	//render quad 
	glBindVertexArray(screenquad_object.vertex_AO);
	glDrawArrays(screenquad_object.draw_mode, 0, screenquad_object.num_elements);
}


////////////////////////////// texture stuff /////////////////
// loads textures
void ApplicationSolar::initializeTextures () {

	int planetIndex = 0;
	for(std::string planet : m_planet_names) {
		pixel_data planet_data;
		try {
			planet_data = texture_loader::file(m_resource_path + "textures/"+planet+"map1k.png");
		}
		catch(std::exception e)
		{
			std::cout<<"Error loading texturefile for "+planet+". \n "+e.what()+"\n";
		}

		GLsizei width = (GLsizei) planet_data.width;
		GLsizei height = (GLsizei) planet_data.height;
		GLenum channel_number = planet_data.channels;
		GLenum channel_type = planet_data.channel_type;

		//glActiveTexture(GL_TEXTURE+planetIndex);
		glActiveTexture(GL_TEXTURE1+2*planetIndex);
		texture_object texture;
		glGenTextures(1, &texture.handle);
		texture.target = GL_TEXTURE_2D;
		std::string texture_name = planet+"_tex";
		m_textures.insert({texture_name, texture});

		glBindTexture(texture.target, texture.handle);

		//optional

		//Texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		//Texture filteriglTexImage2D(GL_TEXTURE_2D, 0, ng
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, channel_number, width, height, 0, channel_number, channel_type, planet_data.ptr());
		//glGenerateMipmap(GL_TEXTURE_2D);
		planetIndex++;

		try {
			planet_data = texture_loader::file(m_resource_path + "maps/" + planet + "map2k-normal.png");
		}
		catch (std::exception e)
		{
			planet_data = texture_loader::file(m_resource_path + "maps/base-normal.png");
			std::cout << "Error loading texturefile for " + planet + ". \n " + e.what() + ". Default normal was loaded.\n";
		}

		width = (GLsizei)planet_data.width;
		height = (GLsizei)planet_data.height;
		channel_number = planet_data.channels;
		channel_type = planet_data.channel_type;

		//glActiveTexture(GL_TEXTURE+planetIndex);
		glActiveTexture(GL_TEXTURE1 + 2 * planetIndex + 1);
		texture_object normal_texture;
		glGenTextures(1, &normal_texture.handle);
		normal_texture.target = GL_TEXTURE_2D;
		texture_name = planet + "_normal_tex";
		m_textures.insert({ texture_name, normal_texture });

		glBindTexture(normal_texture.target, normal_texture.handle);

		//optional

		//Texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//Texture filteriglTexImage2D(GL_TEXTURE_2D, 0, ng
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, channel_number, width, height, 0, channel_number, channel_type, planet_data.ptr());
		//glGenerateMipmap(GL_TEXTURE_2D);
		planetIndex++;

	}

	// loads skymap textures
	std::vector<std::string> sides = {"right","left","bottom","top","front","back"};
	int skyIndex = 0;
	active_skymap_texture = GL_TEXTURE1+planetIndex;
	glActiveTexture(active_skymap_texture);

	texture_object texture;
	glGenTextures(1, &texture.handle);
	texture.target = GL_TEXTURE_CUBE_MAP;
	skymap_texture = texture;

	glBindTexture(texture.target, texture.handle);


	for(std::string side : sides) {
		pixel_data skymap_data;
		try{
			skymap_data = texture_loader::file(m_resource_path + "textures/sky_"+side+".png");
			GLsizei width = (GLsizei) skymap_data.width;
			GLsizei height = (GLsizei) skymap_data.height;
			GLenum channel_number = skymap_data.channels;
			GLenum channel_type = skymap_data.channel_type;

			if(skymap_data.ptr()) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + skyIndex, 0, channel_number, width, height, 0, channel_number, channel_type, skymap_data.ptr());
			}
			else {
				std::cout<<"Error loading skymap for "+side+" face.\n";
			}
		}
		catch(std::exception e) {
			std::cout<<"Error loading skymapfile for "+side+" face. \n "+e.what()+"\n";
		}
		skyIndex++;
	}
	//Texture wrapping
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	//Texture filtering
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	 
}

bool ApplicationSolar::initializeFramebuffer(unsigned width, unsigned height) {

	//generate Framebuffer
	glGenFramebuffers(1, &framebuffer_obj.handle);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_obj.handle);

	//create texture attachment as color_attachment
	texture_object texture;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1,&texture.handle);
	glBindTexture(GL_TEXTURE_2D, texture.handle);

	//std::cout<<"Resolution: "<<initial_resolution.x<<","<<initial_resolution<<"\n";
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.handle, 0);

	framebuffer_obj.texture_obj = texture;
	framebuffer_obj.texture_handle = texture.handle;

	//create renderbuffer attachment (depth attachment)
	unsigned int renderbuffer_object;
	glGenRenderbuffers(1, &renderbuffer_object);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_object);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_object);

	framebuffer_obj.renderbuffer_handle = renderbuffer_object;
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1,drawBuffers);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout<<"Framebuffer could not get initialized. Framebuffer not complete.\n";
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	return true;
}


///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {

// differentiate inputs and apply corresponding translation
  //back and forward
  if (key == GLFW_KEY_W  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    uploadView();
  } else if (key == GLFW_KEY_S  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    uploadView();
  }
  //left and right
  if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)){
  	m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.1f,0.0f,0.0f});
  	uploadView();
  } else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)){
  	m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.1f,0.0f,0.0f});
  	uploadView();
  }

  //shading
  if (key == GLFW_KEY_1 && (action == GLFW_PRESS)){
  	m_current_planet_shader = "planet";
  	uploadView();
  } else if (key == GLFW_KEY_2 && (action == GLFW_PRESS)){
  	m_current_planet_shader = "toon";
  	uploadView();
  } else if (key == GLFW_KEY_3 && (action == GLFW_PRESS)) {
	  time_stop = !time_stop;
	  uploadView();
  }

  //postprocessing
  if (key == GLFW_KEY_8 && (action == GLFW_PRESS)){
  	horizontal_mirroring = !horizontal_mirroring;
  	uploadView();
  } else if (key == GLFW_KEY_9 && (action == GLFW_PRESS)){
  	vertical_mirroring = !vertical_mirroring;
  	uploadView();
  } else if (key == GLFW_KEY_7 && (action == GLFW_PRESS)){
  	greyscale = !greyscale;
  	uploadView();
  } else if (key == GLFW_KEY_0 && (action == GLFW_PRESS)){
  	blur = !blur;
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
  initializeFramebuffer(width, height);
  img_width = width;
  img_height = height;
}

// exe entry point
int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}