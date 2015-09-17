#include "echo.pb.serviceimpl.h"

void MyServiceImpl::Echo(::google::protobuf::RpcController* controller, const ::EchoReq* request, ::EchoRsp* response, ::google::protobuf::Closure* done)
{
    response->set_msg(request->msg());
    return;
}
