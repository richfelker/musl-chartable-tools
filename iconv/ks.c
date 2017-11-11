#include <stdio.h>
#include <string.h>

int main()
{
	char buf[128];
	unsigned ks[94][94] = {0};
	unsigned e, c;
	int i, j;

	while (fgets(buf, sizeof buf, stdin)) {
		if (sscanf(buf, "%x %x", &e, &c)!=2) continue;
		if (e/256-0xA1 >= 93U || e%256-0xA1 >= 94U) continue;
		e -= 0xA1A1;
		ks[e>>8][e&255] = c;
	}
	for (i=0; i<93; i++) for (j=0; j<94; j++)
		printf("%d\n", ks[i][j]);
}
