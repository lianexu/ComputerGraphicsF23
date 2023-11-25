#ifndef PATCH_NODE_H_
#define PATCH_NODE_H_

#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"

#include "CurveNode.hpp"

namespace GLOO {
struct PatchPoint {
  glm::vec3 P;
  glm::vec3 N;
};

class PatchNode : public SceneNode {
 public:
  PatchNode(glm::mat4 control_points_x, glm::mat4 control_points_y, glm::mat4 control_points_z, std::string spline_type);
  

 private:
  void PlotPatch();
  PatchPoint EvalCurve(float u, float v);
  // int ComputeIndex(int i, int j);

  std::vector<glm::mat4> Gs_;
  SplineBasis spline_basis_;

  std::shared_ptr<VertexObject> patch_mesh_;
  std::shared_ptr<ShaderProgram> shader_;

  std::string spline_type_;
  glm::mat4 control_points_x_;
  glm::mat4 control_points_y_;
  glm::mat4 control_points_z_;

  const int N_SUBDIV_ = 50;
};
}  // namespace GLOO

#endif
