#pragma once

#include "IEffect.h"
#include <memory>
#include <algorithm>
#include <thread>
#include <type_traits>
#include <functional>


namespace LedPi
{
  class BaseEffect : public IEffect
  {
  public:
    BaseEffect(uint16_t pixelCount): m_PixelCount(pixelCount), 
    m_PreviousRender(pixelCount), m_CurrentColours(pixelCount) {
      std::fill(m_PreviousRender.begin(), m_PreviousRender.end(), 0xFF000000);
      std::fill(m_CurrentColours.begin(), m_CurrentColours.end(), 0xFF000000);
    };
    ~BaseEffect() {
      Stop();
    }
    virtual std::string GetName() const = 0;
    virtual void Start() = 0;
    virtual void Stop() override;
    virtual void UpdateConfig(std::string configName, boost::any value) override;
    virtual void Dispose() override;
    virtual std::vector<uint32_t> GetColors() override;
    virtual bool IsDirty() override;
    virtual std::unordered_map<std::string, boost::any> GetConfig() override;
  protected:
    uint16_t m_PixelCount;
    void SetRenderData(std::vector<uint32_t> cols);
    template<typename _Func>
    void SetupRenderer(uint32_t delay, const _Func& func);
    template <typename _T>
    _T GetConfigItem(std::string configName);
    bool HasConfigItem(std::string configName);
    void SwitchTo(Effect effect);
    void SwitchBack();
  private:
    std::vector<uint32_t> m_PreviousRender;
    std::shared_ptr<IEffect> m_SwitchedEffect;
    std::vector<uint32_t> m_CurrentColours;
    std::unique_ptr<std::thread> m_RenderThread;
    bool m_RenderRunning;
    uint32_t m_RenderDelay = 100;
    bool m_Dirty;
    std::unordered_map<std::string, boost::any> m_Config;
    std::function<void(std::vector<uint32_t>)> m_RenderFunc;
  };
}