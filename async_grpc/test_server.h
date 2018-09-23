/*
 * test_server.h
 *
 *  Created on: Sep 23, 2018
 *      Author: chris
 */

#ifndef ASYNC_GRPC_TEST_SERVER_H_
#define ASYNC_GRPC_TEST_SERVER_H_

#include <memory>

#include "async_grpc/common/make_unique.h"
#include "async_grpc/proto/test.pb.h"
#include "async_grpc/rpc_handler.h"
#include "async_grpc/server.h"

struct EchoMethod {
  static constexpr const char* MethodName() { return "/proto.Test/Echo"; }
  using IncomingType = proto::Request;
  using OutgoingType = proto::Response;
};

class EchoHandler : public async_grpc::RpcHandler<EchoMethod> {
 public:
  void OnRequest(const proto::Request& request) override {
    auto response = async_grpc::common::make_unique<proto::Response>();
    response->set_output(request.input());
    Send(std::move(response));
  }
};

class TestServer {
 public:
  TestServer(size_t no_grpc_threads, size_t no_event_threads);
  void Start();
  void WaitForShutdown();

 private:
  std::unique_ptr<async_grpc::Server> server_;
};

#endif /* ASYNC_GRPC_TEST_SERVER_H_ */
