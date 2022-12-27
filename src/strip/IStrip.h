#pragma once

#include <string>
#include <memory>
#include <stdint.h>
#include <unordered_map>
#include <boost/any.hpp>
#include "../effects/IEffect.h"
#include "StripOpertation.h"
#include <functional>

namespace LedPi
{
  enum StripMode {
    EFFECTS,
    NETWORK_UDP
  };

  class IStrip
  {
  public:
    virtual std::string GetName() const = 0;
    virtual uint16_t GetPixelCount() const = 0;
    virtual uint32_t GetColorAtPixel(uint16_t pixel) const = 0;
    virtual void SetColorAtPixel(uint16_t index, uint32_t color) = 0;
    virtual void Render() = 0;
    virtual void SetBrightness(uint8_t brightness) = 0;
    virtual void SetEffectColor(uint32_t color) = 0;
    virtual uint8_t GetBrightness() = 0;
    virtual void SetEffect(Effect effect, std::unordered_map<std::string, boost::any> config) = 0;
    virtual std::shared_ptr<IEffect> GetEffect() = 0;
    virtual void On() = 0;
    virtual void Off() = 0;
    virtual StripMode GetStripMode() = 0;
    virtual void SetStripMode(StripMode mode) = 0;
    virtual void listenForChanges(std::function<void(Strips::StripOpertaion)> func) = 0;
    virtual uint32_t GetEffectColor() = 0;
    virtual bool GetState() = 0;
    virtual uint8_t GetUID() = 0;
    virtual void SetStripColors(std::vector<uint32_t> cols) = 0;
  };
}