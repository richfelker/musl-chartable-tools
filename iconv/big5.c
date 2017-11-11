#include <stdio.h>
#include <string.h>

int main()
{
	char buf[128];
	unsigned b5[256][256] = {0};
	unsigned e, c;
	int i, j;

	while (fgets(buf, sizeof buf, stdin)) {
		if (sscanf(buf, "%x %x", &e, &c)!=2) continue;
		b5[e>>8][e&255] = c;
	}
	for (i=0xA1; i<0xFA; i++) for (j=0x40; j<0xFF; j++)
		if (j-0x7FU >= 0xA1-0x7F) printf("%d\n", b5[i][j] % 65536);
}
