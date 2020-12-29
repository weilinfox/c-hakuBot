PLUGIN_DIR = pluginsrc

main: request.o server.o hakuMind.o json.o api.o errorMsg.o plugin.o main.o plugins
	cc request.o server.o hakuMind.o json.o api.o errorMsg.o plugin.o main.o `pkg-config --libs json-glib-1.0 python3` -lcurl -luv -rdynamic -ldl -Wl,-z,origin -Wl,-rpath='$$ORIGIN' -o main -g
main.o: main.c main.h hakuCore/errorMsg.h
	cc -c main.c `pkg-config --cflags json-glib-1.0 python3` -o main.o
hakuMind.o: hakuCore/hakuMind.c hakuCore/hakuMind.h hakuCore/errorMsg.h
	cc -c hakuCore/hakuMind.c `pkg-config --cflags json-glib-1.0 python3` -o hakuMind.o
request.o: hakuCore/request.c hakuCore/request.h hakuCore/errorMsg.h
	cc -c hakuCore/request.c -o request.o
server.o: hakuCore/server.c hakuCore/server.h hakuCore/errorMsg.h
	cc -c hakuCore/server.c `pkg-config --cflags json-glib-1.0 python3` -o server.o
json.o: hakuCore/json.c hakuCore/json.h hakuCore/errorMsg.h
	cc -c hakuCore/json.c `pkg-config --cflags json-glib-1.0 python3` -o json.o
api.o: hakuCore/api.c hakuCore/api.h hakuCore/errorMsg.h
	cc -c hakuCore/api.c -o api.o
errorMsg.o: hakuCore/errorMsg.c hakuCore/errorMsg.h
	cc -c hakuCore/errorMsg.c -o errorMsg.o
plugin.o: hakuCore/plugin.c hakuCore/plugin.h
	cc -c hakuCore/plugin.c `pkg-config --cflags python3` -o plugin.o
plugins: $(PLUGIN_DIR)/
	$(MAKE) -C $(PLUGIN_DIR)
.PHONY: clean
clean:
	-rm *.o
	-rm main
	$(MAKE) -C $(PLUGIN_DIR) clean

.PHONY: install
install:
	-mkdir -p c-hakuBot/plugin
	-cp main c-hakuBot/hakuBot
	-cp $(PLUGIN_DIR)/*.so c-hakuBot/plugin/
	-cp $(PLUGIN_DIR)/*.py c-hakuBot/plugin/
