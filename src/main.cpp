#include <spdlog/spdlog.h>
#include "mqtt/async_client.h"
#include <string>
#include "config/config.h"
#include <vector>
#include "strip/strip-factory.hpp"
#include "network/RemoteStripServer.h"
#include <chrono>
#include <thread>

using namespace std;

const string SERVER_ADDR { "tcp://localhost:1883" };
const string CLIENT_ID { "test_client" };
std::vector<std::unique_ptr<LedPi::IStrip>> strips;
bool running = true;


int main() {
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [thread:%t] [%^%l%$] %v");
  LedPi::Config c;
  boost::asio::io_service ioService;
  auto stripServer = std::make_shared<LedPi::RemoteStripServer>(c.GetApplictionConfig()->debugPort, ioService);

  for(auto& el : c.GetStripConfigurations()) {
    auto strip = LedPi::StripFactory::makeStrip(el, stripServer);
    strips.push_back(std::move(strip));
  }

  // //TODO: MQTT Stuff 
  // mqtt::async_client client(SERVER_ADDR, CLIENT_ID);

  // auto connOpts = mqtt::connect_options_builder()
  //                      .clean_session()
  //                      .connect_timeout(chrono::milliseconds(5000))
  //                      .automatic_reconnect(true)
  //                      .finalize();

  // try {
  //   spdlog::info("Connecting to the mqtt server");
  //   client.connect(connOpts)->wait();

  //   // client.disconnect()->wait();

  // } catch(const mqtt::exception& e) {
  //   spdlog::error("An Error occurred {0}", e.to_string());
  //   return 1;
  // }


  auto thread = std::thread([&]() {
    ioService.run();
  });

  while(running) {
    for(auto& el: strips) {
      el->Render();
    }
    // 15 fps
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 15));
  }

  thread.join();
  return 0;
}