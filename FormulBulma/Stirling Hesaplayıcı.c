#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>






#define CSV_FILE "table.csv"
#define INDEX_FILE "index.bin"



typedef struct {
    unsigned int a;
    unsigned int b;
    long offset;
} IndexRecord;

unsigned int getValue(unsigned int a, unsigned int b, FILE *csvFile, FILE *idxFile);
unsigned int calculateTheValue(unsigned int a, unsigned int b, FILE *csvFile, FILE *idxFile);


// Hesaplama fonksiyonu

unsigned int calculateTheValue(unsigned int a, unsigned int b, FILE *csvFile, FILE *idxFile) {
    if (b == 0 || a == 0) return 0;
    else if (b > a) return 0;
    else if (b == a) return 1;
    else {
        unsigned int left  = getValue(a-1, b-1, csvFile, idxFile);
        unsigned int right = getValue(a-1, b,   csvFile, idxFile);

        // Overflow kontrolü (b * right)
        if (b != 0 && right > UINT_MAX / b) {
            fprintf(stderr, "ERROR: Overflow at (%u,%u) -> %u * %u exceeds UINT_MAX\n", a, b, b, right);
            exit(EXIT_FAILURE);
        }
        unsigned int mult = b * right;

        // Overflow kontrolü (left + mult)
        if (left > UINT_MAX - mult) {
            fprintf(stderr, "ERROR: Overflow at (%u,%u) -> %u + %u exceeds UINT_MAX\n", a, b, left, mult);
            exit(EXIT_FAILURE);
        }

        return left + mult;
    }
}


unsigned int getValue(unsigned int a, unsigned int b, FILE *csvFile, FILE *idxFile) {
	if (b == 0 || a == 0) return 0;//tekrara düþüldü ama csv küçültüm.
    else if (b > a) return 0;
    else if (b == a) return 1;



    IndexRecord rec;
    int found = 0;
    long csvOffset = 0;
    unsigned int result = 0;

    rewind(idxFile);
    while (fread(&rec, sizeof(IndexRecord), 1, idxFile) == 1) {
        if (rec.a == a && rec.b == b) {
            csvOffset = rec.offset;
            found = 1;
            break;
        }
    }

    if (found) {
        fseek(csvFile, csvOffset, SEEK_SET);
        char line[128];
        if (fgets(line, sizeof(line), csvFile)) {
            sscanf(line, "%*u,%*u,%u", &result);
        }
    } else {
        result = calculateTheValue(a, b, csvFile, idxFile);

        fseek(csvFile, 0, SEEK_END);
        csvOffset = ftell(csvFile);
        fprintf(csvFile, "%u,%u,%u\n", a, b, result);

        rec.a = a;
        rec.b = b;
        rec.offset = csvOffset;
        fseek(idxFile, 0, SEEK_END);
        fwrite(&rec, sizeof(IndexRecord), 1, idxFile);
    }

    return result;
}





int main() {
	while (1){

	    unsigned int a, b;
	    printf("a ve b girin: ");
	    scanf("%u %u", &a, &b);
	    
	    FILE *csvFile = fopen(CSV_FILE, "a+");
	    FILE *idxFile = fopen(INDEX_FILE, "a+b");
	    fseek(csvFile, 0, SEEK_END);
	    if (ftell(csvFile) == 0) {
	        fprintf(csvFile, "a,b,result\n");
	    }
	    
	    
	    
	    
	
	    unsigned int val = getValue(a, b, csvFile, idxFile);
	    printf("Sonuc: %u\n", val);
	
	    fclose(csvFile);
	    fclose(idxFile);
	
	}

    return 0;
}



