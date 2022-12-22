#pragma once

#include "../config/strip-config.hpp"
#include "NativeStrip.hpp"
#include "RemoteStrip.hpp"
#include "../network/RemoteStripServer.h"
#include <memory>

namespace LedPi {
   class StripFactory {
     public:
     static std::unique_ptr<IStrip> makeStrip(const StripConfig& stripConfig, std::shared_ptr<RemoteStripServer> stripServer) {
        #ifdef PRODUCTION_MODE
           //Native Strip

        #elif REMOTE_DEBUG_DISPLAY
           //Remote Debugging
           return std::make_unique<RemoteStrip>(stripConfig, stripServer);
        #else
           //Debug Strip

        #endif
     }
   };
}