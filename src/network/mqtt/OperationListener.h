#pragma once

#include "mqtt/async_client.h"
#include <spdlog/spdlog.h>
#include <cstdint>
#include <vector>
#include "ConnectListener_fwd.h"
#include "command/BaseCommandRegister.hpp"

namespace LedPi
{
  class OperationListener : public mqtt::callback
  {
  public:
    OperationListener(ConnectListener& connectListener);    
    virtual void connection_lost(const std::string &cause) override;
    virtual void message_arrived(mqtt::const_message_ptr msg) override;
    virtual void connected(const std::string& cause) override;

  private:
    ConnectListener& m_ConnectListener;
    std::vector<std::shared_ptr<BaseCommandRegister>> m_CommandRegisters;
  };
}