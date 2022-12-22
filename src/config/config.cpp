#include "config.h"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <fstream>
#include <sys/stat.h>

bool exists(const std::string &name)
{
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

namespace LedPi
{
  using json = nlohmann::json;
  using json_validator = nlohmann::json_schema::json_validator;

  static json CONFIG_SCHEMA = R"({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
      "applicationConfig": {
        "type": "object",
        "properties": {
          "applicationName": {
            "type": "string"
          },
          "debugMode": {
            "type": "boolean"
          }
        },
        "required": [
          "applicationName",
          "debugMode"
        ]
      },
      "networkConfig": {
        "type": "object",
        "properties": {
          "reactivePort": {
            "type": "integer"
          },
          "mqtt": {
            "type": "object",
            "properties": {
              "host": {
                "type": "string"
              }
            },
            "required": [
              "host"
            ]
          }
        },
        "required": [
          "reactivePort",
          "mqtt"
        ]
      },
      "stripConfigurations": {
        "type": "array",
        "items": [
          {
            "type": "object",
            "properties": {
              "name": {
                "type": "string"
              },
              "pinNumber": {
                "type": "integer"
              },
              "ledCount": {
                "type": "integer"
              },
              "uid": {
                "type": "integer"
              }
            },
            "required": [
              "name",
              "pinNumber",
              "ledCount",
              "uid"
            ]
          }
        ]
      }
    },
    "required": [
      "applicationConfig",
      "networkConfig",
      "stripConfigurations"
    ]
})"_json;

  Config::Config()
  {
    if (exists("config.json"))
    {
      json_validator validator;

      try
      {
        validator.set_root_schema(CONFIG_SCHEMA);
      }
      catch (const std::exception &e)
      {
        spdlog::error("Validation of schema failed, here is why: {0}\nExiting....", e.what());
        exit(-2);
      }

      std::ifstream f("config.json");
      json data = json::parse(f);
      try
      {
        validator.validate(data);
      }
      catch (const std::exception &e)
      {
        spdlog::error("Validation of schema failed, here is why: {0}\nExiting....", e.what());
        exit(-2);
      }
      spdlog::debug("JSON {0}", data.dump(2));

      this->m_ApplicationConfig = std::make_shared<ApplicationConfig>();
      this->m_ApplicationConfig->applicationName = data["applicationConfig"]["applicationName"];
      this->m_ApplicationConfig->debugMode = data["applicationConfig"]["debugMode"];

      this->m_NetworkConfig = std::make_shared<NetworkConfig>();
      this->m_NetworkConfig->reactivePort = data["networkConfig"]["reactivePort"];

      json stripConfigs = data["stripConfigurations"];
      for (auto &el : stripConfigs)
      {
        StripConfig sc;
        sc.name = el["name"];
        sc.ledCount = el["ledCount"];
        sc.pinNumber = el["pinNumber"];
        sc.uid = el["uid"];
        this->GetStripConfigurations().push_back(sc);
      }
    }
    else
    {
      spdlog::error("Cannot find a config.json file");
    }
  }
}