
CPPFLAGS = -Wall

main : CC = g++
main : main.o 

.PHONY : clean
clean :
	rm -f main.exe main.o