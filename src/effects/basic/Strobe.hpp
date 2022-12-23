#pragma once

#include "../BaseEffect.h"
#include <atomic>
#include <algorithm>
#include <cstdlib>
#include <ctime>

namespace LedPi
{
  namespace Effects
  {
    class Strobe : public BaseEffect
    {
    public:
      Strobe(uint16_t pixelCount) : BaseEffect(pixelCount) {};
      virtual std::string GetName() const override
      {
        return "strobe";
      }
      int speed = 50;
      bool showing = false;
      virtual void Start() override
      {
        if (HasConfigItem("speed"))
        {
          speed = GetConfigItem<int>("speed");
        }
        
        SetupRenderer(speed, [&](std::vector<uint32_t> pr)
        {
          if (showing)
          {
            auto col = GetConfigItem<uint32_t>("c1");
            std::vector<uint32_t> cols(this->m_PixelCount, col);
            this->SetRenderData(cols);
            showing = false;
          } else {
            std::vector<uint32_t> cols(this->m_PixelCount, 0xFF000000);
            this->SetRenderData(cols);
            showing = true;
          }
        });
      }
    };
  }
}