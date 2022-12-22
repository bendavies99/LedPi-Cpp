#pragma once

#include <memory>
#include <vector>
#include "application-config.hpp"
#include "strip-config.hpp"
#include "network-config.hpp"


namespace LedPi {

  class Config {
    public:
      Config();

      std::shared_ptr<ApplicationConfig> GetApplictionConfig() const {
        return m_ApplicationConfig;
      }

      std::shared_ptr<NetworkConfig> GetNetworkConfig() const {
        return m_NetworkConfig;
      }

      std::vector<StripConfig>& GetStripConfigurations() {
        return m_StripConfigurations;
      }
    private:
      std::shared_ptr<ApplicationConfig> m_ApplicationConfig;
      std::shared_ptr<NetworkConfig> m_NetworkConfig;
      std::vector<StripConfig> m_StripConfigurations;
  };
}