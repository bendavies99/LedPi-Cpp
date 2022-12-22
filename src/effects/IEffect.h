#pragma once

#include <string>
#include <stdint.h>
#include <vector>
#include <map>

namespace LedPi
{
  class IEffect
  {
  public:
    virtual std::string GetName() const = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void UpdateConfig(std::string configName, uint32_t value) = 0;
    virtual void Dispose() = 0;
    virtual std::vector<uint32_t> GetColors() = 0;
    virtual bool IsDirty() = 0;
  };
}