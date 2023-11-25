#include "SkeletonNode.hpp"

#include "gloo/utils.hpp"
#include "gloo/InputManager.hpp"
#include "gloo/MeshLoader.hpp"

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"

#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/InputManager.hpp"
// #include <random>

#include <fstream>

#include "glm/ext.hpp" //for printing
#include "glm/gtx/string_cast.hpp"
#include <glm/vec3.hpp>
#include <glm/glm.hpp>


namespace GLOO {
SkeletonNode::SkeletonNode(const std::string& filename)
    : SceneNode(), draw_mode_(DrawMode::Skeleton) {


  // my stuff 
  sphere_mesh_ = PrimitiveFactory::CreateSphere(0.025f, 25, 25);
  vertex_mesh_ = PrimitiveFactory::CreateSphere(0.005f, 25, 25);
  cylinder_mesh_ = PrimitiveFactory::CreateCylinder(0.015f, 1.0f, 25);
  skin_mesh_ = std::make_shared<VertexObject>();
  shader_ = std::make_shared<PhongShader>();
  // skin_shader_ = std::make_shared<PhongShader>();

  std::vector<SceneNode*> skeleton_joints_;
  std::vector<SceneNode*> sphere_nodes_ptrs_;
  std::vector<SceneNode*> cylinders_nodes_ptrs_;
  std::vector<glm::mat4> B_Inverse_mat_; // World to Local BEFORE animation


  LoadAllFiles(filename);
  DecorateTree();
  // OnJointChanged();
}

void SkeletonNode::ToggleDrawMode() {
  draw_mode_ =
      draw_mode_ == DrawMode::Skeleton ? DrawMode::SSD : DrawMode::Skeleton;
  // TODO: implement here toggling between skeleton mode and SSD mode.
  // The current mode is draw_mode_;
  // Hint: you may find SceneNode::SetActive convenient here as
  // inactive nodes will not be picked up by the renderer.
  
  int count = GetChildrenCount();
  if (draw_mode_ == DrawMode::Skeleton){
    // int count = GetChildrenCount();
    GetChild(0).SetActive(true); // activate skeleton
    for (size_t x = 1; x < count; x++){
      GetChild(x).SetActive(false); // disable skin mesh 
    }
    
  } else { // draw mode SSD 
    for (size_t x = 1; x < count; x++){
      GetChild(x).SetActive(true); // activate skin
    }
    GetChild(0).SetActive(false); // disable bones and stuff
  }


}

void SkeletonNode::DecorateTree() {
std::cout << "DecorateTree" << std::endl;
  // TODO: set up addtional nodes, add necessary components here.
  // You should create one set of nodes/components for skeleton mode
  // (spheres for joints and cylinders for bones), and another set for
  // SSD mode (you could just use a single node with a RenderingComponent
  // that is linked to a VertexObject with the mesh information. Then you
  // only need to update the VertexObject - updating vertex positions and
  // recalculating the normals, etc.).

  // The code snippet below shows how to add a sphere node to a joint.
  // Suppose you have created member variables shader_ of type
  // std::shared_ptr<PhongShader>, and sphere_mesh_ of type
  // std::shared_ptr<VertexObject>.
  // Here sphere_nodes_ptrs_ is a std::vector<SceneNode*> that stores the
  // pointer so the sphere nodes can be accessed later to change their
  // positions. joint_ptr is assumed to be one of the joint node you created
  // from LoadSkeletonFile (e.g. you've stored a std::vector<SceneNode*> of
  // joint nodes as a member variable and joint_ptr is one of the elements).
  //
  // auto sphere_node = make_unique<SceneNode>();
  // sphere_node->CreateComponent<ShadingComponent>(shader_);
  // sphere_node->CreateComponent<RenderingComponent>(sphere_mesh_);
  // sphere_nodes_ptrs_.push_back(sphere_node.get());
  // joint_ptr->AddChild(std::move(sphere_node));

// if (draw_mode_ == DrawMode::Skeleton){ // if draw mode == SSD, make Sphere and Cylinder radii = 0!
  // initialize bones cylinders
  for (size_t i = 0; i<skeleton_joints_.size(); i++) {
    int num_children_joints = skeleton_joints_[i]->GetChildrenCount(); // std::cout << "children: " << num_children_joints << std::endl;
    for (size_t j = 0; j<num_children_joints; j++){ // std::cout << "j: " << j << std::endl;
      SceneNode& child_joint = skeleton_joints_[i]->GetChild(j);
      std::unique_ptr<GLOO::SceneNode> cylinder_node = make_unique<SceneNode>();
      cylinder_node->CreateComponent<ShadingComponent>(shader_);
      cylinder_node->CreateComponent<RenderingComponent>(cylinder_mesh_);
      Transform& cylinder_node_transform = cylinder_node->GetTransform();

      // setting the position
      cylinder_node_transform.SetPosition(glm::vec3(0.f,0.f,0.f));   

      // setting the rotation
      glm::vec3 child_joint_position = child_joint.GetTransform().GetPosition();
      glm::vec3 rot_axis = glm::normalize(glm::cross(glm::vec3(0,1.0f,0), child_joint_position));
      float rot_angle = glm::acos(glm::dot(glm::vec3(0,1.0f,0), child_joint_position)/glm::length(child_joint_position)); // std::cout<<glm::to_string(local_to_parent*)<<std::endl;
      cylinder_node_transform.SetRotation(rot_axis, rot_angle);

      // scaling the length
      float scale_length = glm::distance(glm::vec3(0,0,0), child_joint_position);
      cylinder_node_transform.SetScale(glm::vec3(1.0f, scale_length, 1.0f));   // **
    
      skeleton_joints_[i]->AddChild(std::move(cylinder_node)); // **
      cylinders_nodes_ptrs_.push_back(cylinder_node.get());

    }

    // initialize joints spheres
    std::unique_ptr<GLOO::SceneNode> sphere_node = make_unique<SceneNode>();
    sphere_node->CreateComponent<ShadingComponent>(shader_);
    sphere_node->CreateComponent<RenderingComponent>(sphere_mesh_);
    sphere_nodes_ptrs_.push_back(sphere_node.get());
    skeleton_joints_[i]->AddChild(std::move(sphere_node)); // "attach it as a child to each joint node"
  }

// }

// if (draw_mode_ == DrawMode::SSD){
  // else{

//initializing B inverse matrix
  for (size_t j = 0; j<skeleton_joints_.size(); j++){ // B_Inverse_mat_ = vector of B_Inverse matricies, one for each joint (World to Local BEFORE animation)
    glm::mat4 B = skeleton_joints_[j]->GetTransform().GetLocalToWorldMatrix();
    glm::mat4 B_inverse = glm::inverse(B);
    B_Inverse_mat_.push_back(B_inverse);
  }

  //initializing normal vectors
  auto indices = vtx_obj_->GetIndices();
  std::unique_ptr<NormalArray> norm_unique = make_unique<NormalArray>(); 

  std::vector<float> total_vertex_weight(bind_vertices_.size(), 0.0f);
  for (auto vertex : bind_vertices_){
    bind_normals_.push_back(glm::vec3(0.0,0.0,0.0));
  }

  for (size_t y = 0; y < indices.size()-2; y+=3){
    int A = indices[y];
    int B = indices[y+2];
    int C = indices[y+1];

    auto AB = bind_vertices_[A]-bind_vertices_[B];
    auto BC = bind_vertices_[C]-bind_vertices_[B];
    auto AB_BC = glm::cross(AB,BC);
    float AB_BC_length = glm::length(AB_BC);
    glm::vec3 face_normal = AB_BC/AB_BC_length;
    float face_normal_weight = AB_BC_length/2.0f;

    total_vertex_weight[A] = total_vertex_weight1[A] + face_normal_weight;
    total_vertex_weight[B] = total_vertex_weight[B] + face_normal_weight;
    total_vertex_weight[C] = total_vertex_weight[C] + face_normal_weight;

    bind_normals_[A] = bind_normals_[A] + face_normal_weight * face_normal;
    bind_normals_[B] = bind_normals_[B] + face_normal_weight * face_normal;
    bind_normals_[C] = bind_normals_[C] + face_normal_weight * face_normal;
  } 

  for (size_t y = 0; y < bind_normals_.size(); y++){
    bind_normals_[y] = bind_normals_[y]/total_vertex_weight[y];
  }

  for (auto norm : bind_normals_){
    norm_unique->push_back(norm);
  }

  vtx_obj_->UpdateNormals(std::move(norm_unique));

  std::unique_ptr<GLOO::SceneNode> skin_node = make_unique<SceneNode>();
  skin_node->CreateComponent<RenderingComponent>(vtx_obj_);
  skin_node->CreateComponent<ShadingComponent>(shader_);
  AddChild(std::move(skin_node)); 


  if (draw_mode_ == DrawMode::Skeleton){
    int count = GetChildrenCount();
    for (size_t x = 1; x < count; x++){
      GetChild(x).SetActive(false); // disable skin mesh 
    }
  } else {
    GetChild(0).SetActive(false); // disable bones and stuff
  }


// }


}

void SkeletonNode::Update(double delta_time) {
  // Prevent multiple toggle.
  static bool prev_released = true;
  if (InputManager::GetInstance().IsKeyPressed('S')) {
    if (prev_released) {
      ToggleDrawMode();
    }
    prev_released = false;
  } else if (InputManager::GetInstance().IsKeyReleased('S')) {
    prev_released = true;
  }
}


void SkeletonNode::OnJointChanged() {
  // TODO: this method is called whenever the values of UI sliders change.
  // The new Euler angles (represented as EulerAngle struct) can be retrieved
  // from linked_angles_ (a std::vector of EulerAngle*).
  // The indices of linked_angles_ align with the order of the joints in .skel
  // files. For instance, *linked_angles_[0] corresponds to the first line of
  // the .skel file.
  // std::cout << "OnJointChanged" << std::endl;
  for (size_t i = 0; i<linked_angles_.size(); i++){
    auto joint = skeleton_joints_[i];
    auto angle = linked_angles_[i];
    glm::quat rotation(glm::vec3(angle->rx,angle->ry,angle->rz));
    joint->GetTransform().SetRotation(rotation);
  }

  auto pos_unique = make_unique<PositionArray>(); // updated positions
  auto norm_unique = make_unique<NormalArray>(); // updated positions
  std::vector<glm::vec3> positions;


  for (size_t x = 0; x < bind_vertices_.size(); x++){ // for each bind position
    std::vector<float> this_vertex_weights = weights_matrix_[x]; // the weights for this specific vertex

    glm::vec4 point_homo_orig(bind_vertices_[x][0],bind_vertices_[x][1],bind_vertices_[x][2],1.0);
    glm::vec4 point_homo(0.0,0.0,0.0,0.0);

    glm::mat4 normal_mat_sum;
  
    for (size_t y = 0; y < this_vertex_weights.size(); y++){ // loop through vertex weights (vertex index is y+1)
      float weight = this_vertex_weights[y];
      glm::mat4 T = skeleton_joints_[y+1]->GetTransform().GetLocalToWorldMatrix();
      glm::mat4 B_inverse = B_Inverse_mat_[y+1]; 
      point_homo = point_homo + weight*T*B_inverse*point_homo_orig;
      normal_mat_sum = normal_mat_sum + weight*T*B_inverse;
    }
    glm::vec4 normal_homo_orig(bind_normals_[x][0],bind_normals_[x][1],bind_normals_[x][2],0.0);
    normal_mat_sum = glm::transpose(glm::inverse(normal_mat_sum));
    glm::vec3 new_norm = glm::vec3(normal_mat_sum * normal_homo_orig);

    // norm_unique->push_back(glm::normalize(new_norm));
    pos_unique->push_back(glm::vec3(point_homo));
    positions.push_back(glm::vec3(point_homo));
  }
  // vtx_obj_->UpdateNormals(std::move(norm_unique));
  vtx_obj_->UpdatePositions(std::move(pos_unique));

  ////// ------ ////

  // updating the normals
  auto indices = vtx_obj_->GetIndices();
  // std::unique_ptr<NormalArray> norm_unique = make_unique<NormalArray>(); 
  std::vector<glm::vec3> normals; 

  std::vector<float> total_vertex_weight(bind_vertices_.size(), 0.0f);
  for (auto vertex : bind_vertices_){
    normals.push_back(glm::vec3(0.0,0.0,0.0));
  }

  for (size_t y = 0; y < indices.size()-2; y+=3){
    int A = indices[y];
    int B = indices[y+2];
    int C = indices[y+1];

    auto AB = positions[A]-positions[B];
    auto BC = positions[C]-positions[B];
    auto AB_BC = glm::cross(AB,BC);
    float AB_BC_length = glm::length(AB_BC);
    glm::vec3 face_normal = AB_BC/AB_BC_length;
    float face_normal_weight = AB_BC_length/2.0f;

    total_vertex_weight[A] = total_vertex_weight[A] + face_normal_weight;
    total_vertex_weight[B] = total_vertex_weight[B] + face_normal_weight;
    total_vertex_weight[C] = total_vertex_weight[C] + face_normal_weight;


    normals[A] = normals[A] + face_normal_weight * face_normal;
    normals[B] = normals[B] + face_normal_weight * face_normal;
    normals[C] = normals[C] + face_normal_weight * face_normal;


  } 

  for (size_t y = 0; y < normals.size(); y++){
    normals[y] = normals[y]/total_vertex_weight[y];
  }

  for (auto norm : normals){
    norm_unique->push_back(norm);
  }

  vtx_obj_->UpdateNormals(std::move(norm_unique));

}

void SkeletonNode::LinkRotationControl(const std::vector<EulerAngle*>& angles) {
  linked_angles_ = angles;
}

void SkeletonNode::LoadSkeletonFile(const std::string& path) {
  // TODO: load skeleton file and build the tree of joints.
  std::fstream fs(path);

  if (!fs) {
    std::cerr << "ERROR: Unable to open file " + path + "!" << std::endl;
    return;
  }

  std::string line;
  for (size_t i = 0; std::getline(fs, line); i++) {
    std::stringstream ss(line);
    float a, b, c, d;
    ss >> a >> b >> c >> d;
    std::unique_ptr<GLOO::SceneNode> skeleton_node = make_unique<SceneNode>(); // "create a SceneNode for each joint..."
    skeleton_node->GetTransform().SetPosition(glm::vec3(a,b,c)); 
    skeleton_joints_.push_back(skeleton_node.get()); // "we store pointers to these joint nodes at private member variables in SkeletonNode"
    
    // "... and add them to SkeletonNode obeying the same hierarchy from the skeleton file"
    if (i==0){
      AddChild(std::move(skeleton_node)); 
    }
    if (d>=0){
      skeleton_joints_[d]->AddChild(std::move(skeleton_node));
    }

    // std::cout<<glm::to_string(glm::vec4(a,b,c,d))<<std::endl;
  }





}

void SkeletonNode::LoadMeshFile(const std::string& filename) {
  std::shared_ptr<VertexObject> vtx_obj = MeshLoader::Import(filename).vertex_obj; // from provided code
  // TODO: store the bind pose mesh in your preferred way. eg as a member variable of the skeletonnode class
  vtx_obj_ = vtx_obj; 
  bind_vertices_ = vtx_obj_->GetPositions();
}

void SkeletonNode::LoadAttachmentWeights(const std::string& path) {
  // TODO: load attachment weights.
  //std::vector<std::vector<float>> weights_matrix_;
  std::fstream fs(path);

  std::cerr << path << std::endl;

  if (!fs) {
    std::cerr << "ERROR: Unable to open file " + path + "!" << std::endl;
    return;
  }

  std::string line;
  for (size_t i = 0; std::getline(fs, line); i++) {
    std::stringstream ss(line);
    std::vector<float> row;
    float num;
    while (ss >> num){
      row.push_back(num);
    }
  weights_matrix_.push_back(row);
  }

}

void SkeletonNode::LoadAllFiles(const std::string& prefix) {
  std::string prefix_full = GetAssetDir() + prefix;

  LoadSkeletonFile(prefix_full + ".skel");
  LoadMeshFile(prefix + ".obj");
  // LoadMeshFile(prefix_full + ".obj");
  LoadAttachmentWeights(prefix_full + ".attach");
}
}  // namespace GLOO
