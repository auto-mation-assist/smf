// Copyright (c) 2016 Alexander Gallego. All rights reserved.
//
#include "rpc/rpc_client.h"
#include <memory>
#include <utility>
// seastar
#include <core/reactor.hh>
#include <net/api.hh>
// smf
#include "platform/log.h"

namespace smf {

rpc_client::rpc_client(ipv4_addr addr) : server_addr(std::move(addr)) {}

future<> rpc_client::stop() { return make_ready_future(); }
rpc_client::~rpc_client() {}

void rpc_client::enable_histogram_metrics(bool enable) {
  if (enable) {
    if (!hist_) {
      hist_ = std::make_unique<histogram>();
    }
  } else {
    hist_ = nullptr;
  }
}

future<> rpc_client::connect() {
  LOG_THROW_IF(
    conn_,
    "Client already connected to server: `{}'. connect called more than once.",
    server_addr);

  socket_address local =
    socket_address(::sockaddr_in{AF_INET, INADDR_ANY, {0}});
  return engine()
    .net()
    .connect(make_ipv4_address(server_addr), local, seastar::transport::TCP)
    .then([this](connected_socket fd) mutable {
      conn_ = std::make_unique<rpc_connection>(std::move(fd));
    });
}
}  // namespace smf
