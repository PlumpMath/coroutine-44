obj= log.o context.o kern.o hashmap.o list.o
all:a.out
a.out:$(obj)
	gcc -g -o $@ $^
	rm *.o
%.o:%.c
	gcc -g -o $@ -Wpointer-to-int-cast -c $<
clean:
	rm -rf *.o a.out
    
