#ifndef SKELETON_NODE_H_
#define SKELETON_NODE_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"

#include <string>
#include <vector>

namespace GLOO {
class SkeletonNode : public SceneNode {
 public:
  enum class DrawMode { Skeleton, SSD };
  struct EulerAngle {
    float rx, ry, rz;
  };

  SkeletonNode(const std::string& filename);
  void LinkRotationControl(const std::vector<EulerAngle*>& angles);
  void Update(double delta_time) override;
  void OnJointChanged();

 private:
  void LoadAllFiles(const std::string& prefix);
  void LoadSkeletonFile(const std::string& path);
  void LoadMeshFile(const std::string& filename);
  void LoadAttachmentWeights(const std::string& path);

  void ToggleDrawMode();
  void DecorateTree();

  DrawMode draw_mode_;
  // Euler angles of the UI sliders.
  std::vector<EulerAngle*> linked_angles_;


  // my stuff
  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<VertexObject> cylinder_mesh_;
  std::shared_ptr<VertexObject> vertex_mesh_;
  std::shared_ptr<VertexObject> skin_mesh_;
  std::shared_ptr<ShaderProgram> shader_;
  // std::vector<std::unique_ptr<GLOO::SceneNode>> skeleton_joints_;
  // std::vector<std::unique_ptr<GLOO::SceneNode>> sphere_nodes_ptrs_;
  std::vector<SceneNode*> skeleton_joints_;
  std::vector<SceneNode*> sphere_nodes_ptrs_;
  std::vector<SceneNode*> cylinders_nodes_ptrs_;

  std::shared_ptr<VertexObject> vtx_obj_;
  std::vector<std::vector<float>> weights_matrix_;
  std::vector<glm::mat4> B_Inverse_mat_;
  std::vector<glm::vec3> bind_vertices_;
  std::vector<glm::vec3> bind_normals_;
  // const PositionArray& bind_verts_;
  std::vector<glm::vec3> mesh_indices_; 

  // std::ranlux48 gen;
  // std::uniform_int_distribution<int> uniform_0_255(0,255);

  // VertexObject* vtx_obj_;

};
}  // namespace GLOO

#endif
