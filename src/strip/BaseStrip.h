#pragma once

#include "IStrip.h"
#include "../effects/EffectRegistry.hpp"
#include <vector>

#include "../config/strip-config.hpp"

#define STARTING_BRIGHTNESS 255

namespace LedPi
{
  enum StripMode {
    EFFECTS,
    NETWORK_UDP
  };

  class BaseStrip : public IStrip
  {
  public:
    BaseStrip(const StripConfig& stripConfig);
    virtual std::string GetName() const override;
    virtual uint16_t GetPixelCount() const override;
    virtual uint32_t GetColorAtPixel(uint16_t pixel) const override;
    virtual void SetColorAtPixel(uint16_t index, uint32_t color) override;
    virtual void RenderStrip() = 0;
    virtual void Render() override;
  protected:
    const StripConfig& m_StripConfig;
    uint8_t GetUID();
    std::vector<uint32_t>& GetColors();
    void SetEffect(Effect effect);
  private:
    void SetupEffect(Effect effect);  
  private:
    std::vector<uint32_t> m_Colors;
    int m_Brigtness = STARTING_BRIGHTNESS;
    int m_SavedBrightness = STARTING_BRIGHTNESS;
    StripMode m_Mode = StripMode::EFFECTS;
    uint32_t m_EffectColor = 0xFF0000AA;
    std::shared_ptr<IEffect> m_CurrentEffect;
    bool m_IsOn = true;
  };
}