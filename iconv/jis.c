#include <stdio.h>
#include <string.h>

int main()
{
	char buf[128];
	unsigned jis[94][94];
	unsigned e, c;
	int i, j;

	memset(jis, 0, sizeof jis);
	while (fgets(buf, sizeof buf, stdin)) {
		if (sscanf(buf, "%*x %x %x", &e, &c)!=2) continue;
		e -= 0x2121;
		jis[e>>8][e&255] = c;
	}
	for (i=0; i<84; i++) for (j=0; j<94; j++)
		printf("%d\n", jis[i][j]);
}
