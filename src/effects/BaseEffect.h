#pragma once

#include "IEffect.h"

namespace LedPi
{
  class BaseEffect : public IEffect
  {
  public:
    virtual std::string GetName() const = 0;
    virtual void Start() = 0;
    virtual void Stop() override;
    virtual void UpdateConfig(std::string configName, uint32_t value) override;
    virtual void Dispose() override;
    virtual std::vector<uint32_t> GetColors() override;
    virtual bool IsDirty() override;
  };
}