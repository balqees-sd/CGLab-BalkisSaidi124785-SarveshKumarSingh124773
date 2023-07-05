#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "scene_graph.hpp"
#include "string"

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // react to key input
  void keyCallback(int key, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  //handle resizing
  void resizeCallback(unsigned width, unsigned height);

  // draw all objects
  void render() const;

 protected:
  void initializeSceneGraph();
  void initializeShaderPrograms();
  void initializeGeometry();
  // update uniform values
  void uploadUniforms();
  // upload projection matrix
  void uploadProjection();
  // upload view matrix
  void uploadView();
  
  //texture init
  void initializeTextures();
  bool initializeFramebuffer(unsigned width, unsigned height);
  void initializeScreenquad();
  
  // cpu representation of model
  model_object planet_object;
  model_object stars_object;
  model_object orbits_object;
  model_object skybox_object;
  model_object screenquad_object;
  framebuffer_object framebuffer_obj;

  // vector of planet names
  std::vector<std::string> m_planet_names;

  //boolean to show if planets should move or not
  bool time_stop = false;

  // scenegraph
  SceneGraph* scenegraph_;

  // container for textures
  std::map<std::string, texture_object> m_textures{};
  GLenum active_skymap_texture;
  texture_object skymap_texture;

  std::string m_current_planet_shader;

  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;
  
  // init and render individual geometry
  void initializePlanets();
  void initializeStars();
  void initializeOrbits();
  void initializeSkybox();
  void renderPlanets(std::string const& planet_shader) const;
  void renderStars() const;
  void renderOrbits() const;
  void renderSkybox() const;
  void renderQuad() const;

  private:
  bool horizontal_mirroring = false;
  bool vertical_mirroring = false;
  bool greyscale = false;
  bool blur = false;
  unsigned img_width;
  unsigned img_height;
};

#endif