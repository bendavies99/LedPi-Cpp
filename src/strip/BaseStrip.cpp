#include "BaseStrip.h"
#include <string>
#include <algorithm>

LedPi::BaseStrip::BaseStrip(const StripConfig &stripConfig)
  : m_StripConfig(stripConfig), m_Colors(stripConfig.ledCount)
{
  std::fill(m_Colors.begin(), m_Colors.end(), 0xFF000000);
  SetEffect(Effect::Solid);
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
  if (!m_IsOn) return;
  
  if (m_CurrentEffect && m_Mode == StripMode::EFFECTS && m_CurrentEffect->IsDirty())
  {
    m_Colors = m_CurrentEffect->GetColors();
  }

  RenderStrip(); 
}

uint8_t LedPi::BaseStrip::GetUID()
{
  return this->m_StripConfig.uid;
}

std::vector<uint32_t>& LedPi::BaseStrip::GetColors()
{
  return m_Colors;
}

void LedPi::BaseStrip::SetEffect(Effect effect)
{
  SetupEffect(effect);
}


void LedPi::BaseStrip::SetupEffect(Effect effect)
{
  m_CurrentEffect = EffectRegistry->GetEffect(effect);
}
