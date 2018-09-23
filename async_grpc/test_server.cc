#include "async_grpc/test_server.h"

TestServer::TestServer(size_t no_grpc_threads, size_t no_event_threads) {
  async_grpc::Server::Builder server_builder;
  server_builder.SetServerAddress("127.0.0.1:50051");
  server_builder.SetNumGrpcThreads(no_grpc_threads);
  server_builder.SetNumEventThreads(no_event_threads);
  server_builder.RegisterHandler<EchoHandler>();
  server_ = server_builder.Build();
}

void TestServer::Start() { server_->Start(); }

void TestServer::WaitForShutdown() { server_->WaitForShutdown(); }
