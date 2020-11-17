main: sendRequest.o main.o
	cc sendRequest.o main.o -lcurl -o main
main.o: main.c main.h
	cc -c main.c -o main.o
sendRequest.o: hakuCore/sendRequest.c hakuCore/sendRequest.h
	cc -c hakuCore/sendRequest.c -o sendRequest.o

