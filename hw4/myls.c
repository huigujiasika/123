#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#define DIRSIZ 40

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;
  
  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  
  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}


void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  DIR *dp;
  struct dirent *de;
  struct stat st;
  
  if ((dp = opendir(path)) == NULL && (fopen(path, "r") == NULL)) {
    fprintf(stderr, "ls: cannot open %s\n", path);
    return;
  }
  
  if (stat(path, &st) < 0) {
    fprintf(stderr, "ls: cannot stat %s\n", path);
    closedir(dp);
    return;
  }
  
  switch (st.st_mode & __S_IFMT) {
  case __S_IFREG:
    printf("%s %d %d %d\n", fmtname(path), st.st_mode, (int)st.st_ino, (int)st.st_size);
    break;
  
  case __S_IFDIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      fprintf(stdout, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while ((de = readdir(dp)) != NULL) {
        if (de->d_ino == 0)
            continue;
        strncpy(p, de->d_name, DIRSIZ);
        p[DIRSIZ] = '\0';
        if (stat(buf, &st) < 0) {
            fprintf(stdout, "ls: cannot stat %s\n", buf);
            continue;
        }
        printf("%s %d %d %d\n", fmtname(buf), st.st_mode, (int)st.st_ino, (int)st.st_size);
    }
    break;
  }
  closedir(dp);
}

int
main(int argc, char *argv[])
{
  if(argc < 2){
    ls(".");
  } else {
    for(int i=1; i<argc; i++)
      ls(argv[i]);
  }
}
