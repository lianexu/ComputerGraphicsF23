#include "PointLightNode.hpp"

#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/lights/PointLight.hpp"

#include "gloo/InputManager.hpp"
#include "gloo/SceneNode.hpp"
#include "gloo/external.hpp"

#include <glm/vec3.hpp> // glm::vec3



namespace GLOO {
PointLightNode::PointLightNode() { // Constructor
	std::shared_ptr<PointLight> point_light = std::make_shared<PointLight>();
	point_light->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
	point_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
	point_light->SetAttenuation(glm::vec3(1.0f, 0.09f, 0.032f));
	CreateComponent<LightComponent>(point_light);
	GetTransform().SetPosition(glm::vec3(point_light_x, point_light_y, point_light_z));
}


void PointLightNode::Update(double delta_time) {
	static bool prev_released = true;
	if (InputManager::GetInstance().IsKeyPressed(265)) { //up
		if (prev_released) {
			point_light_y++;
			GetTransform().SetPosition(glm::vec3(point_light_x, point_light_y, point_light_z));
		}
		prev_released = false;
	} else if (InputManager::GetInstance().IsKeyPressed(264)) { //down
		if (prev_released) {
			point_light_y--;
			GetTransform().SetPosition(glm::vec3(point_light_x, point_light_y, point_light_z));
		}
		prev_released = false;
	} else if (InputManager::GetInstance().IsKeyPressed(263)) { //left
		if (prev_released) {
			point_light_x--;
			GetTransform().SetPosition(glm::vec3(point_light_x, point_light_y, point_light_z));
		}
		prev_released = false;
	} else if (InputManager::GetInstance().IsKeyPressed(262)) { //right
		if (prev_released) {
			point_light_x++;
			GetTransform().SetPosition(glm::vec3(point_light_x, point_light_y, point_light_z));
		}
		prev_released = false;
	} else if (InputManager::GetInstance().IsKeyReleased(265)
			&& InputManager::GetInstance().IsKeyReleased(264)
			&& InputManager::GetInstance().IsKeyReleased(263)
			&& InputManager::GetInstance().IsKeyReleased(262)) {
		prev_released = true;
	}
}
}