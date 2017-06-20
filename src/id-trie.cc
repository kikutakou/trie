#include <string>
#include <vector>
#include <climits>
#include <assert.h>
#include <stdio.h>
#include <time.h>




/*
 * this trie manages each node by id
 * each node composes the list of (next id | has terminal) pairs
 */


using namespace std;

//debug print
void print_bit(char c){
  for(int i = sizeof(c) * 8 -1 ; i >= 0 ; i--){
    printf("%d", c >> i & 1);
  }
  printf("\n");
}

const int debug = 0;



typedef unsigned long ulong;

class Child{    //next node id and flag if terminated
  
  ulong _id;
  int _value;

  public:
  
  int is_term(){
    return _value > 0;
  }
  
  void set_term(){
    _value = 1;
  }
  
  ulong id(){
    return _id;
  }
  
  void set_id(int id){
    _id = id;
  }
  
};


const int N_GRAN = 4;       //number of byte divided, this should be 1, 2, 4 or 8

const int GRAN_BITS = 8 / N_GRAN;
const char MASK = (char)(1 << GRAN_BITS) - 1;

const int NODE_SIZE = 1 << GRAN_BITS;




#define MEM_CLUSTER_SIZE (1 << 24)




class Trie{
  public:
  
  int node_count;
  ulong word_count;
  
  typedef Child ChildList[NODE_SIZE];
  vector<ChildList*> table;

  
  Trie() : node_count(1), word_count(0){
    table.push_back(new ChildList[MEM_CLUSTER_SIZE]);
  }
  
  ~Trie(){
    for (int i = 0; i < table.size(); i++) {
      delete[] table[i];
    }
    
  }
  
  void alloc_table(ulong id){
    if(id == 0){
      fprintf(stderr, "Error : id space overflown\n");
      abort();
    }
    if(id / MEM_CLUSTER_SIZE >= table.size() ){
      table.push_back(new ChildList[MEM_CLUSTER_SIZE]);
    }
  }
  
  
  void insert(char* word){
    
    Child* child = NULL;
    ulong id = 0;
    
    for (char* ptr = word; (*ptr) != '\0'; ptr++) {
      
      char c = (*ptr);
      for(char s = 8 - GRAN_BITS ; s >= 0 ; s -= GRAN_BITS){
        
        if(debug) printf("char %c for %s[%ld], s = %d\n", c, word, ptr - word, s);

        //get id of current node (before insert)
        if(child){
          id = child->id();
          if(id == 0){
            id = node_count++;
            alloc_table(id);
            child->set_id(id);
          }
        }
        
        //get index from that char
        unsigned char idx = (c >> s) & MASK;
        if(debug) printf("%c = %x, idx = %x\n", c, c, idx);

        
        //find child for next
        ChildList& list = table[id / MEM_CLUSTER_SIZE][id % MEM_CLUSTER_SIZE];
        child = &(list[idx]);
      }
    }
    
    if(!child->is_term() ){
      word_count++;
      child->set_term();
    };
  }
  
  
  
  
  bool search(char* word){

    Child* child = NULL;
    ulong id = 0;
    
    for (char* ptr = word; (*ptr) != '\0'; ptr++) {
      
      char c = (*ptr);
      for(char s = 8 - GRAN_BITS ; s >= 0 ; s -= GRAN_BITS){
        
        if(debug) printf("char %c for %s[%ld], s = %d\n", c, word, ptr - word, s);

        if(child){
          id = child->id();
          if(id == 0){
            return false;
          }
        }

        //get index from that char
        unsigned char idx = (c >> s) & MASK;
        if(debug) printf("%c = %x, idx = %x\n", c, c, idx);
        
        
        //find child for next
        ChildList& list = table[id / MEM_CLUSTER_SIZE][id % MEM_CLUSTER_SIZE];
        child = &(list[idx]);

        
      }
    }

    return child->is_term();
  }
  
  
  void print_node(ulong id, string& buf, char sub_c, int s){
    ChildList& list = table[id / MEM_CLUSTER_SIZE][id % MEM_CLUSTER_SIZE];
    
    for(int idx = 0 ; idx < NODE_SIZE ; idx++){
      Child& child = list[idx];
      
      if(child.is_term()){
        char c = sub_c | idx;
        buf.push_back(c);
        printf("%s\n", buf.c_str());
        buf.pop_back();
      }

      id = child.id();
      if(id > 0){     // if has node
        char c = sub_c | idx << s;
        if(s > 0){
          print_node(id, buf, c, s -= GRAN_BITS);
        }else{
          buf.push_back(c);
          print_node(id, buf, 0, 8 - GRAN_BITS);
          buf.pop_back();
        }
      }
      
    }
  }
  
  void print(){
    printf("print\n");
    string buf;
    print_node(0, buf, 0, 0);
  }
  
  void print_status(){
    printf("word_count = %lu\n", word_count);
    printf("node_count = %d\n", node_count);
    printf("table.size = %lu\n", table.size());
  }
  
  
};




#define BUF_SIZE 2048000


int main(int argc, char** argv)
{
  
  Trie trie;
  printf("node cluster size = %d\n", MEM_CLUSTER_SIZE);

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

  
  //trie.print();
  trie.print_status();
  fclose(fd);

  

  
  return 0;
}

