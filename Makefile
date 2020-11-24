main: request.o server.o hakuMain.o json.o main.o
	cc request.o server.o hakuMain.o json.o main.o `pkg-config --libs json-glib-1.0` -lcurl -luv -o main
main.o: main.c main.h hakuCore/errorMsg.h
	cc -c main.c `pkg-config --cflags json-glib-1.0` -o main.o
hakuMain.o: hakuCore/hakuMain.c hakuCore/hakuMain.h hakuCore/errorMsg.h
	cc -c hakuCore/hakuMain.c -o hakuMain.o
request.o: hakuCore/request.c hakuCore/request.h hakuCore/errorMsg.h
	cc -c hakuCore/request.c -o request.o
server.o: hakuCore/server.c hakuCore/server.h hakuCore/errorMsg.h
	cc -c hakuCore/server.c -o server.o
json.o: hakuCore/json.c hakuCore/json.h hakuCore/errorMsg.h
	cc -c hakuCore/json.c `pkg-config --cflags json-glib-1.0` -o json.o

.PHONY: clean
clean:
	-rm *.o
	-rm main
