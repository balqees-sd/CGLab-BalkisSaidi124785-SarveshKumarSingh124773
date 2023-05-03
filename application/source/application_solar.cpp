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
	:Application{ resource_path }
	, planet_object{}
	, m_view_transform{ glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f}) }
	, m_view_projection{ utils::calculate_projection_matrix(initial_aspect_ratio) }
{
	initializeGeometry();
	initializeShaderPrograms();
}

ApplicationSolar::~ApplicationSolar() {
	glDeleteBuffers(1, &planet_object.vertex_BO);
	glDeleteBuffers(1, &planet_object.element_BO);
	glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

//defining a structure planet with different properties
using namespace std;
struct Planet {
	string name;
	glm::fvec3 size;
	glm::fvec3 translate;
	float rotation;
	float orbitDiff;
};

void ApplicationSolar::render() const {



	// bind shader to upload uniforms
	glUseProgram(m_shaders.at("planet").handle);


	
	//define the planets and the moon
	int NUMBER_OF_CEL_BODIES = 11;
	float SUN_SIZE = 6.0; //sun size
	//Declare default values for the minimum distance from the sun, the rotation speed, the orbit differential,
	//and the time it takes to complete one orbit.
	float DEFAULT_DIST = 8.0, DEFAULT_ROTATION = 1.0, DEFAULT_ORBIT = 1.0, DEFAULT_TIME = 1.0;

	//SUN
	Planet sun;
	sun.name = "sun"; 
	sun.size = { SUN_SIZE, SUN_SIZE, SUN_SIZE }; //default size 
	sun.translate = { 0.0, 0.0, 0.0 }; //center position
	sun.rotation = 0.0; //for the rotation time of planet while rendering
	sun.orbitDiff = 0;//the sun is the center that is way orbitdiff equal to zero

	//MOON
	Planet moon;
	moon.name = "moon";
	moon.size = { SUN_SIZE * 0.1, SUN_SIZE * 0.1, SUN_SIZE * 0.1 };
	moon.translate = { DEFAULT_DIST * 2.0, DEFAULT_DIST * 2.0, DEFAULT_DIST * 2.0 };
	moon.rotation = DEFAULT_ROTATION * 2.0;//for the rotation time of planet while rendering
	moon.orbitDiff = DEFAULT_ORBIT * 3.0;

	//MERCURY
	Planet mercury;
	mercury.name = "mercury";
	mercury.size = { SUN_SIZE * 0.2, SUN_SIZE * 0.2, SUN_SIZE * 0.2 };
	mercury.translate = { DEFAULT_DIST * 1.25, DEFAULT_DIST * 1.25, DEFAULT_DIST * 1.25 };
	mercury.rotation = DEFAULT_ROTATION * 3.0;//for the rotation time of planet while rendering
	mercury.orbitDiff = DEFAULT_ORBIT * 0.2;

	//VENUS
	Planet venus;
	venus.name = "venus";
	venus.size = { SUN_SIZE * 0.4, SUN_SIZE * 0.4, SUN_SIZE * 0.4 };
	venus.translate = { DEFAULT_DIST * 1.30, DEFAULT_DIST * 1.30, DEFAULT_DIST * 1.30 };
	venus.rotation = DEFAULT_ROTATION * -2.0;//for the rotation time of planet while rendering
	venus.orbitDiff = DEFAULT_ORBIT * 0.6;

	//EARTH
	Planet earth;
	earth.name = "earth";
	earth.size = { SUN_SIZE * 0.5, SUN_SIZE * 0.5, SUN_SIZE * 0.5 };
	earth.translate = { DEFAULT_DIST * 1.5, DEFAULT_DIST * 1.5, DEFAULT_DIST * 1.5 };
	earth.rotation = DEFAULT_ROTATION * 1.0; //for the rotation time of planet while rendering
	earth.orbitDiff = DEFAULT_ORBIT * 1.0;

	//MARS
	Planet mars;
	mars.name = "mars";
	mars.size = { SUN_SIZE * 0.25, SUN_SIZE * 0.25, SUN_SIZE * 0.25 };
	mars.translate = { DEFAULT_DIST * 1.65, DEFAULT_DIST * 1.65, DEFAULT_DIST * 1.65 };
	mars.rotation = DEFAULT_ROTATION * 1.3; //for the rotation time of planet while rendering
	mars.orbitDiff = DEFAULT_ORBIT * 1.2;

	//JUPITER
	Planet jupiter;
	jupiter.name = "jupiter";
	jupiter.size = { SUN_SIZE * 0.8, SUN_SIZE * 0.8, SUN_SIZE * 0.8 };
	jupiter.translate = { DEFAULT_DIST * 1.75, DEFAULT_DIST * 1.75, DEFAULT_DIST * 1.75 };
	jupiter.rotation = DEFAULT_ROTATION * 0.5; //for the rotation time of planet while rendering
	jupiter.orbitDiff = DEFAULT_ORBIT * 1.6;

	//SATURN
	Planet saturn;
	saturn.name = "saturn";
	saturn.size = { SUN_SIZE * 0.7, SUN_SIZE * 0.7, SUN_SIZE * 0.7 };
	saturn.translate = { DEFAULT_DIST * 1.85, DEFAULT_DIST * 1.85, DEFAULT_DIST * 1.85 };
	saturn.rotation = DEFAULT_ROTATION * 0.65; //for the rotation time of planet while rendering
	saturn.orbitDiff = DEFAULT_ORBIT * 2.0;

	//URANUS
	Planet uranus;
	uranus.name = "uranus";
	uranus.size = { SUN_SIZE * 0.6, SUN_SIZE * 0.6, SUN_SIZE * 0.6 };
	uranus.translate = { DEFAULT_DIST * 1.9, DEFAULT_DIST * 1.9, DEFAULT_DIST * 1.9 };
	uranus.rotation = DEFAULT_ROTATION * -0.1;//for the rotation time of planet while rendering
	uranus.orbitDiff = DEFAULT_ORBIT * 2.3;

	//NEPTUNE
	Planet neptune;
	neptune.name = "neptune";
	neptune.size = { SUN_SIZE * 0.63, SUN_SIZE * 0.63, SUN_SIZE * 0.63 };
	neptune.translate = { DEFAULT_DIST * 2.0, DEFAULT_DIST * 2.0, DEFAULT_DIST * 2.0 };
	neptune.rotation = DEFAULT_ROTATION * 0.75; //for the rotation time of planet while rendering
	neptune.orbitDiff = DEFAULT_ORBIT * 3.0;

	//PLUTO
	Planet pluto;
	saturn.name = "pluto";
	saturn.size = { SUN_SIZE * 0.1, SUN_SIZE * 0.1, SUN_SIZE * 0.1 };
	saturn.translate = { DEFAULT_DIST * 2.5, DEFAULT_DIST * 2.5, DEFAULT_DIST * 2.5 };
	saturn.rotation = DEFAULT_ROTATION * 0.65; //for the rotation time of planet while rendering
	saturn.orbitDiff = DEFAULT_ORBIT * 3.5;


	//array of all the planets
	Planet arrayOfPlanets[11] = { sun, moon, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto };//replace 2 with the total size of the solar system

	//rendering through a loop
	for (int i = 0; i < 11; ++i) {
		//1st parameter predefined  matrix obj //2nd parameter the rotation values //3rd parameter the rotation axis

		glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * arrayOfPlanets[i].rotation), glm::fvec3{ 0.0f, 1.0f, 0.0f });

		//1st scaling
		model_matrix = glm::scale(model_matrix, glm::fvec3(arrayOfPlanets[i].size));

		//2nd translating the sphere according to the vector we have in the 2nd parameter

		model_matrix = glm::translate(model_matrix, glm::fvec3{ arrayOfPlanets[i].translate[0], 0.0f, -1.0f });

		glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));

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
	m_shaders.emplace("planet", shader_program{ {{GL_VERTEX_SHADER,m_resource_path + "shaders/simple.vert"},
											 {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}} });
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
	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, -0.1f });
		uploadView();
	}
	else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, 0.1f });
		uploadView();
	}
	// Add move scene to right //key A

	else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ -1.0f, 0.0f, 0.0f });
		uploadView();
	}
	//Add move scene to left //key D
	else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 1.0f, 0.0f, 0.0f });
		uploadView();
	}
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
	// mouse handling
	float x = 0.0f;
	float y = 0.0f;

	//Adjusting the camera position based on the movement of the mouse
	const float moveMouseX = 0.9f;
	const float moveMouseY = 0.3f;
	//  to make the movement in X-axis
	if (pos_x > 0) {
		x = -moveMouseX;
	}
	else if (pos_x < 0) {
		x = moveMouseX;
	}
	// to make the movement in X-axis
	if (pos_y > 0) {
		y = -moveMouseY;
	}
	else if (pos_y < 0) {
		y = moveMouseY;
	}
	//When the rotation of the mouse changes

	m_view_transform = glm::rotate(m_view_transform, glm::radians(x), glm::fvec3{ 0.0f, 1.0f, 0.0f });
	m_view_transform = glm::rotate(m_view_transform, glm::radians(y), glm::fvec3{ 1.0f, 0.0f, 0.0f });

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