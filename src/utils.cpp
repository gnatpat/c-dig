int getFileSize(FILE *fp) {
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  return size;
}

int len(char* s) {
  int len = 0;
  while(*s != '\0') {
    s++;
    len++;
  }
  return len;
}

void concat(char* s1, char* s2, char* out) {
  int l1 = len(s1);
  int l2 = len(s2);
  memcpy(out, s1, l1);
  memcpy(out + l1, s2, l2 + 1); // +1 for null terminator;
}
