#include <stdio.h>

int main(int argc, char **argv)
{
	unsigned x;
	int col=0, cnt=0, len;
	char buf[sizeof(int)*3+3];
	FILE *f = stdout;
	while (scanf("%i", &x)==1) {
		len = sprintf(buf, "%u,", x);
		if ((++cnt%256||1) && col < 79-len) {
			col += len;
			fputs(buf, f);
		} else {
			col = len;
			fputs("\n", f);
			fputs(buf, f);
		}
	}
	fputc('\n', f);
	return 0;
}
