plugins: wyy.so #yiyan.so
wyy.so: request.o json.o api.o
	cc wyy.c request.o json.o api.o -fPIC -shared `pkg-config --cflags json-glib-1.0` -lcurl -luv `pkg-config --libs json-glib-1.0` -o wyy.so -g
#yiyan.so: request.o json.o api.o
#	cc yiyan.c json.o api.o request.o -fPIC -shared `pkg-config --cflags json-glib-1.0` -lcurl -luv `pkg-config --libs json-glib-1.0` -o yiyan.so -g
request.o: hakuCore/request.h hakuCore/request.c
	cc -c hakuCore/request.c -lcurl -fPIC -o request.o
json.o: hakuCore/json.h hakuCore/json.c
	cc -c hakuCore/json.c `pkg-config --cflags json-glib-1.0` -fPIC -o json.o
api.o: hakuCore/api.h hakuCore/api.c
	cc -c hakuCore/api.c -fPIC -o api.o

.PHONY: clean
clean:
	-rm *.so
	-rm *.o
