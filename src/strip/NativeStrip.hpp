#pragma once

#include "BaseStrip.h"
#include <stdint.h>
#include <spdlog/spdlog.h>
#include <rpi_ws281x/ws2811.h>

namespace LedPi {
  static uint8_t DMA_CHANNEL = 9;  
  static std::shared_ptr<ws2811_t> pwmStrip;
  static std::shared_ptr<ws2811_t> spiStrip;
  namespace Native {
    void InitNativeLeds() {
      try {
        DMA_CHANNEL++;
        ws2811_t ledString;
        ledString.freq = 800000;
        ledString.dmanum = DMA_CHANNEL;
        pwmStrip = std::make_shared<ws2811_t>(ledString);
      } catch(std::exception& e) {
        spdlog::error("Something went terribly wrong {0}", e.what());
      }

      try {
        DMA_CHANNEL++;
        ws2811_t ledString;
        ledString.freq = 800000;
        ledString.dmanum = DMA_CHANNEL;
        spiStrip = std::make_shared<ws2811_t>(ledString);
      } catch(std::exception& e) {
        spdlog::error("Something went terribly wrong {0}", e.what());
      }
    }

    void RunInitFn() {
      ws2811_return_t ret;
      if ((ret = ws2811_init(pwmStrip.get())) != WS2811_SUCCESS)
      {
          stdlog::error("Unable to init strip {0}", ws2811_get_return_t_str(ret));
          return;
      }

      if ((ret = ws2811_init(spiStrip.get())) != WS2811_SUCCESS)
      {
          stdlog::error("Unable to init strip {0}", ws2811_get_return_t_str(ret));
          return;
      }
    }
  }

  class NativeStrip: public BaseStrip {
    public:
      NativeStrip(const StripConfig& stripConfig): BaseStrip(stripConfig) {
        if (stripConfig.pinNumber == 13 || stripConfig.pinNumber == 12)
        {
          //PWM - 13(1) || 12(0)
          channel = stripConfig.pinNumber == 13 ? 1 : 0;
          isPwm = true;

          ws2811_channel_t c;
          c.gpionum = stripConfig.pinNumber;
          c.invert = 0;
          c.count = GetPixelCount();
          c.strip_type = WS2811_STRIP_GRB;
          c.brightness = GetBrightness();

          pwmStrip->channel[channel] = c;

        } else {
          //SPI - 10
          channel = 0;
          isPwm = false;

          ws2811_channel_t c;
          c.gpionum = stripConfig.pinNumber;
          c.invert = 0;
          c.count = GetPixelCount();
          c.strip_type = WS2811_STRIP_GRB;
          c.brightness = GetBrightness();

          spiStrip->channel[channel] = c;
        }
      }

      void RenderStrip() override {
        if (isPwm)
        {
          pwmStrip->channel[channel].brightness = GetBrightness();
          for(int i = 0; i < GetPixelCount(); i++) {
            pwmStrip->channel[channel].leds[i] = GetColorAtPixel(i);
          }

          ws2811_return_t ret;
          if ((ret = ws2811_render(pwmStrip.get())) != WS2811_SUCCESS)
          {
              stdlog::error("Unable to render strip {0}", ws2811_get_return_t_str(ret));
              return;
          }

        } else {
          spiStrip->channel[channel].brightness = GetBrightness();
          for(int i = 0; i < GetPixelCount(); i++) {
            spiStrip->channel[channel].leds[i] = GetColorAtPixel(i);
          }


          ws2811_return_t ret;
          if ((ret = ws2811_render(spiStrip.get())) != WS2811_SUCCESS)
          {
              stdlog::error("Unable to render strip {0}", ws2811_get_return_t_str(ret));
              return;
          }
        }        
      };
    private:
      int channel;
      bool isPwm;
  };
}