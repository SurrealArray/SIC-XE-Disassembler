###########################################
#  Omar Beltran
#  SIC/XE Disassembler
###########################################

FILES = disassembler.cpp
FLAG = -std=c++11
EXEC = dissem
CC = g++

$(EXEC):
	$(CC) $(FLAG) -o $(EXEC) $(FILES)
	rm -f *.o

clean: 
	rm -f *.o core a.out $(EXEC)
	rm -f sample.lis.txt

###########################################
