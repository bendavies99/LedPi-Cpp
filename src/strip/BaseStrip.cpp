#include "BaseStrip.h"
#include <string>
#include <algorithm>
#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>

LedPi::BaseStrip::BaseStrip(const StripConfig &stripConfig)
    : m_StripConfig(stripConfig), m_Colors(stripConfig.ledCount)
{
  std::fill(m_Colors.begin(), m_Colors.end(), 0xFF000000);
  SetEffect(Effect::Connecting, {});

}

std::string LedPi::BaseStrip::GetName() const
{
  return m_StripConfig.name;
}

uint16_t LedPi::BaseStrip::GetPixelCount() const
{
  return m_StripConfig.ledCount;
}

uint32_t LedPi::BaseStrip::GetColorAtPixel(uint16_t pixel) const
{
  return m_Colors[pixel];
}

void LedPi::BaseStrip::SetColorAtPixel(uint16_t index, uint32_t color)
{
  m_Colors[index] = color;
}

void LedPi::BaseStrip::Render()
{
  if (!m_IsOn)
    return;

  if (m_CurrentEffect && m_Mode == StripMode::EFFECTS && m_CurrentEffect->IsDirty())
  {
    m_Colors = m_CurrentEffect->GetColors();
  }

  RenderStrip();
}

void LedPi::BaseStrip::SetBrightness(uint8_t brightness)
{
  int currBrightness = this->m_Brigtness;
  m_BrightnessChange.store(currBrightness);
  bool isDrop = brightness < currBrightness;
  if (brightness != this->m_Brigtness)
  {
    PushChange(Strips::StripOpertaion::BRIGHTNESS);
    m_BrightnessThread = std::make_shared<std::thread>([=] {
      if (brightness != 0)
      {
        TurnOnEffect();
      }
      while (m_BrightnessChange.load() != brightness)
      {
        if (isDrop && brightness < m_BrightnessChange.load())
        {
          m_BrightnessChange.store(m_BrightnessChange.load() - 20);
          if (m_BrightnessChange.load() < 0)
          {
            m_BrightnessChange.store(0);
          }
          if (m_BrightnessChange.load() < brightness)
          {
            m_BrightnessChange.store(brightness);
          }
        }
        else if (!isDrop && brightness > m_BrightnessChange.load())
        {
          m_BrightnessChange.store(m_BrightnessChange.load() + 20);
          if (m_BrightnessChange.load() > 255)
          {
            m_BrightnessChange.store(255);
          }
          if (m_BrightnessChange.load() > brightness)
          {
            m_BrightnessChange.store(brightness);
          }
        }
        this->m_Brigtness = m_BrightnessChange.load();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
      if (brightness == 0)
      {
        TurnOffEffect();
      }
    });
  }
}

void LedPi::BaseStrip::SetEffectColor(uint32_t color)
{
  this->m_EffectColor = color;
  if (this->m_CurrentEffect)
  {
    this->m_CurrentEffect->UpdateConfig("c1", color);
  }

  PushChange(Strips::StripOpertaion::EFFECT_COLOR);
}

uint8_t LedPi::BaseStrip::GetBrightness()
{
  return this->m_Brigtness;
}

uint8_t LedPi::BaseStrip::GetUID()
{
  return this->m_StripConfig.uid;
}

std::vector<uint32_t> &LedPi::BaseStrip::GetColors()
{
  return m_Colors;
}

void LedPi::BaseStrip::PushChange(LedPi::Strips::StripOpertaion op)
{
  for (const auto &el : m_OperationListeners)
  {
    el(op);
  }
}

void LedPi::BaseStrip::TurnOnEffect()
{
  if (GetStripMode() == StripMode::EFFECTS && m_CurrentEffect)
  {
    m_CurrentEffect->Start();
  }
}

void LedPi::BaseStrip::TurnOffEffect()
{
  if (m_CurrentEffect)
  {
    m_CurrentEffect->Stop();
  }
}

void LedPi::BaseStrip::SetEffect(Effect effect, std::unordered_map<std::string, boost::any> config)
{
  if (m_CurrentEffect)
  {
    m_CurrentEffect->Stop();
  }

  SetupEffect(effect);
  if (!config.count("c1"))
  {
    m_CurrentEffect->UpdateConfig("c1", m_EffectColor);
  }

  for (const auto &kv : config)
  {
    m_CurrentEffect->UpdateConfig(kv.first, kv.second);
  }
  TurnOnEffect();
  PushChange(Strips::StripOpertaion::EFFECT);
}

std::shared_ptr<LedPi::IEffect> LedPi::BaseStrip::GetEffect()
{
  return m_CurrentEffect;
}

void LedPi::BaseStrip::On()
{
  if (!this->m_IsOn)
  {
    m_IsOn = true;
    PushChange(Strips::StripOpertaion::STATE);
    SetBrightness(m_SavedBrightness);
  }
}

void LedPi::BaseStrip::Off()
{
  if (this->m_IsOn)
  {
    m_IsOn = false;
    PushChange(Strips::StripOpertaion::STATE);
    m_SavedBrightness = m_Brigtness;
    SetBrightness(0);
  }
}

LedPi::StripMode LedPi::BaseStrip::GetStripMode()
{
  return m_Mode;
}

void LedPi::BaseStrip::SetStripMode(StripMode mode)
{
  if (mode != this->m_Mode)
  {
    this->m_Mode = mode;
    if (mode == StripMode::NETWORK_UDP)
    {
      this->TurnOffEffect();
      std::fill(m_Colors.begin(), m_Colors.end(), 0xFF000000);
    }
    else
    {
      this->TurnOnEffect();
    }

    PushChange(Strips::StripOpertaion::REACTIVE);
  }
}

template<typename _F>
void LedPi::BaseStrip::listenForChanges(const _F& func)
{
  this->m_OperationListeners.push_back(func);
}

uint32_t LedPi::BaseStrip::GetEffectColor()
{
  return this->m_EffectColor;
}

bool LedPi::BaseStrip::GetState()
{
  return this->m_IsOn;
}

void LedPi::BaseStrip::SetupEffect(Effect effect)
{
  m_CurrentEffect = EffectRegistry->GetEffect(effect, GetPixelCount());
  m_CurrentEffect->Start();
}
