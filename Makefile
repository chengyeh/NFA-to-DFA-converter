project1: main.o
	g++ -g -Wall -std=c++11 -o project1 main.o
main.o: main.cpp
	g++ -g -Wall -std=c++11 -c main.cpp
run: project1
	./project1 < input.txt	
clean:
	rm *.o project1