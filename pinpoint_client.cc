#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <grpcpp/grpcpp.h>
#include "Span.grpc.pb.h"
#include "Service.grpc.pb.h"
#include "Cmd.grpc.pb.h"
#include "Stat.grpc.pb.h"
#include "ThreadDump.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientWriter;

int main(int argc, char** argv) {
	ClientContext context;
	context.AddMetadata("agentid", "test");
	context.AddMetadata("applicationname", "test");
	context.AddMetadata("starttime", "test");

	google::protobuf::Empty empty;
	v1::PSpanMessage msg;
	v1::PSpan span;
	span.set_starttime(9999);
	span.set_elapsed(10);

	msg.mutable_span()->CopyFrom(span);

	std::string span_host = "localhost:9994";
	std::shared_ptr<Channel> span_channel = grpc::CreateChannel(span_host, grpc::InsecureChannelCredentials());
	std::unique_ptr<v1::Span::Stub> span_stub = v1::Span::NewStub(span_channel);
	std::unique_ptr<ClientWriter<v1::PSpanMessage>> writer(span_stub->SendSpan(&context, &empty));
	writer->Write(msg);
	writer->WritesDone();
	Status status = writer->Finish();

	if (status.ok()) {
		std::cout << "Finished \n";
	} else {
		status.error_message();
		status.error_code();
		std::cout << "fail!! " << status.error_message() << " " << status.error_code();
		std::cout << "rpc failed\n";
	}

  return 0;
}
