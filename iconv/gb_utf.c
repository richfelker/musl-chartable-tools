#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int main()
{
	char buf[128];
	unsigned char *bmp = calloc(65536,1);
	unsigned e, c;
	int i, a, cum;

	memset(bmp, 1, 128);
	memset(bmp+0xd800, 1, 0x800);
	while (fgets(buf, sizeof buf, stdin)) {
		if (sscanf(buf, "%x %x", &c, &e)!=2) continue;
		assert(c<65536);
		bmp[c] = 1;
	}
	i = cum = 0;
	while (i<65536) {
		for (; i<65536 && bmp[i]; i++);
		if (i==65536) break;
		a = i;
		for (; i<65536 && !bmp[i]; i++);
		//printf("{ 0x%x, %d, %d },\n", a, cum, i-a);
		printf("{ 0x%x, %d },\n", a, i-a);
		cum += i-a;
	}
}
