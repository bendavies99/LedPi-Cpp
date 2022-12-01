#pragma once

#include <memory>
#include <vector>


namespace LedPi {
  class ApplicationConfig;
  class StripConfig;
  class NetworkConfig;

  class Config {
    public:
      Config();

      std::shared_ptr<ApplicationConfig> GetApplictionConfig() const {
        return m_ApplicationConfig;
      }

      std::shared_ptr<NetworkConfig> GetNetworkConfig() const {
        return m_NetworkConfig;
      }

      std::vector<std::shared_ptr<StripConfig>> GetStripConfigurations() const {
        return m_StripConfigurations;
      }
    private:
      std::shared_ptr<ApplicationConfig> m_ApplicationConfig;
      std::shared_ptr<NetworkConfig> m_NetworkConfig;
      std::vector<std::shared_ptr<StripConfig>> m_StripConfigurations;
  };
}