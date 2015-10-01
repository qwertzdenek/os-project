.PHONY: libdo.a

libdo.a: lib.o calls.o
	ar rcs $@ $^

lib.o: lib.c lib.h
	gcc -c -o $@ $<

calls.o: calls.c calls.h
	gcc -c -o $@ $<

clean:
	rm -f *.o *.a

