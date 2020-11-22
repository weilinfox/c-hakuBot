main: sendRequest.o server.o hakuMain.o main.o
	cc sendRequest.o server.o hakuMain.o main.o -lcurl -luv -o main
main.o: main.c main.h
	cc -c main.c -o main.o
hakuMain.o: hakuCore/hakuMain.c hakuCore/hakuMain.h
	cc -c hakuCore/hakuMain.c -o hakuMain.o
sendRequest.o: hakuCore/sendRequest.c hakuCore/sendRequest.h
	cc -c hakuCore/sendRequest.c -o sendRequest.o
server.o: hakuCore/server.c hakuCore/server.h
	cc -c hakuCore/server.c -o server.o

.PHONY: clean
clean:
	-rm *.o
	-rm main
