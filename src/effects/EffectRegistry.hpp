#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "IEffect.h"
#include "basic/all.h"
#include "internal/Connecting.hpp"
#include <boost/algorithm/string.hpp>
#include "Effect.hpp"
#include <iostream>

namespace LedPi {
  class _EffectRegistry {
    public:
      template<typename Base, typename T>
      inline bool EffectInstanceOf(const T *ptr) {
        return std::dynamic_cast<const Base*>(ptr) != nullptr;
      }

      std::string GetEffectName(std::shared_ptr<IEffect> effect) {
        return effect->GetName();
      }

      std::vector<std::string> GetEffectNames() {
        std::vector<std::string> values;
        for(auto& el : Effect::_values()) {
          if (el._to_string() == "Connecting") continue;
          values.push_back(el._to_string());
        }

        return values;
      }

      Effect GetEffectFromName(std::string effectName) {
        if (Effect::_is_valid_nocase(effectName.c_str()))
        {
          return Effect::_from_string_nocase(effectName.c_str());
        }

        return Effect::Solid;
      }

      std::shared_ptr<IEffect> GetEffect(Effect effect, uint16_t pixelCount) {
        switch (effect)
        {
        case Effect::Strobe:
         return std::make_shared<Effects::Strobe>(pixelCount);  
        case Effect::Connecting:
         return std::make_shared<Effects::Connecting>(pixelCount);  
        case Effect::Fireflicker:
         return std::make_shared<Effects::Fireflicker>(pixelCount);  
        case Effect::Scan:
         return std::make_shared<Effects::Scan>(pixelCount);
        case Effect::Solid:
        default:
          return std::make_shared<Effects::Solid>(pixelCount);
          break;
        } 
      }
  };

  static _EffectRegistry* EffectRegistry = new _EffectRegistry();
}