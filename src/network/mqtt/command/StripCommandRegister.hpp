#pragma once

#include "BaseCommandRegister.hpp"
#include <memory>
#include "../../../strip/IStrip.h"
#include "../../../effects/EffectRegistry.hpp"
#include <algorithm>
#include <string>
#include <fmt/core.h>
#include <sstream>

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

uint32_t GetIntFromHex(std::string hex) {
    uint32_t x;   
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> x;

    return x;
}

namespace LedPi {
  class StripCommandRegister : public BaseCommandRegister {
    public:
      StripCommandRegister(std::shared_ptr<IStrip> strip): BaseCommandRegister(ReplaceAll(strip->GetName(), " ", "_")), m_Strip(strip) {};

      virtual void RegisterCommands() override {
        RegisterCommand("col", [&](std::string msg) { m_Strip->SetEffectColor(GetIntFromHex(msg)); });
        RegisterCommand("bri", [&](std::string msg) { m_Strip->SetBrightness(std::stoi(msg) & 255); });
        RegisterCommand("sfx", [&](std::string sfx) {
           if (boost::iequals(sfx, "reactive"))
           {
             m_Strip->SetStripMode(LedPi::StripMode::NETWORK_UDP);
           } else {
             m_Strip->SetStripMode(LedPi::StripMode::EFFECTS);
           }

           m_Strip->SetEffect(EffectRegistry->GetEffectFromName(sfx), {}); 
        });

        RegisterCommand("state", [&](std::string v) {
           if(boost::iequals(v, "on")) {
             m_Strip->On();
           } else {
             m_Strip->Off();
           }
        });
      };

      virtual void RegisterPublishers(std::function<void(std::string /* key */, std::string /* value */)> send) override {
        m_Strip->listenForChanges([=](LedPi::Strips::StripOpertaion op) {
          switch (op)
          {
          case Strips::StripOpertaion::STATE:
            send("s", m_Strip->GetState() ? "ON" : "OFF");
            break;
          case Strips::StripOpertaion::BRIGHTNESS:
            send("b", std::to_string(m_Strip->GetBrightness()));
            break;
          case Strips::StripOpertaion::EFFECT_COLOR:
            send("c", fmt::format("#{:06x}", m_Strip->GetEffectColor())); //TODO: Hex Color
            break;
          case Strips::StripOpertaion::EFFECT:
            send("fx", m_Strip->GetEffect()->GetName());
            break;
          default:
            break;
          }
        });
      };
    private:
      std::shared_ptr<IStrip> m_Strip;  
  };
}