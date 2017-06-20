#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

using namespace std;

//hash function
typedef unsigned long ulong;
const unsigned int HASH_SIZE = 24;
const unsigned long HASH_SPACE = (unsigned long)1 << 24;

const int debug = 0;

ulong str_hash(const char *str){
	ulong h = 0;
  for (int i = 0; str[i] != '\0' ; i++){
    unsigned char uc = (unsigned char)str[i];
//    if(debug) printf("%s[%d] = %x (%d)\n", str, i, uc, uc);
    int shift = i % sizeof(ulong) * 8;
    h += ((ulong)uc << shift);
  }
	return h % HASH_SPACE;
}


typedef struct data{
  string str;
  int value;
} data;


typedef vector<data> Node;

class HashTable{
	Node* table;
  
public:
  
  HashTable(){
    table = new Node[HASH_SPACE];
  }
	
	void insert(const char *key){
		ulong h = str_hash(key);
		if(debug) printf("key = %s, hash = %lu\n", key, h);
    Node &node = table[h];
    
    for (int i = 0; i < node.size(); i++) {
      if(strcmp(node[i].str.c_str(), key) == 0) return;
    }
    node.push_back((struct data){string(key), 1});
	}
	
	int search(const char* key){
		ulong h = str_hash(key);
    Node &node = table[h];
    for (int i = 0; i < node.size(); i++) {
      if(strcmp(node[i].str.c_str(), key) == 0) return node[i].value;
    }
    if(debug) printf("key = %s, hash = %lu\n", key, h);

    return false;
	}
  
  void print(){
    for (int i = 0; i < HASH_SPACE; i++) {
      Node& node = table[i];
      for (int j = 0; j < node.size(); j++) {
        printf("table[%d][%d] = %s\n", i, j, node[j].str.c_str());
      }
    }
  }
  
  void print_status(){
    ulong sum = 0;
    ulong empty_count = 0;
    for (int i = 0; i < HASH_SPACE; i++) {
      int s = table[i].size();
      if(s == 0) empty_count++;
      else sum += s;
    }
    printf("num words = %lu\n", sum);
    printf("num empty nodes = %lu / %lu\n", empty_count, HASH_SPACE);
    printf("average node len = %lf\n", (double)sum / (HASH_SPACE - empty_count));
  }
  
  void print_config(){
    printf("HASH_SIZE = %d\n", HASH_SIZE);
    printf("HASH_SPACE = %lu\n", HASH_SPACE);
  }
	
};

#define BUF_SIZE 2048000


int main(int argc, char** argv){
//
//  printf("の = %lu\n", str_hash("の"));
//  printf("ぎ = %lu\n", str_hash("ぎ"));
//  printf("のこぎり = %lu\n", str_hash("のこぎり"));
//  exit(1);
  
	HashTable hashtable;
  
  printf("start\n");

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
          hashtable.insert(begin);
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
          if(!hashtable.search(begin)){
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
  
  
  //hashtable.print();
  hashtable.print_status();
  fclose(fd);
  
  
  
  
  return 0;

}
