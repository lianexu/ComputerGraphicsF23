#include "SplineViewerApp.hpp"

#include <fstream>

#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/Material.hpp"
#include "gloo/components/MaterialComponent.hpp"

#include "CurveNode.hpp"
#include "PatchNode.hpp"

#include "glm/ext.hpp" //for printing
#include "glm/gtx/string_cast.hpp"
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

//create as many curve nodes as there individual curve segments

namespace GLOO {

SplineViewerApp::SplineViewerApp(const std::string& app_name,
                                 glm::ivec2 window_size,
                                 const std::string& filename)
    : Application(app_name, window_size), filename_(filename) {
}

void SplineViewerApp::SetupScene() {
  SceneNode& root = scene_->GetRootNode();

  LoadFile(filename_, root);

  auto camera_node = make_unique<ArcBallCameraNode>();
  scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
  root.AddChild(std::move(camera_node));

  auto ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(glm::vec3(0.7f));
  root.CreateComponent<LightComponent>(ambient_light);

  auto point_light = std::make_shared<PointLight>();
  point_light->SetDiffuseColor(glm::vec3(0.9f, 0.9f, 0.9f));
  point_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
  point_light->SetAttenuation(glm::vec3(1.0f, 0.09f, 0.032f));
  auto point_light_node = make_unique<SceneNode>();
  point_light_node->CreateComponent<LightComponent>(point_light);
  point_light_node->GetTransform().SetPosition(glm::vec3(0.0f, 4.0f, 5.f));
  root.AddChild(std::move(point_light_node));
}

void SplineViewerApp::LoadFile(const std::string& filename, SceneNode& root) {
  std::fstream fs(GetAssetDir() + filename);
  if (!fs) {
    std::cerr << "ERROR: Unable to open file " + filename + "!" << std::endl;
    return;
  }

  std::string spline_type; // B-Spline curve or Bezier curve
  std::getline(fs, spline_type);

  std::vector<glm::vec3> control_points; //Get control opints: vector of glm::vec3 objects corresponding to the specified control points
  std::string line;

  for (size_t i = 0; std::getline(fs, line); i++) {
    std::stringstream ss(line);
    float x, y, z;
    ss >> x >> y >> z;
    control_points.push_back(glm::vec3(x, y, z));
  }

  if (spline_type == "Bezier curve"){
    for (size_t i = 0; i<control_points.size()-3; i+=3) {
      glm::mat4x3 glm_control_points;
      glm_control_points[0] = control_points[i];
      glm_control_points[1] = control_points[i+1];
      glm_control_points[2] = control_points[i+2];
      glm_control_points[3] = control_points[i+3];
      root.AddChild(make_unique<CurveNode>(glm_control_points, spline_type));
    }
  }
  
  else if (spline_type == "B-Spline curve"){ // b-spline curve 0123,1234,2345
    for (size_t i = 0; i<control_points.size()-3; i++) {
      glm::mat4x3 glm_control_points;
      glm_control_points[0] = control_points[i];
      glm_control_points[1] = control_points[i+1];
      glm_control_points[2] = control_points[i+2];
      glm_control_points[3] = control_points[i+3];
      root.AddChild(make_unique<CurveNode>(glm_control_points, spline_type));
    }
  }
  else { // Bezier patch or B-Spline patch
    for (size_t t = 0; t<control_points.size()-15; t+=16) {
      glm::mat4 control_points_x;
      glm::mat4 control_points_y;
      glm::mat4 control_points_z;
      
      for (size_t i = 0; i<4; i++) {
        for (size_t j = 0; j<4; j++){
        control_points_x[i][j] = control_points[t+i*4+j][0];
        control_points_y[i][j] = control_points[t+i*4+j][1];
        control_points_z[i][j] = control_points[t+i*4+j][2];
        }
      }
      root.AddChild(make_unique<PatchNode>(control_points_x, control_points_y, control_points_z, spline_type));

    }


  }
}
}  // namespace GLOO
