obj= log.o context.o kern.o pbjson.o
all:a.out
a.out:$(obj)
	g++ -g -o $@ $^
	-rm -rf *.o core*
%.o:%.c
	g++ -g -o $@ -c $<
clean:
	-rm -rf *.o a.out
    
