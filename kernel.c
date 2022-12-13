// JAMES EVANS
// CONNOR WALSH
// KERNEL

void printChar(char*);
void printString(char*);
void readString(char*);
void readSector(char*);
void handleInterrupt21(int, int, int, int);
void readFile(char*, char*, int);
void executeProgram(char*);
void terminate();
void deleteFile(char*);
void writeFile(char*, char*, int);
void handleTimerInterrupt(int, int);

int processActive[8];
int processSP[8];
int currentProcess;

void main() {

	char fileBuffer[13312];
//	char dir[512];
//	char buffer[512];
//	char line[80];
//	char test[80];
	int sectorsRead;
//	int processActive[8];
//	int processSP[8];
//	int currentProcess;
	int i;

	for(i = 0; i < 8; i = i + 1) {
		processActive[i] = 0;
		processSP[i] = 0xFF00;
	}
	currentProcess = -1;

	// TEST READ AND PRINT
//	printString("Enter a line: ");
//	readString(line);
//	printString(line);

	// TEST READSECTOR
//	readSector(buffer, 10);
//	printString(buffer);

	// TEST INTERRUPT
//	makeInterrupt21();
//	interrupt(0x21, 1, test, 0, 0);
//	interrupt(0x21, 0, test, 0, 0);


	// TEST READFILE
//	makeInterrupt21();
//	interrupt(0x21, 3, "messag", fileBuffer, &sectorsRead);

//	if(sectorsRead > 0) {
//		interrupt(0x21, 0, fileBuffer, 0, 0);
//	}
//	else {
//		interrupt(0x21, 0, "File not found...\r\n", 0, 0);
//	}

	// TEST LOADFILE
//	interrupt(0x21, 4, "tstpr1", 0, 0);

	// TEST TERMINATE
//	interrupt(0x21, 5, "tstpr2", 0, 0);

	// EXECUTE SHELL
	makeInterrupt21();
//	executeProgram("shellz");
	interrupt(0x21, 4, "shellz", 0, 0);
	makeTimerInterrupt();

	while(1) {
	}

}

void readFile(char* filename, char* fileBuffer, int* sectorsRead) {
	char dir[512];
	int i;
	int x;
	int entry;
	*sectorsRead = 0;
	x = 0;

	readSector(dir, 2);

	for(entry = 0; entry < 512; entry += 32) {
                for(i = 0; i < 6; ++i) {
                        if(filename[i] != dir[i + entry]) {
                                // NO MATCH
				*sectorsRead = 0;
                                break;
                        }
                        if(filename[i] == '\0' && dir[i + entry] == '\0') {
                                // FILENAME COMPLETE
				*sectorsRead = *sectorsRead + 1;
                                i = 6;
                                break;
                        }
			else if(i == 5) {
				// MATCH
				if(filename[i] == dir[i + entry]) {
					*sectorsRead = *sectorsRead + 1;

					while(dir[x] != 0) {
						readSector(fileBuffer, dir[entry + 6 + x]);
						fileBuffer = fileBuffer + 512;
						x = x + 1;
						*sectorsRead = *sectorsRead + 1;
					}
					return;
				}
			}
		}
	}
}

void executeProgram(char* name) {
	char buffer[13312];
	int i;
	int j;
	int segment;
	int dataseg;

	readFile(name, buffer, 0);

	dataseg = setKernelDataSegment();
	for(i = 0; i < 8; i = i + 1) {
		if(processActive[i] == 0) {
			segment = (i + 2) * 0x1000;
			for(j = 0; j < 13312; j = j + 1) {
				putInMemory(segment, i, buffer[i]);
			}
			initializeProgram(segment);
			processActive[i] = 1;
			processSP[i] = 0xFF00;
			restoreDataSegment(dataseg); // END
			break;
		}
	}
}

void terminate() {
	int dataseg;

	dataseg = setKernelDataSegment(); // BEGIN
	processActive[currentProcess] = 0;
	restoreDataSegment(dataseg); // END

	while(1) {
	}
}

void readString(char* string) {
	int counter;
	char x;
	counter = 0;

	while(1) {
		x = interrupt(0x16, 0, 0, 0, 0);

		// HANDLE BACKSPACE
		if(x == 0x8) {
			if(counter == 0) {
			//IGNORE BACKSPACE
				counter = 0;
			}
			else {
				counter = counter - 1;
				printChar('\b');
				printChar(' ');
				printChar('\b');
			}
		}

		// HANDLE RETURN
		else if(x == 0xd) {
			string[counter] = 0xa;
			printChar(string[counter]);
			counter = counter + 1;
			string[counter] = 0x0;
			printChar(string[counter]);
			return;
		}

		// HANDLE EVERYTHING ELSE
		else {
			string[counter] = x;
			counter = counter + 1;
			printChar(x);
		}
	}
}

void printChar(char* letter) {

	int ah = 0xe;
	int al = letter;
	int ax = ah * 0x100 + al;
	interrupt(0x10, ax, 0, 0, 0);
}

