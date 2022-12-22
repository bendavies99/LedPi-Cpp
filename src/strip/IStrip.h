#pragma once

#include <string>
#include <stdint.h>

namespace LedPi
{
  class IStrip
  {
  public:
    virtual std::string GetName() const = 0;
    virtual uint16_t GetPixelCount() const = 0;
    virtual uint32_t GetColorAtPixel(uint16_t pixel) const = 0;
    virtual void SetColorAtPixel(uint16_t index, uint32_t color) = 0;
    virtual void Render() = 0;
  };
}