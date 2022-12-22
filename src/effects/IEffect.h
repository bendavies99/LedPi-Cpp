#pragma once

#include <string>
#include <stdint.h>
#include <vector>

namespace LedPi
{
  class IEffect
  {
  public:
    virtual std::string GetName() const = 0;
    virtual std::vector<uint32_t> Render(uint16_t pixelCount) = 0;
  };
}