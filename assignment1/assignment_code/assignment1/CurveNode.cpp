#include "CurveNode.hpp"

#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/InputManager.hpp"

// #include "gloo/Material.hpp"

#include "glm/ext.hpp" //for printing
#include "glm/gtx/string_cast.hpp"

// TO-DO CHANGE COLOR OF CONTROL POINTS!! 

namespace GLOO {
CurveNode::CurveNode(glm::mat4x3 glm_control_points, std::string spline_type) { //add glm::vec4 control_points into constructor? 
  // TODO: this node should represent a single spline curve.
  // Think carefully about what data defines a curve and how you can
  // render it.

  // Initialize the VertexObjects and shaders used to render the control points,
  // the curve, and the tangent line.
  sphere_mesh_ = PrimitiveFactory::CreateSphere(0.015f, 25, 25);
  curve_polyline_ = std::make_shared<VertexObject>();
  tangent_line_ = std::make_shared<VertexObject>();
  shader_ = std::make_shared<PhongShader>();
  polyline_shader_ = std::make_shared<SimpleShader>();

  glm_control_points_ = glm_control_points;
  std::vector<SceneNode*> control_points_objects_;

  // glm::vec3 default_color = glm::vec3(1.f, 0.f, 0.f);
  // std::shared_ptr<GLOO::Material> control_points_material_ = std::make_shared<Material>(default_color, default_color, default_color, 0);
  // glm::vec3 control_points_color_;
  // std::unique_ptr<GLOO::SceneNode> line_node_;
  std::shared_ptr<GLOO::VertexObject> line_;
  std::shared_ptr<GLOO::VertexObject> tangent_line_;
  control_point_geometry_ = "Bezier basis"; //B-spline basis or Bezier basis
  spline_type_ = spline_type; //B-Spline curve or Bezier curve - how the control points are interpreted 


  InitCurve();
  PlotTangentLine();

  // InitCurve();
  // PlotTangentLine();
  // PlotControlPoints();

}

void CurveNode::Update(double delta_time) {

  // Prevent multiple toggle.
  static bool prev_released = true;

  if (InputManager::GetInstance().IsKeyPressed('T')) {
    if (prev_released) { // Interpret the control points as the other basis
      // TODO: implement toggling spline bases.
      if (spline_type_ == "B-Spline curve"){
        std::cout << "Bezier curve toggled\n"; 
        spline_type_ = "Bezier curve";

        PlotCurve();
        PlotTangentLine();
        PlotControlPoints();

      }else{
        std::cout << "B-spline curve toggled\n"; 
        spline_type_ = "B-Spline curve";

        PlotCurve();
        PlotTangentLine();
        PlotControlPoints();
      }
    }
    prev_released = false;
  } else if (InputManager::GetInstance().IsKeyPressed('B')) {
    if (prev_released) {
      // TODO: implement converting conrol point geometry from Bezier to B-Spline basis.
      std::cout << "B-spline basis toggled\n"; 
      control_point_geometry_ = "B-Spline basis";
      ConvertGeometry();
      PlotControlPoints();
      PlotCurve();
      PlotTangentLine();
    }
    prev_released = false;
  } else if (InputManager::GetInstance().IsKeyPressed('Z')) {
    if (prev_released) {
      // TODO: implement converting conrol point geometry from B-Spline to Bezier basis.
      std::cout << "Bezier basis toggled\n"; 
      control_point_geometry_ = "Bezier basis";
      ConvertGeometry();
      PlotControlPoints();
      PlotCurve();
      PlotTangentLine();
    }
    prev_released = false;
  } else {
    prev_released = true;
  }
}

void CurveNode::ToggleSplineBasis() {
  // TODO: implement toggling between Bezier and B-Spline bases.
  
}

void CurveNode::ConvertGeometry() {
  // TODO: implement converting the control points between bases.
 // glm::inverse
// G * Original Basis * New Basis Inverse

  const glm::mat4 BBezier( 
      1, 0, 0, 0, -3, 3, 0, 0, 3, -6, 3, 0, -1, 3, -3, 1
  );

  const glm::mat4 BBSpline(
      1/6.0, 4/6.0, 1/6.0, 0, -3/6.0, 0, 3/6.0, 0, 3/6.0, -6/6.0, 3/6.0, 0, -1/6.0, 3/6.0, -3/6.0, 1/6.0
  );

 if (control_point_geometry_ == "B-Spline basis"){
  glm_control_points_ = glm_control_points_ * BBezier * glm::inverse(BBSpline);
 }else{
  glm_control_points_ = glm_control_points_ * BBSpline * glm::inverse(BBezier);
  
 }

}

CurvePoint CurveNode::EvalCurve(float t) { // DONE 
  // // TODO: implement evaluating the spline curve at parameter value t.
  const glm::mat4 BBezier( 
    1, 0, 0, 0, -3, 3, 0, 0, 3, -6, 3, 0, -1, 3, -3, 1
  );

  const glm::mat4 BBSpline(
    1/6.0, 4/6.0, 1/6.0, 0, -3/6.0, 0, 3/6.0, 0, 3/6.0, -6/6.0, 3/6.0, 0, -1/6.0, 3/6.0, -3/6.0, 1/6.0
  );

  glm::vec4 MonomialBasis(1, t, t*t, t*t*t);
  glm::vec4 MonomialBasisDerivative(0, 1, 2*t, 3*t*t);

  glm::vec3 P;
  glm::vec3 T; 

  if (spline_type_ == "B-Spline curve"){ //B-Spline curve
    P = glm_control_points_ * BBSpline * MonomialBasis;
    T = glm_control_points_ * BBSpline * MonomialBasisDerivative;
  }else{
    P = glm_control_points_ * BBezier * MonomialBasis;
    T = glm_control_points_ * BBezier * MonomialBasisDerivative;
  }

  CurvePoint point; //= {.P=P,.T=T};
  point.P = P; // Position
  point.T = T; // Tangent

  return point; //returns Position and Tangent
}

void CurveNode::InitCurve() {
  // TODO: create all of the nodes and components necessary for rendering the
  // curve, its control points, and its tangent line. You will want to use the
  // VertexObjects and shaders that are initialized in the class constructor.


  // ------ initialize points ------ //
  for (size_t i = 0; i < 4; i++) {
    // std::cout<<glm::to_string(glm_control_points_[i])<<std::endl; // prints the locations of the points
    std::unique_ptr<GLOO::SceneNode> sphere_node = make_unique<SceneNode>(); // make pointer to a ScneneNode //instead of auto, std::unique_ptr<GLOO::SceneNode>
    control_points_objects_.push_back(sphere_node.get());

    Transform& sphere_node_transform = sphere_node->GetTransform(); // auto& sphere_node1_transform = sphere_node1->GetTransform();
    sphere_node_transform.SetPosition(glm_control_points_[i]);

    auto points_shader = std::make_shared<SimpleShader>(); // make shader
    sphere_node->CreateComponent<ShadingComponent>(points_shader);

    sphere_node->CreateComponent<RenderingComponent>(sphere_mesh_); // add rendering component 
    
    glm::vec3 points_color;

    if (spline_type_ == "B-Spline curve"){ //B-Spline curve
      points_color = glm::vec3(0.f, 1.f, 0.f); // red 
    }else{
      points_color = glm::vec3(1.f, 0.f, 0.f); // green 
    }
    // control_points_material_->SetAmbientColor(points_color);
    // control_points_material_->SetDiffuseColor(points_color);
    // control_points_material_->SetSpecularColor(points_color);
    auto control_points_material = std::make_shared<Material>(points_color, points_color, points_color, 0); // color white //HEREEEE!!!!!!!!!! CHANGED HERE!!!!!
    sphere_node->CreateComponent<MaterialComponent>(control_points_material); // add material component 

    AddChild(std::move(sphere_node));
}

// ------ initialize curve AKA line ------ //
  line_ = std::make_shared<VertexObject>(); 
  auto positions = make_unique<PositionArray>(); 
  for (size_t i = 0; i < N_SUBDIV_; i++) { //t = 0, 1/50, 2/50, ... , 49/50
    positions->push_back(
      EvalCurve(float(i)/N_SUBDIV_).P); 
  }
  positions->push_back(EvalCurve(1).P); //t = 1

  auto indices = make_unique<IndexArray>(); 
  indices->push_back(0); 
  for (size_t i = 1; i < N_SUBDIV_; i++) {
    indices->push_back(i); 
    indices->push_back(i); 
  }
  indices->push_back(N_SUBDIV_); 

  line_->UpdatePositions(std::move(positions));
  line_->UpdateIndices(std::move(indices));

  auto shader = std::make_shared<SimpleShader>(); 

  std::unique_ptr<GLOO::SceneNode> line_node = make_unique<SceneNode>();
  line_node->CreateComponent<ShadingComponent>(shader);

  auto& rc = line_node->CreateComponent<RenderingComponent>(line_);
  rc.SetDrawMode(DrawMode::Lines);

  glm::vec3 color(1.f, 1.f, 0.f);

  auto material = std::make_shared<Material>(color, color, color, 0);
  line_node->CreateComponent<MaterialComponent>(material);

  AddChild(std::move(line_node));

  // ----- initialize tangent line ----- //
  tangent_line_ = std::make_shared<VertexObject>();

  float line_length = 0.1;
  
  glm::vec3 center_point = EvalCurve(0.5).P;
  glm::vec3 tangent = glm::normalize(EvalCurve(0.5).T);
  glm::vec3 left_point = center_point + tangent * glm::vec3(line_length, line_length, line_length);
  glm::vec3 right_point = center_point + tangent * glm::vec3(-line_length, -line_length, -line_length);

  std::unique_ptr<PositionArray> tangent_positions = make_unique<PositionArray>(); 
  tangent_positions->push_back(left_point); //the vertext at index 0
  tangent_positions->push_back(center_point); //the vertex at index 1
  tangent_positions->push_back(right_point);

  std::unique_ptr<IndexArray> tangent_indices = make_unique<IndexArray>(); 
  tangent_indices->push_back(0); 
  tangent_indices->push_back(1); 
  tangent_indices->push_back(1); 
  tangent_indices->push_back(2); 

  tangent_line_->UpdatePositions(std::move(tangent_positions));
  tangent_line_->UpdateIndices(std::move(tangent_indices));

  auto tangent_shader = std::make_shared<SimpleShader>(); //Shader tells the color

  auto tangent_line_node = make_unique<SceneNode>();
  tangent_line_node->CreateComponent<ShadingComponent>(tangent_shader);

  auto& tangent_rc = tangent_line_node->CreateComponent<RenderingComponent>(tangent_line_);
  tangent_rc.SetDrawMode(DrawMode::Lines);

  glm::vec3 tangent_color(1.f, 1.f, 1.f);
  auto tangent_material = std::make_shared<Material>(tangent_color, tangent_color, tangent_color, 0);
  tangent_line_node->CreateComponent<MaterialComponent>(tangent_material);

  AddChild(std::move(tangent_line_node));

}

void CurveNode::PlotCurve() {
  // TODO: plot the curve by updating the positions of its VertexObject.
  std::unique_ptr<PositionArray> positions = make_unique<PositionArray>(); 
  for (size_t i = 0; i < N_SUBDIV_; i++) { //t = 0, 1/50, 2/50, ... , 49/50
    positions->push_back(
      EvalCurve(float(i)/N_SUBDIV_).P); 
  }
  positions->push_back(EvalCurve(1).P); //t = 1

  std::unique_ptr<IndexArray> indices = make_unique<IndexArray>();
  indices->push_back(0); 
  for (size_t i = 1; i < N_SUBDIV_; i++) { // N_SUBDIV_-1 
    indices->push_back(i); 
    indices->push_back(i); 
  }
  indices->push_back(N_SUBDIV_);  //N_SUBDIV_-1

  line_->UpdatePositions(std::move(positions));
  line_->UpdateIndices(std::move(indices));
}

void CurveNode::PlotControlPoints() { 
  //plot the curve control points
  for (size_t i = 0; i < 4; i++) {
    SceneNode* sphere_node = control_points_objects_[i];

    // UPDATE COLOR 
    glm::vec3 points_color;
    if (spline_type_ == "B-Spline curve"){ //B-Spline curve
      points_color = glm::vec3(0.f, 1.f, 0.f); // red 
    }else{
      points_color = glm::vec3(1.f, 0.f, 0.f); // green 
    }
    Material& material = sphere_node->GetComponentPtr<MaterialComponent>()->GetMaterial();
    material.SetAmbientColor(points_color);
    material.SetDiffuseColor(points_color);
    material.SetSpecularColor(points_color);

    Transform& sphere_node_transform = sphere_node->GetTransform();
    sphere_node_transform.SetPosition(glm_control_points_[i]);
  }
}

void CurveNode::PlotTangentLine() {
  // TODO: implement plotting a line tangent to the curve.
  // Below is a sample implementation for rendering a line segment
  // onto the screen. Note that this is just an example. This code
  // currently has nothing to do with the spline.

  float line_length = 0.1;
  
  glm::vec3 center_point = EvalCurve(0.5).P;
  glm::vec3 tangent = glm::normalize(EvalCurve(0.5).T);
  glm::vec3 left_point = center_point + tangent * glm::vec3(line_length, line_length, line_length);
  glm::vec3 right_point = center_point + tangent * glm::vec3(-line_length, -line_length, -line_length);

  std::unique_ptr<PositionArray> positions = make_unique<PositionArray>(); 
  positions->push_back(left_point); //the vertext at index 0
  positions->push_back(center_point); //the vertex at index 1
  positions->push_back(right_point);

  std::unique_ptr<IndexArray> indices = make_unique<IndexArray>(); 
  indices->push_back(0); 
  indices->push_back(1); 
  indices->push_back(1); 
  indices->push_back(2); 

  tangent_line_->UpdatePositions(std::move(positions));
  tangent_line_->UpdateIndices(std::move(indices));
}
}  // namespace GLOO