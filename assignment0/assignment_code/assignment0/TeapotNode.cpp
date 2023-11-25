#include "TeapotNode.hpp"

#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"

#include "gloo/MeshLoader.hpp"
#include "gloo/InputManager.hpp"
#include "gloo/SceneNode.hpp"

#include <glm/vec3.hpp> // glm::vec3

namespace GLOO {
TeapotNode::TeapotNode() { // Constructor
	std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();
	std::shared_ptr<VertexObject> mesh =
		MeshLoader::Import("assignment0/teapot.obj").vertex_obj;
	if (mesh == nullptr) {
		return;
	}

	CreateComponent<ShadingComponent>(shader);
	CreateComponent<RenderingComponent>(mesh);
	GetTransform().SetPosition(glm::vec3(0.f, 0.f, 0.f));
	GetTransform().SetRotation(glm::vec3(1.0f, 0.0f, 0.0f), 0.3f);

	CreateComponent<MaterialComponent>(
		std::make_shared<Material>(Material::GetDefault())); // default red material
}


void TeapotNode::Update(double delta_time) {
	static bool prev_released = true;
	if (InputManager::GetInstance().IsKeyPressed('C')) {
		if (prev_released) {
			glm::vec3 ambient_rgb = glm::vec3((float) (rand() % 100) / 100, (float) (rand() % 100) / 100, (float) (rand() % 100) / 100);
			glm::vec3 diffuse_rgb = glm::vec3((float) (rand() % 100) / 100, (float) (rand() % 100) / 100, (float) (rand() % 100) / 100);
			MaterialComponent* material_ptr = GetComponentPtr<MaterialComponent>(); // returns pointer to requested component if it exists, nullptr otherwise
			Material* material = &material_ptr->GetMaterial(); // get a reference to the material to change it
			material->SetAmbientColor(ambient_rgb);
			material->SetDiffuseColor(diffuse_rgb);
		}
		prev_released = false;
	} else if (InputManager::GetInstance().IsKeyReleased('C')) {
		prev_released = true;
	}
}
}