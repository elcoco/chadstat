statusline: main.o
	g++ main.o -lpulse-simple -lasound -lcurl -o statusline

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o statusline

install:
	cp -f statusline ~/bin/apps
