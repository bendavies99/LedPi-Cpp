#include "config.h"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <experimental/filesystem>

namespace LedPi
{
  using json = nlohmann::json;

  Config::Config() {
    if (std::experimental::filesystem::exists("config.json"))
    {
      std::ifstream f("config.json");
      json data = json::parse(f);
      spdlog::debug("JSON {0}", data.dump(2));
      spdlog::info("Application Name: {0}", data["applicationConfig"]["applicationName"]);
    } else {
      spdlog::error("Cannot find a config.json file");
    }
  }
} 