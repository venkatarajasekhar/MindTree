#include "lights.h"

Light::Light(Light_t type, float intensity, glm::vec4 color)
    : AbstractTransformable(LIGHT), _type(type), _intensity(intensity), _color(color)
{
}

AbstractTransformablePtr Light::clone() const
{
    auto *obj = new Light(*this);
    return std::shared_ptr<AbstractTransformable>(obj);
}

Light::Light_t Light::getLightType() const
{
    return _type;
}

PointLight::PointLight(float intensity, glm::vec4 color)
    : Light(POINT, intensity, color)
{
}

AbstractTransformablePtr PointLight::clone() const
{
    auto *obj = new PointLight(*this);
    return std::shared_ptr<PointLight>(obj);
}

SpotLight::SpotLight(float intensity, glm::vec4 color, float coneangle)
    : Light(SPOT, intensity, color), _coneAngle(coneangle)
{
}

AbstractTransformablePtr SpotLight::clone() const
{
    auto *obj = new SpotLight(*this);
    return std::shared_ptr<AbstractTransformable>(obj);
}

DistantLight::DistantLight(float intensity, glm::vec4 color)
    : Light(DISTANT, intensity, color)
{
}

AbstractTransformablePtr DistantLight::clone() const
{
    auto *obj = new DistantLight(*this);
    return std::shared_ptr<AbstractTransformable>(obj);
}

