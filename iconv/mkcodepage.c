#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static const unsigned short legacy_chars[] = {
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,
29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,
55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,
105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,
124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,
143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,
162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,
181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,
200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,
219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,
238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
#include "legacychars.h"
};

int main(int argc, char **argv)
{
	/* Initialize table with zeros. For byte value 0 this is valid
	 * and is self-mapping. For any other byte value, 0 indicates
	 * that it's invalid in the codepage. */
	unsigned cp[256] = {0};
	char line[80];
	int b, c, i;
	FILE *f = stdout;
	int col=-1U/2, len;
	char buf[32];

	while (fgets(line, sizeof line, stdin)) {
		if (sscanf(line, "%i %i", &b, &c)!=2) continue;
		for (i=0; i<sizeof legacy_chars / sizeof *legacy_chars
			&& legacy_chars[i] != c; i++)
		if (i == sizeof legacy_chars / sizeof *legacy_chars) {
			fprintf(stderr, "not found: %x\n", c);
			exit(1);
		}
		cp[b] = i;
	}
	for (b=0; b<256 && (legacy_chars[cp[b]]==b); b++);
	b &= -4;
	for (argv++; *argv; argv++)
		fprintf(f, "\"%s\\0\"\n", *argv);
	fprintf(f, "\"\\0\\%o", b/4);
	// 00000000 00111111 11112222 22222233 33333333
	for (i=0; b<256; b+=4, i++) {
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
