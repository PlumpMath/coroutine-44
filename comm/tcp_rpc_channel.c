#include "tcp_rpc_channel.h"
#include "echo.pb.h"
#include "kern.h"
#include "log.h"
#include <string>
#include <stdint.h>

using namespace std;
namespace comm
{
    TcpRpcChannel::TcpRpcChannel():RpcChannel(){}
    TcpRpcChannel::TcpRpcChannel(const std::string ip,int tport):ip(ip),port(tport){}
//    TcpRpcChannel::TcpRpcChannel(struct sockaddr_in &stAddr):addr(stAddr){}
    TcpRpcChannel::~TcpRpcChannel(){}

    void TcpRpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method, ::google::protobuf::RpcController* controller,
            const ::google::protobuf::Message* request, ::google::protobuf::Message* response, ::google::protobuf::Closure* done)
    {
        RpcMeta meta;
        string req;
        request->SerializeToString(&req);
        meta.set_service_name(method->service()->name());
        meta.set_method_name(method->name());
        meta.set_len(req.size());
        int fd = m_connect(ip.c_str(), port, 500);
        log_debug("connect to %s:%d", ip.c_str(), port);
        string meta_req;
        meta.SerializeToString(&meta_req);
        uint32_t meta_len = meta_req.size();
        m_send(fd, (char*)&meta_len, sizeof(meta_len), 500);
        log_debug("send 1, size:4");

        m_send(fd, (char*)meta_req.data(), meta_req.size(), 500);
        log_debug("send 2, meta size:%d", req.size());

        m_send(fd, (char*)req.data(), req.size(), 500);
        log_debug("send 3, data size:%d", req.size());

        m_recv(fd, (char*)&meta_len, sizeof(meta_len), 500);
        log_debug("recv 1");
        char* buf = (char*)malloc(meta_len); 
        if(buf == NULL) return;
        m_recv(fd, buf, meta_len, 500);
        log_debug("recv 2");
        response->ParseFromArray(buf, meta_len);
        free(buf);
        m_close(fd);
    }
} // namespace comm

