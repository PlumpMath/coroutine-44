#ifndef _TCP_RPC_CHANNEL_H_
#define _TCP_RPC_CHANNEL_H_
#include <stdio.h>
#include <string>
#include <stdint.h>
#include <google/protobuf/service.h>
#include <sys/types.h>       
#include <sys/socket.h>

namespace comm{
    class TcpRpcChannel : public google::protobuf::RpcChannel
    {
        public:
            TcpRpcChannel();
            TcpRpcChannel( const std::string ip,int port);
            //TcpRpcChannel( struct sockaddr_in &stAddr);
            virtual ~TcpRpcChannel();
            virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method, ::google::protobuf::RpcController* controller,
                    const ::google::protobuf::Message* request, ::google::protobuf::Message* response, ::google::protobuf::Closure* done);
        private:
            std::string ip;
            int port;
            //struct sockaddr_in addr;
    }; 
}
#endif
