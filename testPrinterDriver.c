#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 256
static char receive[BUFFER_LENGTH];

int printText(char stringToSend[]) {
	int fd, ret;
	fd = open("/dev/printerDevice", O_RDWR);
	if (fd < 0) {
		perror("Failed to open the device...");
		return errno;
	}

	ret = write(fd, stringToSend, strlen(stringToSend));

	if (ret < 0) {
		perror("Failed to print1!");
		close(fd);
		return 0;
	} else {
		if (read(fd, receive, BUFFER_LENGTH) < 0) {
			perror("Failed to print2!");
			close(fd);
			return 0;
		}
	}

	close(fd);
	if (strcmp(receive, stringToSend) == 0) {
		return 1;
	}
}

int main() {
	int ret, fd, x;
	char stringToSend[BUFFER_LENGTH], fileName[256], buff[BUFFER_LENGTH];
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
					fgets(buff, BUFFER_LENGTH, f);
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