#pragma once

#include "../BaseEffect.h"
#include <algorithm>

namespace LedPi
{
  namespace Effects
  {
    class Solid : public BaseEffect
    {
    public:
      Solid(uint16_t pixelCount): BaseEffect(pixelCount) {};
      virtual std::string GetName() const override
      {
        return "solid";
      }
      virtual void Start() override {
        if (HasConfigItem("c1"))
        {
         std::vector<uint32_t> v(m_PixelCount);
         std::fill(v.begin(), v.end(), GetConfigItem<uint32_t>("c1"));
         SetRenderData(v);
        }
      }
    };
  }
}