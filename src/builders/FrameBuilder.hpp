#pragma once

#include "Builder.hpp"

class FrameBuilder : public Builder
{
protected:
  FrameBuilder();

protected:
  Result makeMeshContent(ColorationMethod coloration) const override;
};
