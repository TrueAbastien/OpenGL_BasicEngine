#pragma once

#include "Builder.hpp"

class WFSphereBuilder : public Builder
{
protected:
    WFSphereBuilder(float radius);

public:
    float getRadius() const;

protected:
    Result makeMeshContent(ColorationMethod coloration) const override;

protected:
    float m_radius;
};