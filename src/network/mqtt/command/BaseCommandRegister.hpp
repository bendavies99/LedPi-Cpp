#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <boost/algorithm/string.hpp>

namespace LedPi
{
  class BaseCommandRegister
  {
  public:
    BaseCommandRegister(std::string prefix): m_Prefix(prefix) {};

    void RunCommand(std::string name, std::string message) {
      if(!boost::iequals(name.substr(0, name.find("/")), m_Prefix)) return;
      std::string newName = name.substr(name.find("/") + 1);
      if (m_Commands.count(newName))
      {
        m_Commands[newName](message);
      }
      
    }
  public:
    virtual void RegisterCommands() = 0;   
    virtual void RegisterPublishers(std::function<void(std::string /* key */, std::string /* value */)> send) = 0;

    template<typename _F>
    void RegisterCommand(std::string name, const _F& func) {
      m_Commands[name] = func;
    }
  protected:
    std::string m_Prefix;
    std::unordered_map<std::string, std::function<void(std::string)>> m_Commands;
  };
}