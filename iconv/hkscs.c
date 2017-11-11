#include <stdio.h>
#include <string.h>

int main()
{
	char buf[128];
	unsigned b5[256][256] = {0};
	unsigned short highbits[(31*157+15)/16] = {0};
	unsigned e, c;
	int i, j;
	int h, k;

	while (fgets(buf, sizeof buf, stdin)) {
		if (sscanf(buf, "%x %x", &e, &c)!=2) continue;
		b5[e>>8][e&255] = c;
	}
	for (h=k=i=0; i<0xFF; i++) {
		if (i<0x87 || (i>=0xA1 && i<=0xF9)) continue;
		for (j=0x40; j<0xFF; j++) {
			if (j-0x7FU < 0xA1-0x7F) continue;
			printf("%d\n", b5[i][j] % 65536);
			if (b5[i][j] >= 65536)
				highbits[h] |= 1<<k;
			if (k<15) k++;
			else k=0, h++;
		}
	}
	for (i=0; i<(sizeof highbits)/2; i++)
		printf("%d\n", highbits[i]);
}
