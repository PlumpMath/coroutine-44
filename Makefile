obj= log.o context.o kern.o hashmap.o list.o
src= $(wildcard *c)
#obj=$(patsubst  %.c,%.o,$(src))
all:a.out
a.out:$(obj)
	gcc -g -o $@ $^
	rm -rf *.o core*
%.o:%.c
	gcc -g -o $@ -Wpointer-to-int-cast -c $<
#$(obj):$(src)
#	gcc -g -o $@ -Wpointer-to-int-cast -c $<
clean:
	rm -rf *.o a.out
    
