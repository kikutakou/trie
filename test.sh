#!/bin/bash


cd $(dirname $0)

TEXT="/Users/kikuta/NLP-data/wikipedia_data/lowcorpus_neologd_wakatied_contents.txt"

mkdir -p bin


#
#g++ -O3 src/id-trie.cc -o bin/trie
#./bin/trie "$TEXT"


g++ -O3 src/hashtable.cc -o bin/hashtable
./bin/hashtable "$TEXT"







