#include "BaseEffect.h"
#include <spdlog/spdlog.h>
#include <chrono>
#include <thread>
#include "EffectRegistry.hpp"

void LedPi::BaseEffect::Stop()
{
  //Get rid of any lingering threads
  Dispose();
  if (m_RenderThread != nullptr)
  {
    m_RenderRunning = false;
    m_RenderThread->join();
    m_RenderThread.reset();
  }

  if (m_SwitchedEffect != nullptr)
  {
    m_SwitchedEffect->Stop();
  }
}

void LedPi::BaseEffect::UpdateConfig(std::string configName, boost::any value)
{
  m_Config[configName] = value;
  if (m_SwitchedEffect)
  {
    m_SwitchedEffect->UpdateConfig(configName, value);
  } else {
    Start();
  }
}

void LedPi::BaseEffect::Dispose()
{
}

std::vector<uint32_t> LedPi::BaseEffect::GetColors()
{
  if (m_SwitchedEffect)
  {
    return m_SwitchedEffect->GetColors();
  }

  this->m_Dirty = false; 
  return m_CurrentColours;
}

bool LedPi::BaseEffect::IsDirty()
{
  if (m_SwitchedEffect)
  {
    return m_SwitchedEffect->IsDirty();
  }
  
  return m_Dirty;
}

std::unordered_map<std::string, boost::any> LedPi::BaseEffect::GetConfig()
{
  return m_Config;
}

void LedPi::BaseEffect::SetRenderData(std::vector<uint32_t> cols)
{
  if (cols.size() != m_PixelCount)
  {
    throw "Cols should be the size of the strip";
  }
  this->m_Dirty = true;
  this->m_PreviousRender = cols;
  this->m_CurrentColours = cols;
}


template<typename _Func>
void LedPi::BaseEffect::SetupRenderer(uint32_t delay, const _Func& func)
{
  if (m_SwitchedEffect != nullptr)
  {
      spdlog::warn("You are currently rendering an effect please use switchBack before reapplying the renderer");
      return;
  }
  
  if (m_RenderThread != nullptr)
  {
    m_RenderRunning = false;
    m_RenderThread->join();
    m_RenderThread.reset();
  }
  m_RenderFunc = func;
  m_RenderRunning = true;
  m_RenderDelay = delay;
  m_RenderThread = std::make_unique<std::thread>([&]() {
    while(m_RenderRunning) {
      m_RenderFunc(m_PreviousRender);
      std::this_thread::sleep_for(std::chrono::milliseconds(m_RenderDelay));
    }
  });
}

template <typename _T>
_T LedPi::BaseEffect::GetConfigItem(std::string configName)
{
  if (m_Config.count(configName))
  {
    return boost::any_cast<_T>(m_Config[configName]);
  }
  
  return NULL;
}

bool LedPi::BaseEffect::HasConfigItem(std::string configName)
{
  return m_Config.count(configName) > 0;
}

void LedPi::BaseEffect::SwitchTo(Effect effect)
{
  Stop();
  m_SwitchedEffect = EffectRegistry->GetEffect(effect, m_PixelCount);
  m_SwitchedEffect->Start();
}

void LedPi::BaseEffect::SwitchBack()
{
  Stop();
  m_SwitchedEffect.reset();
  Start();
}
