#include <spdlog/spdlog.h>
#include "mqtt/async_client.h"
#include <string>
#include "config/config.h";

using namespace std;

const string SERVER_ADDR { "tcp://localhost:1883" };
const string CLIENT_ID { "test_client" };

int main() {
  spdlog::info("Initialising for server {0}...", SERVER_ADDR);
  LedPi::Config c;


  mqtt::async_client client(SERVER_ADDR, CLIENT_ID);

  spdlog::info("Done...");

  auto connOpts = mqtt::connect_options_builder()
                       .clean_session()
                       .connect_timeout(chrono::milliseconds(5000))
                       .automatic_reconnect(true)
                       .finalize();

  try {
    spdlog::info("Connecting to the mqtt server");
    client.connect(connOpts)->wait();

  } catch(const mqtt::exception& e) {
    spdlog::error("An Error occurred {0}", e.to_string());
    return 1;
  }


  return 0;
}