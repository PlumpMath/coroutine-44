#include "echo.pb.serviceimpl.h"
#include "log.h"
void MyServiceImpl::Echo(::google::protobuf::RpcController* controller, const ::EchoReq* request, ::EchoRsp* response, ::google::protobuf::Closure* done)
{
    response->set_msg(request->msg()+" ningdegang");
    log_debug("req:%s", request->msg().c_str());
    return;
}
