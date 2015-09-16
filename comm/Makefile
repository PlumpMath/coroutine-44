obj= log.o context.o kern.o pbjson.o
LIB=-lprotobuf
all:a.out
a.out:$(obj)
	g++ -g -o $@ $^ $(LIB)
	-rm -rf *.o core*
%.o:%.c
	g++ -g -o $@ -c $<
clean:
	-rm -rf *.o a.out log
    
