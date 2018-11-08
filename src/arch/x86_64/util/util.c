

#include <TGR.h>

char *ascii     = "................................ !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~.................................¢£¤¥¦§¨©ª«¬®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ";
char *ascii_non = "................................ !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~.......................................................................................................................................................";

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

void dumpData(char *name, FileStruct file, int size, bool use_non, int start, int end) {
 uint8_t bytes[16];
 printf("._______._______________________________________________.________________.\n|%s",name);
 if       (strlen(name) < 2) {  printf("      "); }
  else if (strlen(name) < 3) {  printf("     "); }
  else if (strlen(name) < 4) {  printf("    "); }
  else if (strlen(name) < 5) {  printf("   "); }
  else if (strlen(name) < 6) {  printf("  "); }
  else if (strlen(name) < 7) {  printf(" "); }
 printf("|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F|0123456789ABCDEF|\n|-------|-----------------------------------------------|----------------|\n|0000000|");
 int j = 1,l=0;
 for (int i=start; i < end; i++) {
  if (i >= file.size) { break; }
  if (j > 15) {
   bytes[j-1] = file.data[i];
   if (file.data[i] < 0x10) { printf("0"); }
   printf("%x|",file.data[i]);
   for (int k=0; k < 16; k++) {
   if (use_non) { printf("%c",ascii_non[bytes[k]]); }
    else        { printf("%c",ascii[bytes[k]]); }
   } l=0;
   printf("|\n|");
   if      (i+1 < 0x1) {        printf("0000000%x|",i+1); }
   else if (i+1 < 0x10) {       printf("000000%x|",i+1); }
   else if (i+1 < 0x100) {      printf("00000%x|",i+1); }
   else if (i+1 < 0x1000) {     printf("0000%x|",i+1); }
   else if (i+1 < 0x10000) {    printf("000%x|",i+1); }
   else if (i+1 < 0x100000) {   printf("00%x|",i+1); }
   else if (i+1 < 0x1000000) {  printf("0%x|",i+1); }
   else if (i+1 < 0x10000000) { printf("%x|",i+1); }
   j = 0;
  } else {
   if (file.data[i] < 0x10) { printf("0"); } printf("%x ",file.data[i]);
   bytes[j-1] = file.data[i];
  } j++; l++;
 }
 if (j > 0) {
  for (int i=j; i < 16; i++) {
   printf("-- ");
   bytes[j-1] = 0x00;
  } printf("--|");
  for (int i=0; i < l-1; i++) {
   if (use_non) { printf("%c",ascii_non[bytes[i]]); }
    else        { printf("%c",ascii[bytes[i]]); }
  }
  for (int i=j; i < 16; i++) {
   printf(" ");
  } printf(" |\n");
 } printf("|_______|_______________________________________________|________________|\n\\Size: 0x%lx/%ld Bytes(",file.size,file.size);
 if (file.size < 1024) { printf("%ld KB)",file.size/1024); } else { printf("%ld MB)",file.size/1024/1024); }
 printf(" of 0x%x/%d bytes(",size,size);
 if (size < 1024) { printf("%d KB)\n\n",(size/1024)+1); } else { printf("%d MB)\n\n",(size/1024/1024)+1); }
}

void crop(char *dst, char *src, size_t mn, size_t mx) {
 const int len = mx - mn;
 src += mn;
 for (int i = 0; i < len; i++){
  dst[i] = src[i];
 }
 dst[len] = '\0';
}

FileStruct load(char *address, int make) {
 FileStruct file;
 file.found = true;
 if (access(address, F_OK) == -1) {
  if (make == 0) {
   printf("EMU ERROR: file(\"%s\") was not found...\n", address);
   file.found = false;
   return file;
  } else {
   printf("EMU WARNING: file \"%s\" was not found(your data could be lost if existsed), attempting to create local file...", address);
   FILE *SAV_file = fopen(address, "wb");
   fwrite(calloc(SAVSIZ, sizeof(*SAV)), SAVSIZ-1, 8, SAV_file);
   fclose(SAV_file);
//   file.fp = fopen(address, "wb+");
//   file.data = calloc(SAVSIZ, sizeof(*SAV));
//   fwrite(&file.data, SAVSIZ, 8, file.fp);
//   fclose(file.fp);
  }
 }
 file.fp = fopen(address, "rb");
 fseek(file.fp, 0, SEEK_END);
 file.size = ftell(file.fp);
 rewind(file.fp);
 file.data = malloc((file.size + 1) * sizeof(*file.data));
 fread(file.data, file.size, 1, file.fp);
 file.data[file.size] = '\0';
 fclose(file.fp);
 return file;
}
