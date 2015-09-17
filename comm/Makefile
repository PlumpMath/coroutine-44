obj= log.o context.o kern.o pbjson.o test.o echo.pb.o echo.pb.serviceimpl.o
LIB=-lprotobuf
all:a.out
a.out:$(obj) 
	g++ -g -o $@ $^ $(LIB)
	-rm -rf *.o core*
%.o:%.c
	g++ -g -o $@ -c $<
%.o:%.cc
	g++ -g -o $@ -c $<
pbfiles:
	protoc --cpp_out=./ --proto_path=./ echo.proto 
clean:
	-rm -rf *.o a.out log
	-rm -rf *.pb.cc  *.pb.h
    
