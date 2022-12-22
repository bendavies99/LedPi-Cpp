#include "RemoteStripServer.h"
#include <string>
#include <iostream>
#include <spdlog/spdlog.h>

LedPi::RemoteStripServer::RemoteStripServer(int port, boost::asio::io_service &ioService)
    : m_ioService(ioService), m_Socket(ioService, udp::endpoint(udp::v4(), port))
{
  ConnectClient(udp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 25543));
  RecieveMessage();
}

void LedPi::RemoteStripServer::RecieveMessage()
{
  m_Socket.async_receive_from(boost::asio::buffer(recv_buffer_), remote_endpoint_, [&](std::error_code ec, size_t bytes_read)
                              {
      if (ec) {
      spdlog::error(ec.message());
      return;
      }
      std::string str(recv_buffer_.data(), bytes_read);
      spdlog::info("Recv: {0}", str);

      if (str == "exit")
      {
        m_ioService.stop();
      } else if (str == "ic") {
        ConnectClient(remote_endpoint_);
      } else if (str == "id") {
        DisconnectClient(remote_endpoint_);
      }
      RecieveMessage(); });
}

void LedPi::RemoteStripServer::ConnectClient(udp::endpoint address)
{
  if (std::none_of(m_Clients.begin(), m_Clients.end(), [&](const auto &rc)
                   { return rc == address; }))
  {
    std::string endpoint_string = address.address().to_string() + ":" + std::to_string(address.port());
    spdlog::info("Remote client connected: {0}", endpoint_string);
    m_Clients.push_back(address);
  }
}

void LedPi::RemoteStripServer::DisconnectClient(udp::endpoint address)
{
  auto it = std::find_if(m_Clients.begin(), m_Clients.end(), [&](const auto &rc)
                         { return rc == address; });
  if (it != m_Clients.end())
  {
    std::string endpoint_string = address.address().to_string() + ":" + std::to_string(address.port());
    spdlog::info("Remote client disconnected: {0}", endpoint_string);
    m_Clients.erase(it);
  }
}
void LedPi::RemoteStripServer::SendRenderData(int uid, std::vector<uint32_t>& colors)
{
  if (m_Clients.size() == 0)
    return;

  std::vector<uint8_t> bytes;
  bytes.push_back(uid & 0xFF);

  for (int i = 0; i < colors.size(); ++i)
  {
    bytes.push_back((colors[i] >> 16) & 0xFF);
    bytes.push_back((colors[i] >> 8) & 0xFF);
    bytes.push_back((colors[i]) & 0xFF);
  }

  for(auto& el: m_Clients) {
    m_Socket.async_send_to(boost::asio::buffer(bytes), el, [&](std::error_code ec, size_t bytes_read) {
      if (ec) {
        spdlog::error(ec.message());
        return;
      }
    });
  }
};
