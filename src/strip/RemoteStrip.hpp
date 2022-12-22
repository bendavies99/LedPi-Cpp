#pragma once

#include "BaseStrip.h"
#include <spdlog/spdlog.h>
#include "../network/RemoteStripServer.h"

namespace LedPi
{
  class RemoteStrip : public BaseStrip
  {
  private:
    std::shared_ptr<RemoteStripServer> m_StripServer;
  public:
    RemoteStrip(const StripConfig &stripConfig, std::shared_ptr<RemoteStripServer> stripServer) 
    : BaseStrip(stripConfig), m_StripServer(stripServer)
    {

    }

    void Render() override
    {
      m_StripServer->SendRenderData(GetUID(), GetColors());
    };
  };
}