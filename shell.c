//JAMES EVANS
//CONNOR WALSH
//SHELL

void main() {
	char input[80];
	char buffer[13312];
	char filename[6];
	char secondFilename[6];
	char dir[512];
	char line[80];
	int entry;
	int i;
	int j;

	enableInterrupts();
	while(1) {
		syscall(0, "\r\nSHELL> ");
		syscall(1, input);
		syscall(0, "\r\n");

		filename[0] = input[5];
		filename[1] = input[6];
		filename[2] = input[7];
		filename[3] = input[8];
		filename[4] = input[9];
		filename[5] = input[10];

		secondFilename[0] = input[12];
		secondFilename[1] = input[13];
		secondFilename[2] = input[14];
		secondFilename[3] = input[15];
		secondFilename[4] = input[16];
		secondFilename[5] = input[17];

		if(input[0] == 't' && input[1] == 'y' && input[2] == 'p' && input[3] == 'e') {
			// 'TYPE' COMMAND
			syscall(3, filename, buffer, 0);
			syscall(0, buffer);
		}
		else if(input[0] == 'e' && input[1] == 'x' && input[2] == 'e' && input[3] == 'c') {
			// 'EXEC' COMMAND
			syscall(3, filename, buffer, 0);
			syscall(4, filename);
		}
		else if(input[0] == 'd' && input[1] == 'i' && input[2] == 'r' && input[3] == 'c') {
			// 'DIRC' COMMAND
			syscall(2, dir, 2);

			for(entry = 0; entry < 512; entry = entry + 32) {
				char dirEntry[6];

				for(j = 0; j < 6; j = j + 1) {
					dirEntry[j] = dir[entry + j];
				}
				if(dir[entry] == '\0') {
					// SKIP ENTRY IF DELETED/FREE
					entry = entry + 0;
				}
				else {
					// PRINT ENTRY
					syscall(0, dirEntry);
					syscall(0, "\r\n");
				}
			}
		}
		else if(input[0] == 'd' && input[1] == 'e' && input[2] == 'l' && input[3] == 'e') {
			// 'DELE' COMMAND
			syscall(7, filename);
		}
		else if(input[0] == 'c' && input[1] == 'o' && input[2] == 'p' && input[3] == 'y') {
			// 'COPY' COMMAND
			syscall(3, filename, buffer, 0);
			syscall(8, buffer, secondFilename, 1);
		}
		else if(input[0] == 'c' && input[1] == 'r' && input[2] == 'a' && input[3] == 't') {
			// 'CRAT' COMMAND... CREATE TEXT FILE
			syscall(1, line);
			syscall(8, line, filename, 1);
		}
		else if(input[0] == 'k' && input[1] == 'i' && input[2] == 'l' && input[3] == 'l') {
			// 'KILL' COMMAND
			syscall(9, filename[0]);
		}
		else {
			syscall(0, "Command not recognized...\r\n");
		}
	}
}


