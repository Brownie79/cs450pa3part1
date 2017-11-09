#include "types.h"
#include "user.h"
#include "syscall.h"

int main(int argc, char *argv[]) {
	int *pointer = malloc(sizeof(int));
	printf(1,"Pointer address = %p\n", pointer);
	free(pointer);
	printf(1,"\n\n\n\n\n")
	printf(1, "malloc(int*64): \n");
	int *pointer2 = malloc(sizeof(int)*64);
	printf(1,"Pointer address = %p\n", pointer2);

	exit();
}