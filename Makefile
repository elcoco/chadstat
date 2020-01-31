CXX = g++
CXXFLAGS = -lasound -lcurl -Wall

# target: dependencies
# 	  action


statusline: main.o blocks.o utils.o
	$(CXX) main.o blocks.o utils.o $(CXXFLAGS) -o statusline

main.o: main.cpp 
	$(CXX) -c main.cpp

blocks.o: blocks.cpp blocks.h
	$(CXX) -c blocks.cpp

utils.o: utils.cpp utils.h
	$(CXX) -c utils.cpp

clean:
	rm *.o

install:
	cp -f statusline ~/bin/apps
