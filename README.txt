   
   Omar Beltran
   SIC/XE Disassembler
 

Files included: 
	disassembler.cpp, disassembler.hpp, Makefile, README.txt, sample-obj.txt, sample.sym.txt, assign1_text.obj.txt, assign1.sym.txt

Compile Instructions (Assuming source code is in ~/a2 folder):
	$ cd a2
	$ make


Starting the program:
To start this program, you can use the Makefile to make the executable. To do this, type "make" in the program's and Makefile's directory. Upon doing this, "dissem" executable file is created. With this the user must type the executable with the object file and symbol file in that order. An example of this would be "dissem sample-obj.txt sample.sym.txt". Once the user has inputted this with the correct input files, the program will output a listing file called "sample.lis.txt". To clean up the directory from the outputs, use "make clean" which will remove the "dissem" executable as well as the "sample.lis.txt" file. 



How the program works:
My disassembler program starts by getting symbol and object files. It first opens the object and creates a listing file. The object file will be read line by line. On the first line, it will get the header record. On every line after that that starts with 'T', it will get the text record. The program will ignore modification records. On the last line, it will get the end record. After it is done with each record, it will close to two files and close the program. 

With the header record, it will print the address of where it starts and converts it to an integer (to calculate addresses). It will then print the header name and once again, where the program address starts except without any leading zeroes. It will save the starting address for then end record and save the ending address to calculate any remaining label RESWs. 

With the text record, it will get the starting address of the text record and if this address doesn't match with the current address, it will get all the RESW labels until the starting address of the text record matches with the current address. It will then get the length of the text record, which is used to determined how long to continue iterating through the text record line. The address of the current address is printed and then will check if the address is a literal address to print the "LTORG". Next, it will check the address is the label or literal address. If it is a literal it will skip going though the opcode. When going through opcode the program will remove the two rightmost bits for the opcode. It will then iterate through the data structure mnemonic table and check if that opcode matches with an instruction. If it does it will go through the object code and determine the flags and displacement/address. There are a few special cases like that require hardcoding, like if the opcode is the "LDB" instruction, then the displacement is save as the Base register and saved globally. The same is done for the "LDX" instruction. Then the program counter is set in case it is need when the P flag is set. The mnemonic instruction is printed and then will check if the instruction is simple, immediate, or indirect. Two other special cases are implemented and that is when the opcode equals to "CLEAR" where it will print the register based on r1 of the object code. Another special case is when the opcode is "RSUB" and that means nothing is printed next to it. The next thing the program does is check what the flags of X, B, P, and E are. If E = 1, then it is format 4 and a variable int equalling 2 is set which will help set the next address, the length of the object code, and the the counter of where it is pointing the text record. If X = 1, the register of X is added by the displacement/address to the target address. If B = 1, the Base register is added by the displacement/address to the target address. If P = 1, the program counter register is added by the displacement/address to the target address. If the X, B, P, and E are all equal to 0, the program will print the address of the object code depending on if it is format 4 or format 3. For every other case, the target address is compared to the label address as well as the literal address and if one of them match, then they the label or literal are printed. After that the object code is printed and the new current address as well the counter of the text record are calculated. Also, a special case is implemented that if the opcode is "LDB" it will print the "BASE" with the label. This text record will keep iterating through the text record until it has reached the length of the text record. 

With the end record, it will first start by printing all remaining label RESW if end of program address is greater than the current address. Then it will print "END" and lastly print the label of the starting address of the program.

I have another function that is used to check the current address with the label/literal and see if their addresses are equal. It will then print the label/literal. This function is used after printing the current address as well as after the opcode instruction, if it needs to print a label. 

I also have another function that is used to print all RESW labels. The way this works is that an address parameter is inputted and the function will print all labels of RESW between the current address and address parameter. This function is used in the beginning of the text record function to fill in the gaps between text record addresses. It is also used in the beginning of the end record function to print all remains labels of RESW. 
