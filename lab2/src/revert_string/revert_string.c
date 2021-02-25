#include "stdio.h"
#include "revert_string.h"

void RevertString(char *str)
{
	int cnt = 0;
	for (unsigned long i = 0; str[i] != '\0'; i++)	
	{
		cnt++;
	}
	for (unsigned long i = 0; i < cnt/2; i++)
	{
		char tmp = str[i];
		str[i] = str[cnt - 1 - i];
		str[cnt - 1 - i] = tmp;
	}
}

void __attribute__ ((constructor)) initLibrary(void) {
 //
 // Function that is called when the library is loaded
 //
    printf("Library is initialized\n");
}
void __attribute__ ((destructor)) cleanUpLibrary(void) {
 //
 // Function that is called when the library is »closed«.
 //
    printf("Library is exited\n"); 
}
