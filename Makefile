warp:
	g++ ./src/main.cpp `pkg-config opencv --libs --cflags` -o ./bin/warp

clean: 
	rm warp
