#pragma once

#include "boost/asio.hpp"
#include <spdlog/spdlog.h>
#include <memory>
#include <vector>
#include <array>
#include <stdint.h>

using boost::asio::ip::udp;

namespace LedPi
{
  class RemoteStripServer
  {
  private:
    boost::asio::io_service& m_ioService;
    udp::socket m_Socket;
    std::vector<udp::endpoint> m_Clients;
    udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;
  public:
    RemoteStripServer(int port, boost::asio::io_service& ioService);
    void SendRenderData(int uid, std::vector<uint32_t>& colors);
  private:
    void RecieveMessage();
    void ConnectClient(udp::endpoint address);
    void DisconnectClient(udp::endpoint address);
  };
}