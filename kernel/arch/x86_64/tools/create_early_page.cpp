#include <stdio.h>

#define KERNEL_LMA  0x0000000001000000
#define KERNEL_VMA  0xffffffff81000000
#define ABSOLUTE(x) ((x) - KERNEL_VMA + KERNEL_LMA)

FILE *fp = NULL;
char buffer[128];

void psymbol(bool end, int p4, int p3 = -1, int p2 = -1, int p1 = -1) {
	if (end) {
		fprintf(fp, "end_pml_%d", p4);
	} else {
		fprintf(fp, "pml_%d", p4);
	}
	if (p3 != -1) {
		fprintf(fp, "_%d", p3);
	}
	if (p2 != -1) {
		fprintf(fp, "_%d", p2);
	}
	if (p1 != -1) {
		fprintf(fp, "_%d", p1);
	}
}

void PML(int p4, int p3 = -1, int p2 = -1, int p1 = -1) {
	psymbol(false, p4, p3, p2, p1);
	fprintf(fp, ":\n");
}

void END_PML(int p4, int p3 = -1, int p2 = -1, int p1 = -1) {
	psymbol(true, p4, p3, p2, p1);
	fprintf(fp, ":\n");
}

void PML_ENTRY(int p4, int p3 = -1, int p2 = -1, int p1 = -1, int i = -1) {
	if (i == -1) {
		fprintf(fp, ".quad ABSOLUTE(");
		psymbol(false, p4, p3, p2, p1);
		fprintf(fp, ") + 7\n");
	} else {
		unsigned long long a, i1, i2, i3, i4;
		i1 = p1;
		i2 = p2;
		i3 = p3;
		i4 = p4;
		if (p3 == 255) {
			a = (i3 << 39) + (i2 << 30) + (i1 << 21) + (i << 12) + 7;
		} else {
			a = ((i2 & 1) << 30) + (i1 << 21) + (i << 12) + 7;
		}
		fprintf(fp, ".quad 0x%x%08x\n",
		        (unsigned int)(a >> 32), (unsigned int)(a & 0xffffffff));
	}
}

void SKIP(int a) {
	fprintf(fp, ".skip %d * 8\n", a);
}


void P(int p4, int p3, int p2, int p1) {
	unsigned long long a, i1, i2, i3, i4;
	i1 = p1;
	i2 = p2;
	i3 = p3;
	i4 = p4;
	if (p3 == 255) {
		a = (i3 << 39) + (i2 << 30) + (i1 << 21) + 7;
	} else {
		a = ((i2 & 1) << 30) + (i1 << 21) + 7;
	}
	PML(p4, p3, p2, p1);
	fprintf(fp, "i = 0\n");
	fprintf(fp, ".rept 512\n");
	fprintf(fp, "	.quad 0x%llx + (i << 12)\n", a);
	fprintf(fp, "	i = i + 1\n");
	fprintf(fp, ".endr\n");
	END_PML(p4, p3, p2, p1);
}

int main(void) {
	fp = fopen("early_page.S", "w");
	fprintf(fp, "#define KERNEL_LMA  0x0000000001000000\n");
	fprintf(fp, "#define KERNEL_VMA  0xffffffff81000000\n");
	fprintf(fp, "#define ABSOLUTE(x) ((x) - KERNEL_VMA + KERNEL_LMA)\n");
	PML(0);
	PML_ENTRY(0, 0);
	SKIP(510);
	PML_ENTRY(0, 511);
	END_PML(0);
	PML(0, 0);
	PML_ENTRY(0, 0, 0);
	SKIP(511);
	END_PML(0, 0);
	PML(0, 0, 0);
	PML_ENTRY(0, 0, 0, 0);
	PML_ENTRY(0, 0, 0, 1);
	SKIP(510);
	END_PML(0, 0, 0);
	PML(0, 511);
	SKIP(510);
	PML_ENTRY(0, 511, 510);
	PML_ENTRY(0, 511, 511);
	END_PML(0, 511);
	int p2 = 510;
	while (p2 <= 511) {
		PML(0, 511, p2);
		for (int i = 0; i < 512; i++) {
			PML_ENTRY(0, 511, p2, i);
		}
		END_PML(0, 511, p2);
		p2++;
	}
	P(0, 0, 0, 0);
	P(0, 0, 0, 1);
	for (int i = 0; i < 512; i++) {
		P(0, 511, 510, i);
	}
	for (int i = 0; i < 512; i++) {
		P(0, 511, 511, i);
	}
	fclose(fp);
}
