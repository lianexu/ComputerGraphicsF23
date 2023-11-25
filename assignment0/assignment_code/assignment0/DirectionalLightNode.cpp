#include "DirectionalLightNode.hpp"

#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/lights/DirectionalLight.hpp"

#include "gloo/external.hpp"
#include "gloo/InputManager.hpp"
#include "gloo/SceneNode.hpp"

#include <glm/vec3.hpp> // glm::vec3


namespace GLOO {
DirectionalLightNode::DirectionalLightNode() { // Constructor
	std::shared_ptr<DirectionalLight> directional_light = std::make_shared<DirectionalLight>();
	directional_light->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
	directional_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
	
	CreateComponent<LightComponent>(directional_light);
	directional_light->SetDirection(glm::vec3(2.0f, 4.0f, 5.0f));
}

}

