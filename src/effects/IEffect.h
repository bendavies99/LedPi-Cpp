#pragma once

#include <string>
#include <stdint.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <boost/any.hpp>
#include "Effect.hpp"

namespace LedPi
{
  class IEffect
  {
  public:
    virtual std::string GetName() const = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void UpdateConfig(std::string configName, boost::any value) = 0;
    virtual void Dispose() = 0;
    virtual std::vector<uint32_t> GetColors() = 0;
    virtual bool IsDirty() = 0;
  };
}