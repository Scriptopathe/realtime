CC=gcc
LINK=g++
CFLAGS=-I../include -I../include/x86_64-linux-gnu -I../xenomai/include  -I../lib_destijl/headers
OBJ=fonctions.o global.o main.o video.o
OBJ_SRC=../lib_destijl/source/*.o
LIB= -L../libs/x86_64-linux-gnu -L../libs -L../libs/gcc/x86_64-linux-gnu/4.6 -L../xenomai/lib -Wl,-rpath=../libs -Wl,-rpath=../libs/x86_64-linux-gnu -lopencv_core -lopencv_highgui -lopencv_imgproc -lxenomai -lnative 
WARN= 
all: main


main: $(OBJ) 						
	$(LINK) $(OBJ) $(OBJ_SRC) -o robot $(LIB)

fonctions.o: fonctions.c dmessage.o fonctions.h 
	$(CC) -g $(WARN) $(CFLAGS)  -c fonctions.c -o fonctions.o

video.o: video.c video.h dmessage.o
	$(CC) -g $(WARN) $(CFLAGS)  -c video.c -o video.o

global.o: global.c global.h
	$(CC) -g $(WARN) $(CFLAGS) -c global.c -o global.o

main.o: main.c global.h fonctions.h
	$(CC) -g $(WARN) $(CFLAGS) -c main.c -o main.o

dmessage.o: ../lib_destijl/source/dmessage.o
	touch dmessage.o

../lib_destijl/source/dmessage.o: ../lib_destijl/source/dmessage.c ../lib_destijl/headers/dmessage.h ../lib_destijl/headers/dtools.h  
	cd ../lib_destijl/source; echo "entering source"; \
	$(CC) -g $(CFLAGS)  -c *.c
	cd ..

clean:
	rm -f $(OBJ) $(OBJ_SRC) robot
