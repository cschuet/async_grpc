/*
 * test_server_main.cc
 *
 *  Created on: Sep 23, 2018
 *      Author: chris
 */

#include "async_grpc/test_server.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(grpc_threads, 1, "");
DEFINE_int32(event_threads, 1, "");

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = true;
  google::ParseCommandLineFlags(&argc, &argv, true);
  TestServer test_server(FLAGS_grpc_threads, FLAGS_event_threads);
  test_server.Start();
  test_server.WaitForShutdown();
}
