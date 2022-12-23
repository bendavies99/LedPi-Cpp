#pragma once

#include "../BaseEffect.h"
#include <atomic>
#include <algorithm>

namespace LedPi
{
  namespace Effects
  {
    class Scan : public BaseEffect
    {
    public:
      std::atomic<uint16_t> counter;
      std::atomic<bool> reverse;
      Scan(uint16_t pixelCount) : BaseEffect(pixelCount) {};
      virtual std::string GetName() const override
      {
        return "scan";
      }
      virtual void Start() override
      {
        std::atomic_init(&counter, 0);
        std::atomic_init(&reverse, false);
        SetupRenderer(10, [&](std::vector<uint32_t> pr)
        {
         int dir = this->reverse.load() ? -1 : 1;
         std::vector<uint32_t> cols(this->m_PixelCount, 0xFF000000);

        for (int i = 0; i < 2; i++) {
          int index = counter.load() - i;
          if (index < 0 || index > this->m_PixelCount - 1) continue;
          cols[index] = GetConfigItem<uint32_t>("c1");
        }

        this->SetRenderData(cols);

        counter.store(counter.load() + dir);

        if (counter.load() >= this->m_PixelCount) {
          counter.store(this->m_PixelCount - 1);
          reverse.store(true);
        } else if (counter.load() <= 0) {
          counter.store(0);
          reverse.store(false);
        } 
        
        });
      }
    };
  }
}