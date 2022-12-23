#pragma once

namespace LedPi {
  namespace Strips {
    enum StripOpertaion {
      /**
       * When an Effect is changed
       */
      EFFECT,
      /**
       * When Reactive mode is turned on or off
       */
      REACTIVE,
      /**
       * When the Effect Color is changed
       */
      EFFECT_COLOR,
      /**
       * When the brightness is changed
       */
      BRIGHTNESS,
      /**
       * When the strip is turned off / on
       */
      STATE
    };
  }
}