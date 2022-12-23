#pragma once

#include <better_enums/enum.h>

namespace LedPi {
 BETTER_ENUM(Effect, uint16_t, 
     Fireflicker,
     Scan,
     Solid,
     Connecting,
     Strobe
  );
}
