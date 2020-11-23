main: sendRequest.o server.o hakuMain.o json.o main.o
	cc sendRequest.o server.o hakuMain.o json.o main.o `pkg-config --libs json-glib-1.0` -lcurl -luv -o main
main.o: main.c main.h
	cc -c main.c `pkg-config --cflags json-glib-1.0` -o main.o
hakuMain.o: hakuCore/hakuMain.c hakuCore/hakuMain.h
	cc -c hakuCore/hakuMain.c -o hakuMain.o
sendRequest.o: hakuCore/sendRequest.c hakuCore/sendRequest.h
	cc -c hakuCore/sendRequest.c -o sendRequest.o
server.o: hakuCore/server.c hakuCore/server.h
	cc -c hakuCore/server.c -o server.o
json.o: hakuCore/json.c hakuCore/json.h
	cc -c hakuCore/json.c `pkg-config --cflags json-glib-1.0` -o json.o

.PHONY: clean
clean:
	-rm *.o
	-rm main
