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
      virtual std::string GetName() const override
      {
        return "solid";
      }
      virtual void Start() override {
         std::vector<uint32_t> v(1);
         std::fill(v.begin(), v.end(), 0xFFFF00FF);
      }
    };
  }
}