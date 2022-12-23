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
  std::atomic<uint8_t> bChange(currBrightness);
  bool isDrop = brightness < currBrightness;
  if (brightness != this->m_Brigtness)
  {
    PushChange(Strips::StripOpertaion::BRIGHTNESS);
    std::thread([&] {
      if (brightness != 0)
      {
        TurnOnEffect();
      }
      while (bChange.load() != brightness)
      {
        if (isDrop && brightness < bChange.load())
        {
          bChange.store(bChange.load() - 20);
          if (bChange.load() < 0)
          {
            bChange.store(0);
          }
          if (bChange.load() < brightness)
          {
            bChange.store(brightness);
          }
        }
        else if (!isDrop && brightness > bChange.load())
        {
          bChange.store(bChange.load() + 20);
          if (bChange.load() > 255)
          {
            bChange.store(255);
          }
          if (bChange.load() > brightness)
          {
            bChange.store(brightness);
          }
        }
        this->m_Brigtness = bChange.load();
        Render();
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
    m_CurrentEffect->Dispose();
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

void LedPi::BaseStrip::listenForChanges(void (*func)(LedPi::Strips::StripOpertaion))
{
  this->m_OperationListeners.push_back(func);
}

uint32_t LedPi::BaseStrip::GetEffectColor()
{
  return this->m_EffectColor;
}

void LedPi::BaseStrip::SetupEffect(Effect effect)
{
  m_CurrentEffect = EffectRegistry->GetEffect(effect, GetPixelCount());
  m_CurrentEffect->Start();
}
