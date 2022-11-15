#pragma once

#include "Builder.hpp"

class SphereBuilder : public Builder
{
protected:
    SphereBuilder(float radius);

public:
    float getRadius() const;

protected:
    Result makeMeshContent(ColorationMethod coloration) const override;

protected:
    float m_radius;
};