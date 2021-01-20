#include <stdlib.h>
#include <stdio.h>

typedef char* const (*zmFunc)(int);

char* const zm(int a) {
	printf("zm: %d\n", a);
	return NULL;
}

int main(int argc, char** argv) {
	zmFunc a = zm;
	a(11);
	printf("Test pointer\n");
	return 0;
}
