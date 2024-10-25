#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_rng(char *buf, int *a, int *b, char *v) {
	char *ebuf = buf;
	int ta = 0, tb = 0;
	char tv = 0;

	tb = ta = strtol(buf, &ebuf, 16);
	if (ebuf == buf) return 0;
	buf = ebuf;
	if (*buf == '.') {
		++buf;
		if (*buf++ != '.') return 0;
		tb = strtol(buf, &ebuf, 16);
		if (ebuf == buf) return 0;
		buf = ebuf;
	}
	while (*buf == ' ' || *buf == '\t') { ++buf; }
	if (*buf != ';') return 0;
	++buf;
	while (*buf == ' ' || *buf == '\t') { ++buf; }
	switch (tv = *buf++) {
	case 'A': case 'F': case 'H': case 'W': break;
	case 'N':
		if (*buf == 'a') {
			tv = 'q';
			++buf;
		}
		break;
	default: return 0;
	}
	if (!(*buf == ' ' || *buf == '\t' || *buf == '\n' || *buf == 0))
		return 0;

	*a = ta, *b = tb, *v = tv;
	return 1;
}

int main(int argc, char **argv)
{
	char *set = calloc(0x110000,1);
	char table1[0x300];
	char buf[256], dummy;
	int a = 0, b = 0;
	char wprop = '_';
	FILE *f;

	if (argc<2) return 1;

	char *base_data_path = "/EastAsianWidth.txt";
	char *path = calloc(strlen(argv[1]) + 128, 1);
	strcpy(path, argv[1]);
	strcat(path, base_data_path);

	f = fopen(path, "rb");
	if (!f) return 1;
	while (fgets(buf, sizeof buf, f)) {
		if (parse_rng(buf, &a, &b, &wprop)) {
			if (wprop == 'W' || wprop == 'F') {
				for (; a<=b; a++) set[a]=1;
			}
		} else if (buf[0] == '\n') {}
		else if (buf[0] == '#') {}
		else {
			fprintf(stderr, "Fail: [%s]", buf);
			return 1;
		}
	}
	fclose(f);

	//for (a=0x4dc0; a<=0x4dff; a++) set[a]=1;

#define STEP 0x100
	int blocks_needed=0;
	for (a=0; a<0x20000; a+=STEP) {
		for (b=0; b<STEP; b++)
			if (set[a+b]!=set[a]) break;
		printf("%d\n", b!=STEP ? 18+blocks_needed++ : 16+!!set[a]);
	}
	for (a=0; a<32; a++) printf("0\n");
	for (a=0; a<32; a++) printf("255\n");
	for (a=0; a<0x20000; a+=STEP) {
		for (b=0; b<STEP; b++)
			if (set[a+b]!=set[a]) break;
		unsigned x=0;
		if (b!=STEP) for (b=0; b<STEP; b++) {
			x=x/2+128*!!set[a+b];
			if (!(b+1&7)) printf("%d\n", x&255);
		}
	}
	return 0;
}
