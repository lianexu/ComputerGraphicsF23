#ifndef CURVE_NODE_H_
#define CURVE_NODE_H_

#include <string>
#include <vector>

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"


namespace GLOO {

enum class SplineBasis { Bezier, BSpline };

struct CurvePoint {
  glm::vec3 P;
  glm::vec3 T;
};

class CurveNode : public SceneNode {
 public:
  CurveNode(glm::mat4x3  glm_control_points, std::string spline_type);
  void Update(double delta_time) override;
  

 private:
  void ToggleSplineBasis();
  void ConvertGeometry();
  CurvePoint EvalCurve(float t);
  
  void InitCurve();
  void PlotCurve();
  void PlotControlPoints();
  void PlotTangentLine();

  SplineBasis spline_basis_;

  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<VertexObject> curve_polyline_;

  std::shared_ptr<ShaderProgram> shader_;
  std::shared_ptr<ShaderProgram> polyline_shader_;

  glm::mat4x3  glm_control_points_;
  std::string spline_type_;
  std::string control_point_geometry_;
  std::vector<SceneNode*> control_points_objects_;
  // glm::vec3 control_points_color_;
  // std::shared_ptr<GLOO::Material> control_points_material_;

  std::shared_ptr<GLOO::VertexObject> line_;
  std::shared_ptr<GLOO::VertexObject> tangent_line_;
  //make_unique<SceneNode>();
  const int N_SUBDIV_ = 50;
};
}  // namespace GLOO

#endif
