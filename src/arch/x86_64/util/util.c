

#include <TGR.h>


char* concat(const char *s1, const char *s2) {
 const size_t len1 = strlen(s1);
 const size_t len2 = strlen(s2);
 char *result = malloc(len1+len2+1);//+1 for the null-terminator
 //in real code you would check for errors in malloc here
 memcpy(result, s1, len1);
 memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
 return result;
}

void bin_dump(uint64_t u) {
 int i = 63;
 while (i --> 0) {
  printf("%hhu", (u & (uint64_t) pow(2, i)) ? 1 : 0);
 }
 printf("\n");
}

int dumpData(char *name, FileStruct file, int size) {
 printf(".______________________________________________________.\n|%s",name);
 if        (strlen(name) < 2) {  printf("     ");
 } else if (strlen(name) < 3) {  printf("    ");
 } else if (strlen(name) < 4) {  printf("   ");
 } else if (strlen(name) < 5) {  printf("  ");
 } else if (strlen(name) < 6) {  printf(" ");
 } printf("|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F|\n|------|-----------------------------------------------|\n|000000|");
 int j = 1;
 for (int i=0; i < file.size; i++) {
  if (j > 15) {
   if (file.data[i] < 0x10) { printf("0"); } printf("%x|\n|",file.data[i]);
   if       (i+1 < 0x1) {       printf("000000%x|",i+1); }
    else if (i+1 < 0x10) {      printf("00000%x|",i+1); }
    else if (i+1 < 0x100) {     printf("0000%x|",i+1); }
    else if (i+1 < 0x1000) {    printf("000%x|",i+1); }
    else if (i+1 < 0x10000) {   printf("00%x|",i+1); }
    else if (i+1 < 0x100000) {  printf("0%x|",i+1); }
    else if (i+1 < 0x1000000) { printf("%x|",i+1); }
   j = 0;
  } else {
   if (file.data[i] < 0x10) { printf("0"); } printf("%x ",file.data[i]);
  } j++;
 }
 if (j > 0) {
  for (int i=j; i < 16; i++) {
   printf("-- ");
  } printf("--|\n");
 } printf("|______|_______________________________________________|\n\\Size: 0x%x/%d Bytes(",file.size,file.size);
 if (file.size < 1024) { printf("%d KB)",file.size/1024); } else { printf("%d MB)",file.size/1024/1024); }
 printf(" of 0x%x/%d bytes(",size,size);
 if (size < 1024) { printf("%d KB)\n\n",(size/1024)+1); } else { printf("%d MB)\n\n",(size/1024/1024)+1); }
 return 0;
}

void crop(char *dst, char *src, size_t mn, size_t mx) {
 const int len = mx - mn;
 src += mn;
 for (int i = 0; i < len; i++) dst[i] = src[i];
}

FileStruct load(char *adress, int make) {
 FileStruct file;
 file.found = true;
 if (access(adress, F_OK) == -1) {
  printf("EMU ERROR: file(\"%s\") was not found...\n", adress);
  if (make == 0) {
   file.found = false;
   return file;
  } else {
   printf("making file \"%s\"", adress);
   file.fp = fopen(adress, "wb");
   file.data = calloc(SAVSIZ, SAVSIZ);
   fwrite(file.data, SAVSIZ, 1, file.fp);
  }
 }
 file.fp = fopen(adress, "rb");
 fseek(file.fp, 0, SEEK_END);
 file.size = ftell(file.fp);
 rewind(file.fp);
 file.data = malloc((file.size + 1) * sizeof(*file.data));
 fread(file.data, file.size, 1, file.fp);
 file.data[file.size] = '\0';
 return file;
}