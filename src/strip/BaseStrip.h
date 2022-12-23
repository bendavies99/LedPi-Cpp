#pragma once

#include "IStrip.h"
#include "../effects/EffectRegistry.hpp"
#include <vector>

#include "../config/strip-config.hpp"
#include <unordered_map>
#include <boost/any.hpp>
#include "StripOpertation.h"

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
    virtual void SetBrightness(uint8_t brightness) override;
    virtual void SetEffectColor(uint32_t color) override;
    virtual uint8_t GetBrightness() override;
    virtual void SetEffect(Effect effect, std::unordered_map<std::string, boost::any> config) override;
    virtual std::shared_ptr<IEffect> GetEffect() override;
    virtual void On() override;
    virtual void Off() override;
    virtual StripMode GetStripMode() override;
    virtual void SetStripMode(StripMode mode) override;
    virtual void listenForChanges(void (*func)(Strips::StripOpertaion)) override;
    virtual uint32_t GetEffectColor() override;
    
  protected:
    const StripConfig& m_StripConfig;
    uint8_t GetUID();
    std::vector<uint32_t>& GetColors();
    void PushChange(LedPi::Strips::StripOpertaion op);
    void TurnOnEffect();
    void TurnOffEffect();
  private:
    void SetupEffect(Effect effect);  
  private:
    std::vector<uint32_t> m_Colors;
    uint8_t m_Brigtness = STARTING_BRIGHTNESS;
    uint8_t m_SavedBrightness = STARTING_BRIGHTNESS;
    StripMode m_Mode = StripMode::EFFECTS;
    uint32_t m_EffectColor = 0xFF0000AA;
    std::shared_ptr<IEffect> m_CurrentEffect;
    std::vector<void (*)(Strips::StripOpertaion)> m_OperationListeners;
    bool m_IsOn = true;
  };
}