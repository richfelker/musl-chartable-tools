#include <stdio.h>
#include <string.h>

int main()
{
	char buf[128];
	unsigned gb[126][191];
	unsigned e, c;
	int i, j;

	memset(gb, 0, sizeof gb);
	while (fgets(buf, sizeof buf, stdin)) {
		if (sscanf(buf, "%x %x", &c, &e)!=2) continue;
		e -= 0x8140;
		gb[e>>8][e&255] = c;
	}
	for (i=0; i<126; i++) for (j=0; j<191; j++)
		if (j!=63) printf("%d\n", gb[i][j]);
}
