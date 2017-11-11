#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static const unsigned short legacy_chars[] = {
#include "legacychars.h"
};

int main(int argc, char **argv)
{
	unsigned cp[128];
	char line[80];
	int b, c, i;
	FILE *f = stdout;
	int col=-1U/2, len;
	char buf[32];

	for (b=0; b<128; b++) cp[b] = 1;
	while (fgets(line, sizeof line, stdin)) {
		if (sscanf(line, "%i %i", &b, &c)!=2 || b<128) continue;
		if (c==b) c=0;
		for (i=0; i<sizeof legacy_chars / sizeof *legacy_chars
			&& legacy_chars[i] != c; i++)
		if (i == sizeof legacy_chars / sizeof *legacy_chars) {
			fprintf(stderr, "not found: %x\n", c);
			exit(1);
		}
		cp[b-128] = i;
	}
	for (b=0; b<128 && (!cp[b] || legacy_chars[cp[b]]==b+128); b++);
	b &= -4;
	for (argv++; *argv; argv++)
		fprintf(f, "\"%s\\0\"\n", *argv);
	fprintf(f, "\"\\0\\%o", b);
	// 00000000 00111111 11112222 22222233 33333333
	for (i=0; b<128; b+=4, i++) {
		len = sprintf(buf, "\\%o\\%o\\%o\\%o\\%o",
			cp[b]&255,
			cp[b]>>8 | cp[b+1]<<2&255,
			cp[b+1]>>6 | cp[b+2]<<4&255,
			cp[b+2]>>4 | cp[b+3]<<6&255,
			cp[b+3]>>2);
		if (col < 79-len) {
			col += len;
			fputs(buf, f);
		} else {
			col = len;
			fputs("\"\n\"", f);
			fputs(buf, f);
		}
	}
	fputs("\"\n\n", f);
}
