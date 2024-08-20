CC = g++
LIBS = -std=c++17 -lz -lstdc++fs $(shell pkg-config --cflags --libs opencv4)
SRCS = $(wildcard ${DIR_SRCS}*.cpp)
EXEC = $(DIR_BIN)creer_images
WARNING = -Wall



$(EXEC): $(SRCS)
	$(CC) $(WARNING) -O3 -o $(EXEC) $(SRCS) $(LIBS)

clean:
	rm -f $(EXEC)
