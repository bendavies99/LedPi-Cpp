#pragma once

#include "../BaseEffect.h"

#include <atomic>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <timercpp/timercpp.h>

namespace LedPi
{
  namespace Effects
  {
    class Connecting : public BaseEffect
    {
    public:
      Connecting(uint16_t pixelCount) : BaseEffect(pixelCount) {};
      Timer timer;
      virtual std::string GetName() const override
      {
        return "connecting";
      }

      virtual void Dispose() override {
        timer.stop();
      }

      void DoSwitchBack() {
        SwitchBack();
      }

      void Fail() {
        SwitchTo(Effect::Strobe);
        UpdateConfig("c1", 0xFFFF0000);
        UpdateConfig("speed", 350);
        timer.setTimeout([&]() {
          this->DoSwitchBack();
        }, 350 * 6);
      }


      void Success() {
        SwitchTo(Effect::Strobe);
        UpdateConfig("c1", 0xFF00FF00);
        UpdateConfig("speed", 350);
        timer.setTimeout([&]() {
          this->DoSwitchBack();
        }, 350 * 6);
      }

      int space = 2;
      int counter = 0;
      int maxCount;
      virtual void Start() override
      {
        maxCount = m_PixelCount / space;
        SetupRenderer(50, [&](std::vector<uint32_t> pr)
        {
          std::vector<uint32_t> cols(this->m_PixelCount, 0xFF000000);

          for(int i = 0; i < cols.size(); i+= space) {
            int count = i / space;
            if(counter >= count) {
              cols[i] = 0xFF0000FF;
            } else {
              cols[i] = 0xFF000000;
            }
            cols[i + 1] = 0xFF000000;
          }         

          this->SetRenderData(cols);
          counter++;
          if(counter >= maxCount) {
            counter = 0;
          }
        });
      }
    };
  }
}