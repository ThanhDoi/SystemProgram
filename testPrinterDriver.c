#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define STRING_LENGTH 256
static char receive[STRING_LENGTH]; // The receive buffer from the LKM

int printText(char stringToSend[]) {
	int fd, ret;
	fd = open("/dev/printerDevice", O_RDWR); // Open the device with read/write access
	if (fd < 0) {
		perror("Failed to open the device...");
		return errno;
	}

	ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM

	if (ret < 0) {
		perror("Failed to print1!");
		close(fd);
		return 0;
	} else {
		if (read(fd, receive, STRING_LENGTH) < 0) { // Read in a string (with spaces)
			perror("Failed to print2!");
			close(fd);
			return 0;
		}
	}

	close(fd);
	if (strcmp(receive, stringToSend) == 0) { // Check write result
		return 1;
	}
}

int main() {
	int ret, fd, x;
	char stringToSend[STRING_LENGTH], fileName[STRING_LENGTH], buff[STRING_LENGTH];
	FILE *f;

	printf("\nStarting device test code example...");

	while (1) {
		printf("\nChoose from menu:\n");
		printf("1. Print a file\n");
		printf("2. Print a text\n");
		printf("3. Exit\n");
		printf("Please choose: ");
		scanf("%d", &x);
		switch (x) {
			case 1:
			printf("Enter file name: ");
			scanf("%*c%[^\n]", fileName);
			f = fopen(fileName, "r");
			if (f == NULL) {
				printf("Cannot open file!\n");
				break;
			} else {
				printf("Printing...\n");
				while (!feof(f)) {
					fgets(buff, STRING_LENGTH, f);
					if (!feof(f)) {
						buff[strlen(buff)-1] = '\0';
					} else {
						buff[strlen(buff)] = '\0';
					}
					if (printText(buff) == 0) {
						perror("Failed to print!");
					}
				}
				printf("Printed!\n");
			}
			break;
			case 2:
			printf("Enter text: ");
			scanf("%*c%[^\n]", stringToSend);
			printf("Printing text...\n");
			if (printText(stringToSend) == 1)
				printf("Printed!\n");
			close(fd);
			break;
			case 3:
			return 0;
			default:
			printf("Choose again...\n");
		}

	}
}