#ifndef POINT_LIGHT_NODE_H_
#define POINT_LIGHT_NODE_H_
#include "gloo/SceneNode.hpp"
namespace GLOO {
class PointLightNode : public SceneNode {
    public:
        PointLightNode();
        void Update(double delta_time) override;
		float point_light_x = 0.0f;
		float point_light_y = 4.0f;
		float point_light_z = 5.0f;
};
}
#endif