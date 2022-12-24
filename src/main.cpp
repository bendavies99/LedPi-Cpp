#include <spdlog/spdlog.h>
#include "mqtt/async_client.h"
#include <string>
#include "config/config.h"
#include <vector>
#include "strip/strip-factory.hpp"
#include "network/RemoteStripServer.h"
#include <chrono>
#include <thread>
#include "network/MQTTClient.hpp"

using namespace std;

std::vector<std::shared_ptr<LedPi::IStrip>> strips;
bool running = true;

int main() {
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [thread:%t] [%^%l%$] %v");
  LedPi::Config c;
  boost::asio::io_service ioService;
  auto stripServer = std::make_shared<LedPi::RemoteStripServer>(c.GetApplictionConfig()->debugPort, ioService);

  for(auto& el : c.GetStripConfigurations()) {
    try {
      auto strip = LedPi::StripFactory::makeStrip(el, stripServer);
      strips.push_back(std::move(strip));
    } catch(std::exception& e) {
      spdlog::error("Something went wrong {0}", e.what());
    }
  }

  spdlog::info("Setting up mqtt client");
  auto mClient = LedPi::MQTTClient(c.GetNetworkConfig(), strips);

  auto thread = std::thread([&]() {
    ioService.run();
  });
  running = true;
  while(running) {
    for(auto& el: strips) {
      el->Render();
    }
    // 15 fps
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
  }

  thread.join();
  mClient.Shutdown();
  return 0;
}