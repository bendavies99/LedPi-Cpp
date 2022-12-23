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
    class Fireflicker : public BaseEffect
    {
    public:
      Fireflicker(uint16_t pixelCount) : BaseEffect(pixelCount) {};
      virtual std::string GetName() const override
      {
        return "fireflicker";
      }
      virtual void Start() override
      {
        srand(time((time_t)0));
        SetupRenderer(100, [&](std::vector<uint32_t> pr)
        {
          auto col = GetConfigItem<uint32_t>("c1");
          auto r = (col >> 16) & 0xFF;
          auto g = (col >> 8) & 0xFF;
          auto b = (col) & 0xFF;
          int lum = std::max(r, std::max(g, b));
          std::vector<uint32_t> cols(this->m_PixelCount, 0xFF000000);
          
          for(size_t i = 0; i < this->m_PixelCount; i++) {
            int flicker = rand() % (std::abs(lum) + 1);
            uint8_t newR = std::max((int)r - flicker, 0);
            uint8_t newG = std::max((int)g - flicker, 0);
            uint8_t newB = std::max((int)b - flicker, 0);

            uint32_t rgb = newR;
            rgb = (rgb << 8) + newG;
            rgb = (rgb << 8) + newB;

            cols[i] = rgb;
          }

          this->SetRenderData(cols);
        
        });
      }
    };
  }
}