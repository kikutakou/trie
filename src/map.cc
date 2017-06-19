#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;


#define BUF_SIZE 2048000


int main(int argc, char** argv)
{
  
  map<>;
  
  //time
  char buf[BUF_SIZE];
  FILE* fd;
  struct rusage r;
  timeval start, end;
  double sec;
  
  //check arg
  if(argc < 2){
    fprintf(stderr, "usage  : %s [file]\n", argv[0]);
  }
  
  //file open
  if((fd = fopen(argv[1], "r")) == NULL){
    fprintf(stderr, "Error : can't open file %s\n", argv[1]);
    exit(1);
  }
  
  
  getrusage(RUSAGE_SELF, &r);
  start = r.ru_utime;
  
  //construct
  while( fgets(buf,BUF_SIZE,fd) != NULL ){
    char *begin, *end;
    for(begin = end = buf; (*end) != '\0'; end++){
      if((*end) == ' ' || (*end) == '\n' || (*end) == '\t'){
        if(end != begin){
          (*end) = '\0';
          //printf("%s\n", begin);
          trie.insert(begin);
        }
        begin = end + 1;
      }
    }
  }
  
  getrusage(RUSAGE_SELF, &r);
  end = r.ru_utime;
  
  sec = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000;
  printf("construct %.2f sec\n", sec);
  
  //read file again
  fseek(fd,  0, SEEK_SET);
  
  getrusage(RUSAGE_SELF, &r);
  start = r.ru_utime;
  
  //search
  while( fgets(buf,BUF_SIZE,fd) != NULL ){
    char *begin, *end;
    for(begin = end = buf; (*end) != '\0'; end++){
      if((*end) == ' ' || (*end) == '\n' || (*end) == '\t'){
        if(end != begin){
          (*end) = '\0';
          //printf("%s\n", begin);
          if(trie.search(begin) == false){
            printf("%s not found\n", begin);
          }
        }
        begin = end + 1;
      }
    }
  }
  
  getrusage(RUSAGE_SELF, &r);
  end = r.ru_utime;
  
  sec = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000;
  printf("search %.2f sec\n", sec);
  printf("maxrss = %ld MB\n", r.ru_maxrss / 1024 / 1024 );
  
  
  fclose(fd);
  
  
  
  
  return 0;

}

