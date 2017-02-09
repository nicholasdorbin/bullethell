#include "Engine/Renderer/Shaders/Light.hpp"


//-----------------------------------------------------------------------------------------------
Light::Light()
    : m_color(Vector4(1.0f, 1.0f, 1.0f, 1.0f))
    , m_position(Vector3(0.0f, 0.0f, 0.0f))
    , m_direction(Vector3(0.0f, 0.0f, 0.0f))
    , m_minLightDistance(0.f)
    , m_maxLightDistance(0.f)
    , m_powerAtMin(0.f)
    , m_powerAtMax(0.f)
    , m_directionalInterp(0.f)
    , m_thetaInner(0.f)
    , m_thetaOuter(0.f)
    , m_thetaInnerPower(0.f)
    , m_thetaOuterPower (0.f)
{
};
//-----------------------------------------------------------------------------------------------
//Point Lights
//-----------------------------------------------------------------------------------------------
Light::Light( const Vector4& color, const Vector3& pos, float radius)
    : m_position(pos)
    , m_direction(Vector3(0.f, 0.f, 0.f))
    , m_color(color)
    , m_minLightDistance(radius - 1.f)
    , m_maxLightDistance(radius + 1.f)
    , m_powerAtMin(1.f)
    , m_powerAtMax(0.f)
    , m_directionalInterp(0.f)
    , m_thetaInner(1.f)
    , m_thetaOuter(-1.f)
    , m_thetaInnerPower(1.f)
    , m_thetaOuterPower(1.f)
{ }


//-----------------------------------------------------------------------------------------------
Light::Light(const Vector4& color, const Vector3& position, const Vector3& direction, float minLightDist, float maxLightDist,
    float powerAtMin, float powerAtMax, float dirInterp,
    float thetaInner, float thetaOuter,
    float thetaInnerPower, float thetaOuterPower
    )
    : m_position(position)
    , m_direction(direction)
    , m_color(color)
    , m_minLightDistance(minLightDist)
    , m_maxLightDistance(maxLightDist)
    , m_powerAtMin(powerAtMin)
    , m_powerAtMax(powerAtMax)
    , m_directionalInterp(dirInterp)
    , m_thetaInner(thetaInner)
    , m_thetaOuter(thetaOuter)
    , m_thetaInnerPower(thetaInnerPower)
    , m_thetaOuterPower(thetaOuterPower)

{
}
