clientobj= log.o context.o kern.o pbjson.o client.o echo.pb.o echo.pb.serviceimpl.o tcp_rpc_channel.o
serverobj= log.o context.o kern.o pbjson.o server.o echo.pb.o echo.pb.serviceimpl.o tcp_rpc_channel.o
LIB=-lprotobuf
all:client server
client:$(clientobj) 
	g++ -g -o $@ $^ $(LIB)
	#-rm -rf *.o core*
server:$(serverobj) 
	g++ -g -o $@ $^ $(LIB)
	#-rm -rf *.o core*
%.o:%.c
	g++ -g -o $@ -c $<
%.o:%.cc
	g++ -g -o $@ -c $<
pbfiles:
	protoc --cpp_out=./ --proto_path=./ echo.proto 
clean:
	-rm -rf *.o client server clientlog serverlog
	#-rm -rf *.pb.cc  *.pb.h
    
