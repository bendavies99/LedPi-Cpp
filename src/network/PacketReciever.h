#pragma once

#include <functional>
#include <stdint.h>
#include <vector>
#include "boost/asio.hpp"
#include <thread>


using boost::asio::ip::udp;

namespace LedPi {
  class PacketReciever {
    public:
      PacketReciever(int port);
      void Subscribe(std::function<void(uint8_t /*id*/, std::vector<uint32_t> /* colors */)> func);
      void Start();
      void Stop();

    private:
      std::vector<std::function<void(uint8_t, std::vector<uint32_t>)>> m_Subscribers;
      boost::asio::io_service m_IoService;
      std::thread m_Thread;
      udp::socket m_Socket;
      udp::endpoint remote_endpoint_;
      std::array<char, 1024> recv_buffer_;
      void RecieveMessage();
  };
}