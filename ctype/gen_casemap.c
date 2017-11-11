#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

int main()
{
	int *map = calloc(0x110000,sizeof *map);
	char buf[256], dummy;
	int a, b;
	FILE *f;
	char *p;

	f = fopen("UnicodeData.txt", "rb");
	while (fgets(buf, sizeof buf, f)) {
		if (sscanf(buf, "%x;%*[^;];%*1[LNM]%*[^;];%c", &a, &dummy)==2) {
			p=strrchr(buf, ';');
			for (p--; *p!=';'; p--);
//			for (p--; *p!=';'; p--);
			if (sscanf(p+1, "%x", &b)!=1) continue;
			//printf("%.4x -> %.4x\n", a, b);
			map[a] = b;
		}
	}
	fclose(f);

	for (a=0; a<0x20000; a++) {
//		b = towupper(a);
		b = towlower(a);
		if (map[a] && b != map[a])
			printf("%.4x -> %.4x\n", a, map[a]);
		else if (!map[a] && b != a)
			printf("bad map %.4x -> %.4x\n", a, b);
	}


return 0;

#define STEP 0x80
	for (a=0; a<0x20000; a+=STEP) {
		int offset = 0;
		for (b=0; b<STEP; b++) {
			if (map[a+b]<=0) continue;
			if (!offset) offset = map[a+b]-(a+b);
			if (map[a+b]-(a+b) != offset) break;
		}
		if (!offset) printf("block %.4x: no case mappings here\n", a);
		else if (b==STEP) printf("block %.4x: offset %d works\n", a, offset);
		else {
			printf("block %.4x: no common offset\n", a);
			for (b=0; b<STEP; b++) {
				if (map[a+b]<0) printf("\t--");
				else printf("\t%d", map[a+b]-(a+b));
				if (!(b+1&7)) putchar('\n');
			}
		}
	}
}
