statusline: main.o
	g++ main.o -lpulse-simple -lpulse -lasound -o statusline

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o statusline

install:
	cp -f statusline ~/bin/apps
