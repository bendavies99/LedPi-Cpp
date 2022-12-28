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
#include "network/PacketReciever.h"
#ifdef PRODUCTION_MODE
#include "strip/NativeStrip.hpp"
#endif

using namespace std;

std::vector<std::shared_ptr<LedPi::IStrip>> strips;
bool running = true;

int main() {
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [thread:%t] [%^%l%$] %v");
  LedPi::Config c;
  boost::asio::io_service ioService;
  boost::asio::io_service ioService2;
  auto stripServer = std::make_shared<LedPi::RemoteStripServer>(c.GetApplictionConfig()->debugPort, ioService);

  #ifdef PRODUCTION_MODE
    LedPi::Native::InitNativeLeds();
  #endif
  for(auto& el : c.GetStripConfigurations()) {
    try {
      auto strip = LedPi::StripFactory::makeStrip(el, stripServer);
      strips.push_back(std::move(strip));
    } catch(std::exception& e) {
      spdlog::error("Something went wrong {0}", e.what());
    }
  }


  #ifdef PRODUCTION_MODE
    LedPi::Native::RunInitFn();
  #endif
  spdlog::info("Setting up mqtt client");
  auto mClient = LedPi::MQTTClient(c.GetNetworkConfig(), strips);

  auto pr = LedPi::PacketReciever(c.GetNetworkConfig()->reactivePort, ioService2);
  pr.Subscribe([&](uint8_t id, std::vector<uint32_t> cols) {
    for(const auto& el : strips) {
      if ((el->GetUID() & 0xFF) == id && el->GetStripMode() == LedPi::StripMode::NETWORK_UDP && el->GetBrightness() > 0)
      {
        el->SetStripColors(cols);
      }
    }
  });

  auto thread = std::thread([&]() {
    ioService.run();
  });
  auto thread2 = std::thread([&]() {
    ioService2.run();
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
  thread2.join();
  mClient.Shutdown();
  return 0;
}