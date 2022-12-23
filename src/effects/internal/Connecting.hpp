#pragma once

#include "../BaseEffect.h"
#include <atomic>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

namespace LedPi
{
  namespace Effects
  {
    class Connecting : public BaseEffect
    {
    public:
      Connecting(uint16_t pixelCount) : BaseEffect(pixelCount) {};
      virtual std::string GetName() const override
      {
        return "connecting";
      }

      void DoSwitchBack() {
        SwitchBack();
      }
      std::shared_ptr<std::thread> failThread;
      void Fail() {
        SwitchTo(Effect::Strobe);
        UpdateConfig("c1", 0xFFFF0000);
        UpdateConfig("speed", 350);
        failThread = std::make_shared<std::thread>([this] {
          std::this_thread::sleep_for(std::chrono::milliseconds(350 * 6));
          this->DoSwitchBack();
        });
      }


      std::shared_ptr<std::thread> successThread;
      void Success() {
        SwitchTo(Effect::Strobe);
        UpdateConfig("c1", 0xFF00FF00);
        UpdateConfig("speed", 350);
        successThread = std::make_shared<std::thread>([this] {
          std::this_thread::sleep_for(std::chrono::milliseconds(350 * 6));
          this->DoSwitchBack();
        });
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