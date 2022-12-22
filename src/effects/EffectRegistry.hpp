#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "IEffect.h"
#include "basic/all.h"

namespace LedPi {
  enum Effect {
    Rainbow,
    Reactive,
    Solid
  };

  class _EffectRegistry {
    public:
      std::string GetEffectName(std::shared_ptr<IEffect> effect) {
        return effect->GetName();
      }
      std::vector<std::string> GetEffectNames() {
        return { "rainbow", "reactive", "solid" };
      }
      std::shared_ptr<IEffect> GetEffect(Effect effect) {
        switch (effect)
        {
        case Solid:
        default:
          return std::make_shared<Effects::Solid>();
          break;
        } 
      }
  };

  static _EffectRegistry* EffectRegistry = new _EffectRegistry();
}