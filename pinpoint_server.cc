#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

#include <grpc/grpc.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include "Span.grpc.pb.h"
#include "Service.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using google::protobuf::Empty;
using namespace v1;

class SpanImpl final : public Span::Service
{
	public:
	explicit SpanImpl() {}
	Status SendSpan(ServerContext* context, ServerReader<PSpanMessage>* reader, Empty* response) override
	{
		PSpanMessage msg;

		std::cout << "Message Arrived : \n";
		
		while(reader->Read(&msg)) {
			if (msg.has_span()) {
				PSpan span = msg.span();
				std::cout << span.starttime() << span.elapsed() << "\n";
			} else {
				std::cout << "No Span\n";
			}
		}


		std::cout << "Client metadata: " << std::endl;
		const std::multimap<grpc::string_ref, grpc::string_ref> metadata = context->client_metadata();
		for (auto iter = metadata.begin(); iter != metadata.end(); ++iter) {
			std::cout << "Header key: " << iter->first << ", value: ";
			size_t isbin = iter->first.find("-bin");
			if ((isbin != std::string::npos) && (isbin + 4 == iter->first.size())) {
				std::cout <<  std::hex;
				for (auto c : iter->second) {
					std::cout << static_cast<unsigned int>(c);
				}
				std::cout <<  std::dec;
			} else {
				std::cout << iter->second;
			}
			std::cout << std::endl;
		}
		return Status::OK;
	}
};

int main(int argc, char** argv)
{
	std::string server_address("0.0.0.0:9994");
	SpanImpl service;
	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();

}
