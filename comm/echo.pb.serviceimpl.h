#include "echo.pb.h"

class MyServiceImpl: public MyService
{
    void Echo(::google::protobuf::RpcController* controller, const ::EchoReq* request, ::EchoRsp* response, ::google::protobuf::Closure* done);
};
