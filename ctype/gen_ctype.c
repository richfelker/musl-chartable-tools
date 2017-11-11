#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char *set = calloc(0x110000,1);
	char table1[0x300];
	char buf[128], dummy;
	int a, b;
	FILE *f;
	int wanted_set;

	if (argc<2) return 1;
	if (argv[1][0]=='a') wanted_set = 1;
	else if (argv[1][0]=='p') wanted_set = 2;
	else return 1;

	f = fopen("data/UnicodeData.txt", "rb");
	while (fgets(buf, sizeof buf, f)) {
		if (sscanf(buf, "%x;%*[^;];Nd%c", &a, &dummy)==2)
			set[a] = 1;
		else if (sscanf(buf, "%x;%*[^;];%c", &a, &dummy)==2)
			set[a] = 2;
	}
	fclose(f);

	f = fopen("data/DerivedCoreProperties.txt", "rb");
	while (fgets(buf, sizeof buf, f)) {
		if (sscanf(buf, "%x..%x ; Alphabetic%c", &a, &b, &dummy)==3)
			for (; a<=b; a++) set[a]=1;
		else if (sscanf(buf, "%x ; Alphabetic%c", &a, &dummy)==2)
			set[a] = 1;
	}
	fclose(f);

	/* Fix misclassified Thai characters */
	set[0xe2f] = set[0xe46] = 2;

	/* Clear digits */
	for (a=0x30; a<=0x39; a++) set[a]=0;

	/* Clear spaces */
	set[0x0020] = 0;
	for (a=0x2000; a<=0x2006; a++) set[a]=0;
	for (a=0x2008; a<=0x200a; a++) set[a]=0;
	set[0x205f] = 0;
	set[0x3000] = 0;

	/* Clear controls */
	for (a=0x00; a<=0x1f; a++) set[a]=0;
	for (a=0x7f; a<=0x9f; a++) set[a]=0;
	for (a=0x2028; a<=0x2029; a++) set[a]=0;
	for (a=0xfff9; a<=0xfffb; a++) set[a]=0;
	for (a=0xd800; a<=0xdfff; a++) set[a]=0;

	/* Fill in elided CJK ranges */
	for (a=0x3400; a<=0x4db5; a++) set[a]=1;
	for (a=0x4e00; a<=0x9fcc; a++) set[a]=1;
	for (a=0xac00; a<=0xd7a3; a++) set[a]=1;
	for (a=0x20000; a<=0x2a6d6; a++) set[a]=1;
	for (a=0x2a700; a<=0x2b734; a++) set[a]=1;
	for (a=0x2b740; a<=0x2b81d; a++) set[a]=1;

	for (a=0; a<=0x10ffff; a++) if (set[a]!=wanted_set) set[a]=0;

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
}
