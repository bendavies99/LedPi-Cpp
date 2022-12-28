#pragma once

#include "../config/strip-config.hpp"
#ifdef PRODUCTION_MODE
#include "NativeStrip.hpp"
#elif REMOTE_DEBUG_DISPLAY
#include "RemoteStrip.hpp"
#endif
#include "../network/RemoteStripServer.h"
#include <memory>

namespace LedPi {
   class StripFactory {
     public:
     static std::shared_ptr<IStrip> makeStrip(const StripConfig& stripConfig, std::shared_ptr<RemoteStripServer> stripServer) {
        #ifdef PRODUCTION_MODE
           //Native Strip
           return std::make_shared<NativeStrip>(stripConfig);

        #elif REMOTE_DEBUG_DISPLAY
           //Remote Debugging
           return std::make_shared<RemoteStrip>(stripConfig, stripServer);
        #else
           //Debug Strip

        #endif
     }
   };
}