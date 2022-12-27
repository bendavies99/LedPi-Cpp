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
      PacketReciever(int port, boost::asio::io_service& ioService);
      void Subscribe(std::function<void(uint8_t /*id*/, std::vector<uint32_t> /* colors */)> func);

    private:
      std::vector<std::function<void(uint8_t, std::vector<uint32_t>)>> m_Subscribers;
      std::thread m_Thread;
      udp::socket m_Socket;
      udp::endpoint remote_endpoint_;
      std::array<char, 1024> recv_buffer_;
      void RecieveMessage();
  };
}