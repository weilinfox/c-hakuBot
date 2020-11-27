main: request.o server.o hakuMind.o json.o api.o errorMsg.o plugin.o main.o
	cc request.o server.o hakuMind.o json.o api.o errorMsg.o plugin.o main.o `pkg-config --libs json-glib-1.0` -lcurl -luv -ldl -Wl,-z,origin -Wl,-rpath='$ORIGIN' -o main -g
main.o: main.c main.h hakuCore/errorMsg.h
	cc -c main.c `pkg-config --cflags json-glib-1.0` -o main.o
hakuMind.o: hakuCore/hakuMind.c hakuCore/hakuMind.h hakuCore/errorMsg.h
	cc -c hakuCore/hakuMind.c `pkg-config --cflags json-glib-1.0` -o hakuMind.o
request.o: hakuCore/request.c hakuCore/request.h hakuCore/errorMsg.h
	cc -c hakuCore/request.c -o request.o
server.o: hakuCore/server.c hakuCore/server.h hakuCore/errorMsg.h
	cc -c hakuCore/server.c `pkg-config --cflags json-glib-1.0` -o server.o
json.o: hakuCore/json.c hakuCore/json.h hakuCore/errorMsg.h
	cc -c hakuCore/json.c `pkg-config --cflags json-glib-1.0` -o json.o
api.o: hakuCore/api.c hakuCore/api.h hakuCore/errorMsg.h
	cc -c hakuCore/api.c -o api.o
errorMsg.o: hakuCore/errorMsg.c hakuCore/errorMsg.h
	cc -c hakuCore/errorMsg.c -o errorMsg.o
plugin.o: hakuCore/plugin.c hakuCore/plugin.h
	cc -c hakuCore/plugin.c -o plugin.o

.PHONY: clean
clean:
	-rm *.o
	-rm main
