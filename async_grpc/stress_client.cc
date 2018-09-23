#include <ctime>
#include <memory>
#include <thread>
#include <vector>

#include "async_grpc/client.h"
#include "async_grpc/common/make_unique.h"
#include "async_grpc/test_server.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(sender_threads, 1, "");

namespace {

constexpr char kServerAddress[] = "127.0.0.1:50051";
constexpr int kSecondsToSend = 10;

}  // namespace

class Sender {
 public:
  Sender() {
    channel_ = ::grpc::CreateChannel(kServerAddress,
                                     ::grpc::InsecureChannelCredentials());
  }

  void Start() {
    start_time_ = std::clock();
    LOG(INFO) << "Start";
    sender_thread_ =
        async_grpc::common::make_unique<std::thread>([this]() { DoSending(); });
    LOG(INFO) << "!Start";
  }

  void WaitForCompletion() {
    LOG(INFO) << "WaitForCompletion";
    sender_thread_->join();
  }

  int64_t sent_messages() const { return sent_messages_; }

 private:
  void DoSending() {
    while (true) {
      if (counter % 10 == 0) {
        std::clock_t now = std::clock();
        double elapsed_secs = double(now - start_time_) / CLOCKS_PER_SEC;
        if (elapsed_secs > kSecondsToSend) {
          return;
        }
        counter = 0;
      }

      async_grpc::Client<EchoMethod> client(channel_);
      ::grpc::Status status;
      proto::Request request;
      request.set_input(2);
      if (!client.Write(request, &status)) {
        LOG(ERROR) << status.error_code() << " " << status.error_message();
      }
      ++sent_messages_;
      ++counter;
    }
  }
  int counter = 0;
  int64_t sent_messages_ = 0;
  std::unique_ptr<std::thread> sender_thread_;
  std::shared_ptr<::grpc::Channel> channel_;
  std::clock_t start_time_;
};

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = true;
  google::ParseCommandLineFlags(&argc, &argv, true);
  std::vector<Sender> senders;
  senders.resize(FLAGS_sender_threads);
  for (auto& sender : senders) {
    sender.Start();
  }
  int64_t total = 0;
  for (auto& sender : senders) {
    sender.WaitForCompletion();
    total += sender.sent_messages();
    LOG(INFO) << "sent messages: " << sender.sent_messages();
  }
  LOG(INFO) << "QPS: " << ((double)total) / (double)kSecondsToSend;

  return 0;
}
