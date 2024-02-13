#
# To compile prog:
#    make
#----------------------------------------------------------------------

all: serial-basic

serial-basic: serial-basic.c image-lib.c image-lib.h
	gcc -Wall -pedantic serial-basic.c image-lib.c -g -o ap-paralelo-1 -pthread -lgd

clean:
	rm ap-paralelo-1
