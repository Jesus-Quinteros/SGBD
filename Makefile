all: main

main:
	g++ main.cpp src/Sector.cpp src/Bloque.cpp src/Pista.cpp src/Superficie.cpp src/Plato.cpp src/Disco.cpp -o main -Iincludes

clean:
	rm -f main *.o
