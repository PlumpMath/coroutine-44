#include "tcp_rpc_channel.h"
#include "echo.pb.h"
#include "kern.h"
#include <string>
#include <stdint.h>

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
        int fd = m_connect(ip.c_str(), port, 500);
        uint32_t meta_len =  meta.ByteSize();
        m_send(fd, (char*)&meta_len, sizeof(meta_len), 500);
        string req;
        meta.SerializeToString(&req);
        m_send(fd, (char*)req.c_str(), request->ByteSize(), 500);
        m_recv(fd, (char*)&meta_len, sizeof(meta_len), 500);
        char* buf = (char*)malloc(meta_len); 
        if(buf == NULL) return;
        m_recv(fd, buf, meta_len, 500);
        response->ParseFromArray(buf, meta_len);
    }
} // namespace comm