void printString(char* string) {

	int c = 0;
	int ah = 0xe;

	while(string[c] != 0x0) {
		int al = string[c];
		int ax = ah * 0x100 + al;
		interrupt(0x10, ax, 0, 0, 0);
		c = c + 1;
	}
}

void readSector(char* buffer, int sector) {

	int ah = 2;
	int al = 1;
	int bx = buffer;
	int ch = 0;
	int cl = sector + 1;
	int dh = 0;
	int dl = 0x80;

	int ax = ah * 256 + al;
	int cx = ch * 256 + cl;
	int dx = dh * 256 + dl;

	interrupt(0x13, ax, bx, cx, dx);
}

void writeSector(char* buffer, int sector) {

	int ah = 3;
	int al = 1;
	int bx = buffer;
	int ch = 0;
	int cl = sector + 1;
	int dh = 0;
	int dl = 0x80;

	int ax = ah * 256 + al;
	int cx = ch * 256 + cl;
	int dx = dh * 256 + dl;

	interrupt(0x13, ax, bx, cx, dx);
}

void deleteFile(char* filename) {
	char dir[512];
	char map[512];
	int entry;
	int markedSector;
	int i;

	readSector(dir, 2);
	readSector(map, 1);

	for(entry = 0; entry < 512; entry = entry + 32) {
		if(dir[entry] == filename[0] && dir[entry + 1] == filename[1] && dir[entry + 2] == filename[2]
			&& dir[entry + 3] == filename[3] && dir[entry + 4] == filename[4] &&
				 dir[entry + 5] == filename[5]) {

			// SET FIRST BYTE OF ENTRY TO 00
			dir[entry] = '\0';
			for(i = 6; i < 32; i = i + 1) {
				// CLEAR MAP
				markedSector = dir[i];
				map[markedSector] = '\0';
				// CLEAR DIRECTORY SECTORS... NOT SURE IF NECESSARY
			//	dir[entry + i] = '\0';
			}
		}
	}

	// WRITE BACK TO DIRECTORY AND MAP
	writeSector(dir, 2);
	writeSector(map, 1);
}

void writeFile(char* buffer, char* filename, int numberOfSectors) {
	char dir[512];
	char map[512];
	int entry;
	int sectorsWritten;
	int markedSector;
	int i;
	int m;
	int j;
	sectorsWritten = 0;

	readSector(dir, 2);
	readSector(map, 1);

	for(entry = 0; entry < 512; entry = entry + 32) {
		if(dir[entry] == '\0') {
			for(i = 0; i < 6; i = i + 1) {
				// CLAIM SPACE IN DIRECTORY
				dir[entry + i] = filename[i];
			}
			for(j = 0; j < numberOfSectors; j = j + 1) {
				for(m = 3; m < 512; m = m + 1) {
					// CLAIM SPACE IN MAP
					if(map[m] == '\0') {
						markedSector = m;
						map[m] = 0xFF;
						dir[entry + 6 + sectorsWritten] = m;
						writeSector(buffer, m);
						break;
					}
				}
			sectorsWritten = sectorsWritten + 1;
			}
			break;
		}
	}

	// WRITE BACK TO DIRECTORY AND MAP
	writeSector(dir, 2);
	writeSector(map, 1);
}

void killProcess(int id) {
	int dataseg;

	dataseg = setKernelDataSegment();
	processActive[id] = 0;
	restoreDataSegment(dataseg);
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {

	if(ax == 0) {
		printString(bx);
	}
	if(ax == 1) {
		readString(bx);
	}
	if(ax == 2) {
		readSector(bx, cx);
	}
	if(ax == 3) {
		readFile(bx, cx, dx);
	}
	if(ax == 4) {
		executeProgram(bx);
	}
	if(ax == 5) {
		terminate();
	}
	if(ax == 6) {
		writeSector(bx, cx);
	}
	if(ax == 7) {
		deleteFile(bx);
	}
	if(ax == 8) {
		writeFile(bx, cx, dx);
	}
	if(ax == 9) {
		killProcess(bx);
	}
}

void handleTimerInterrupt(int segment, int sp) {
	int i;
	int j;
	int dataseg;

//	printChar("T");
//	printChar("i");
//	printChar("c");

	dataseg = setKernelDataSegment();
	for(j = 0; j < 8; j = j + 1) {
		putInMemory(0xb800, 60*2 + j*4, j+0x30);
		if(processActive[j] == 1) {
			putInMemory(0xb800, 60*2 + j*4 + 1, 0x20);
		}
		else {
			putInMemory(0xb800, 60*2 + j*4 + 1, 0);
		}
	}
	if(currentProcess != -1) {
		processSP[currentProcess] = sp;
	}
	for(i = 0; i < 8; i = i + 1) {
		if(i == 8) {
			i = 0;
			currentProcess = 0;
			continue;
		}
		currentProcess = currentProcess + 1;
		if(processActive[currentProcess] == 1) {
			break;
		}
	}

	segment = (currentProcess + 2) * 0x1000;
	sp = processSP[currentProcess];
	restoreDataSegment(dataseg);

	returnFromTimer(segment, sp);
}

