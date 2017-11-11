#include <stdio.h>
#include <stdlib.h>

int main()
{
	char *set = calloc(0x110000,1);
	char table1[0x300];
	char buf[128], dummy;
	int a, b;
	FILE *f;

	f = fopen("data/EastAsianWidth.txt", "rb");
	while (fgets(buf, sizeof buf, f)) {
		if (sscanf(buf, "%x..%x;%*[WF]%c", &a, &b, &dummy)==3)
			for (; a<=b; a++) set[a]=1;
		else if (sscanf(buf, "%x;%*[WF]%c", &a, &dummy)==2)
			set[a] = 1;
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
}
