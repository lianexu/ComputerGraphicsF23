#include "PatchNode.hpp"

#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"

#include "glm/ext.hpp" //for printing
#include "glm/gtx/string_cast.hpp"
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

namespace GLOO {
PatchNode::PatchNode(glm::mat4 control_points_x, glm::mat4 control_points_y, glm::mat4 control_points_z, std::string spline_type) {
  shader_ = std::make_shared<PhongShader>();
  patch_mesh_ = std::make_shared<VertexObject>();
  

  // TODO: this node should represent a single tensor product patch.
  // Think carefully about what data defines a patch and how you can
  // render it.

  spline_type_ = spline_type; //B-Spline curve or Bezier curve
  control_points_x_ = control_points_x;
  control_points_y_ = control_points_y;
  control_points_z_ =control_points_z;

  PlotPatch();
  
}

PatchPoint PatchNode::EvalCurve(float u, float v) { 
  const glm::mat4 BBezier( 
    1, 0, 0, 0, -3, 3, 0, 0, 3, -6, 3, 0, -1, 3, -3, 1
  );
  const glm::mat4 BBezierT( 
    1.0,-3.0,3.0,-1.0,0.0,3.0,-6.0,3.0,0,0,3.0,-3.0,0,0,0,1.0
  );
  const glm::mat4 BBSpline(
    1/6.0, 4/6.0, 1/6.0, 0, -3/6.0, 0, 3/6.0, 0, 3/6.0, -6/6.0, 3/6.0, 0, -1/6.0, 3/6.0, -3/6.0, 1/6.0
  );
  const glm::mat4 BBSplineT(
    1.0/6,-3.0/6,3.0/6,-1.0/6,4.0/6,0,-6.0/6,3.0/6,1.0/6,3.0/6,3.0/6,-3.0/6,0,0,0,1.0/6
  );


  glm::vec4 MonomialBasisU(1, u, u*u, u*u*u);
  glm::vec4 MonomialBasisDerivativeU(0, 1, 2*u, 3*u*u);
  
  glm::vec4 MonomialBasisV(1, v, v*v, v*v*v);
  glm::vec4 MonomialBasisDerivativeV(0, 1, 2*v, 3*v*v);

  float Px;
  float Py;
  float Pz;

  glm::vec3 N;


  // P = MonomialBasisU^T * SplineBasis^T * Geometry Matrix * SplineBasis * MonomialBasisV
  if (spline_type_ == "B-Spline patch"){ //B-Spline curve
    // auto x = MonomialBasisU * BBSpline * control_points_x_ * BBSpline;
    Px = glm::dot(MonomialBasisU * BBSplineT * control_points_x_ * BBSpline, MonomialBasisV);

    // auto y = MonomialBasisU * BBSpline * control_points_y_ * BBSpline;
    Py = glm::dot(MonomialBasisU * BBSplineT * control_points_y_ * BBSpline, MonomialBasisV);

    // auto z = MonomialBasisU * BBSpline * control_points_z_ * BBSpline;
    Pz = glm::dot(MonomialBasisU * BBSplineT * control_points_z_ * BBSpline, MonomialBasisV);
    
    auto dPx_dU = glm::dot(MonomialBasisDerivativeU * BBSplineT * control_points_x_ * BBSpline, MonomialBasisV);
    auto dPy_dU = glm::dot(MonomialBasisDerivativeU * BBSplineT * control_points_y_ * BBSpline, MonomialBasisV);
    auto dPz_dU = glm::dot(MonomialBasisDerivativeU * BBSplineT * control_points_z_ * BBSpline, MonomialBasisV);

    auto dPx_dV = glm::dot(MonomialBasisU * BBSplineT * control_points_x_ * BBSpline, MonomialBasisDerivativeV);
    auto dPy_dV = glm::dot(MonomialBasisU * BBSplineT * control_points_y_ * BBSpline, MonomialBasisDerivativeV);
    auto dPz_dV = glm::dot(MonomialBasisU * BBSplineT * control_points_z_ * BBSpline, MonomialBasisDerivativeV);


    glm::vec3 dP_dU(dPx_dU, dPy_dU, dPz_dU);
    glm::vec3 dP_dV(dPx_dV, dPy_dV, dPz_dV);
    N = glm::cross(dP_dU, dP_dV); // MAKE SURE THAT THIS IS NORMALIZED!!!!!!!!!! and multiply by -1
    N = glm::normalize(N); // and multiply by -1
    // N[0] = N[0] * -1.0;
    // N[1] = N[1] * -1.0;
    // N[2] = N[2] * -1.0;


  }else{
    Px = glm::dot(MonomialBasisU * BBezierT * control_points_x_ * BBezier, MonomialBasisV);
    Py = glm::dot(MonomialBasisU * BBezierT * control_points_y_ * BBezier, MonomialBasisV);
    Pz = glm::dot(MonomialBasisU * BBezierT * control_points_z_ * BBezier, MonomialBasisV);


    auto dPx_dU = glm::dot(MonomialBasisDerivativeU * BBezierT * control_points_x_ * BBezier, MonomialBasisV);
    auto dPy_dU = glm::dot(MonomialBasisDerivativeU * BBezierT * control_points_y_ * BBezier, MonomialBasisV);
    auto dPz_dU = glm::dot(MonomialBasisDerivativeU * BBezierT * control_points_z_ * BBezier, MonomialBasisV);

    auto dPx_dV = glm::dot(MonomialBasisU * BBezierT * control_points_x_ * BBezier, MonomialBasisDerivativeV);
    auto dPy_dV = glm::dot(MonomialBasisU * BBezierT * control_points_y_ * BBezier, MonomialBasisDerivativeV);
    auto dPz_dV = glm::dot(MonomialBasisU * BBezierT * control_points_z_ * BBezier, MonomialBasisDerivativeV);


    glm::vec3 dP_dU(dPx_dU, dPy_dU, dPz_dU);
    glm::vec3 dP_dV(dPx_dV, dPy_dV, dPz_dV);
    N = glm::cross(dP_dU, dP_dV); // MAKE SURE THAT THIS IS NORMALIZED!!!!!!!!!! and multiply by -1
    N = glm::normalize(N); 
  }

  glm::vec3 P(Px,Py,Pz); 

  PatchPoint point; //= {.P=P,.T=T};
  point.P = P; // Position
  point.N = N; // Normal
  // point.N = glm::vec3(0.0,0.0,1.0);

  return point; //returns Position and Normal
}

// int ComputeIndex(int i, int j){
//   int res = i * 4 + j;
//   return res;
// }

void PatchNode::PlotPatch() { 
  auto positions = make_unique<PositionArray>(); //starter code
  auto normals = make_unique<NormalArray>(); //starter code
  auto indices = make_unique<IndexArray>(); //starter code

// TODO: fill "positions", "normals", and "indices"
  // ------ initialize points ------ //
  for (size_t u = 0; u<=N_SUBDIV_; u++) {
    for (size_t v = 0; v<=N_SUBDIV_; v++){
      glm::vec3 point = EvalCurve(float(u)/N_SUBDIV_, float(v)/N_SUBDIV_).P;
      // std::cout<<glm::to_string(point)<<std::endl;
      glm::vec3 normal = EvalCurve(float(u)/N_SUBDIV_, float(v)/N_SUBDIV_).N;
      // std::cout<<glm::to_string(normal)<<std::endl;

      positions->push_back(point); 
      normals->push_back(normal); 
    }
  }

  for (size_t i = 0; i<N_SUBDIV_; i++) {
    for (size_t j = 0; j<N_SUBDIV_; j++){
      int here = i * (N_SUBDIV_+1) + j;
      int diag = (i+1) * (N_SUBDIV_+1) + (j+1);
      int down = (i+1) * (N_SUBDIV_+1) + j;
      int right = i * (N_SUBDIV_+1) + (j+1);

      indices->push_back(here);
      indices->push_back(diag);
      indices->push_back(down);

      indices->push_back(here);
      indices->push_back(diag);
      indices->push_back(right);

    }
  }


///

  patch_mesh_->UpdatePositions(std::move(positions)); //starter code
  patch_mesh_->UpdateNormals(std::move(normals)); //starter code
  patch_mesh_->UpdateIndices(std::move(indices)); //starter code


// auto shader = std::make_shared<SimpleShader>(); 
std::unique_ptr<GLOO::SceneNode> patch_node = make_unique<SceneNode>();
patch_node->CreateComponent<ShadingComponent>(shader_);


auto& rc = patch_node->CreateComponent<RenderingComponent>(patch_mesh_);
rc.SetDrawMode(DrawMode::Triangles);

// glm::vec3 color(1.f, 1.f, 0.f);

// auto material = std::make_shared<Material>(color, color, color, 0);
// patch_node->CreateComponent<MaterialComponent>(material);

AddChild(std::move(patch_node));


}




}  // namespace GLOO
