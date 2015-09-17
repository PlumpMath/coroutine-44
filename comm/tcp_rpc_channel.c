#include "tcp_rpc_channel.h"
#include "echo.pb.h"
#include "kern.h"
#include <string>

using namespace std;
namespace comm
{
    TcpRpcChannel::TcpRpcChannel():RpcChannel(){}
    TcpRpcChannel::TcpRpcChannel(const std::string ip,int port):ip(ip),port(port){}
//    TcpRpcChannel::TcpRpcChannel(struct sockaddr_in &stAddr):addr(stAddr){}
    TcpRpcChannel::~TcpRpcChannel(){}

    void TcpRpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method, ::google::protobuf::RpcController* controller,
            const ::google::protobuf::Message* request, ::google::protobuf::Message* response, ::google::protobuf::Closure* done)
    {
        RpcMeta meta;
        meta.set_service_name(method->service()->name());
        meta.set_method_name(method->name());
        meta.set_len(request->ByteSize());
    }
} // namespace comm

