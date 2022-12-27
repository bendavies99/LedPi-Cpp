#include "PacketReciever.h"
#include <spdlog/spdlog.h>

LedPi::PacketReciever::PacketReciever(int port)
 : m_Socket(m_IoService, udp::endpoint(boost::asio::ip::address::from_string("0.0.0.0"), port))
{
  RecieveMessage();
}

void LedPi::PacketReciever::Subscribe(std::function<void(uint8_t, std::vector<uint32_t>)> func)
{
  m_Subscribers.push_back(func);
}

void LedPi::PacketReciever::Start()
{
  m_Thread = std::thread([&]() {
    m_IoService.run();
  });
}

void LedPi::PacketReciever::Stop()
{
  m_IoService.stop();
}

void LedPi::PacketReciever::RecieveMessage()
{
m_Socket.async_receive_from(boost::asio::buffer(recv_buffer_), remote_endpoint_, [&](std::error_code ec, size_t bytes_read)
                              {
        if (ec) {
        spdlog::error(ec.message());
        return;
        }

        uint8_t id = recv_buffer_[0];
        std::vector<uint32_t> data((bytes_read - 1) / 3);
        for(int i = 0; i < bytes_read - 1; i+= 3) {
          uint8_t r = recv_buffer_[i] & 0xFF;
          uint8_t g = recv_buffer_[i + 1] & 0xFF;
          uint8_t b = recv_buffer_[i + 2] & 0xFF;
          uint32_t c = (0xFF << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
          data[i] = c;
        }

        for(const auto& el : m_Subscribers) {
          el(id, data);
        }

        RecieveMessage(); 
      });
}
