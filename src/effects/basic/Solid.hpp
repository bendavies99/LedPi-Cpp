#pragma once

#include "../IEffect.h"
#include <algorithm>

namespace LedPi
{
  namespace Effects
  {
    class Solid : public IEffect
    {
    public:
      virtual std::string GetName() const override
      {
        return "solid";
      }
      virtual std::vector<uint32_t> Render(uint16_t pixelCount) override {
         std::vector<uint32_t> v(pixelCount);
         std::fill(v.begin(), v.end(), 0xFFFF00FF);

         return v;
      }
    };
  }
}