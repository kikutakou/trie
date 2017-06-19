#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

class Node {
private:
  
  typedef map<char, Node*> Map;
  Map children;
  
public:
  
  bool terminated;
  
  Node() : terminated(false){};
  
  Node* find(char c){
    Map::iterator it = children.find(c);
    return (it == children.end()) ? NULL : it->second;
  }
  
  Node* add_new(char c){
    Node* new_node = new Node();
    children[c] = new_node;
    return new_node;
  }
  
  void print_all(string& buf){
    for (Map::iterator it = children.begin() ; it != children.end(); it++) {
      
      buf += it->first;
      if(it->second->terminated > 0){
        printf("%s\n", buf.c_str());
      }
      it->second->print_all(buf);
      buf.pop_back();
    }
  }
  
};


class Trie {
private:
  
  // root node
  Node root;
  
  // for memory release
  vector<Node*> all_nodes;

public:

  unsigned long count;

  Trie() {
    count = 0;
  };
  
  ~Trie(){
    for (int i=0; i<all_nodes.size(); ++i) {
      delete all_nodes[i];
    }
  }
  
  bool insert(char* word){
    Node* cur = &root;
    
    for (char* ptr = word; (*ptr) != '\0'; ptr++) {
      
      Node* next = cur->find(*ptr);
      
      if(next){
        cur = next;
        
      }else{
        Node* new_node = cur->add_new(*ptr);
        all_nodes.push_back(new_node);  // add to all node list
        cur = new_node;   // move to the next
        
      }
    }
    
    bool is_already_added = cur->terminated;
    if(!is_already_added){
      cur->terminated = true;
      count++;
    }
    
    return is_already_added;
    
  }
  
  bool search(char* word){
    Node* cur = &root;
    
    for (char* ptr = word; (*ptr) != '\0'; ptr++) {
      cur = cur->find(*ptr);
      if (!cur) return false;
    }
    
    return cur->terminated;
  }
  
  void print(){
    string buf;
    root.print_all(buf);
  }
  
};



#define BUF_SIZE 2048000


int main(int argc, char** argv)
{
  
  Trie trie;
  
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

