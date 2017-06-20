#!/bin/bash


cd $(dirname $0)

TEXT="/Users/kikuta/NLP-data/wikipedia_data/lowcorpus_wakatied_ipa_noun.txt"

mkdir -p bin



g++ -O3 src/id-trie.cc -o bin/trie
./bin/trie "$TEXT"


g++ -O3 src/hashtable.cc -o bin/hashtable
./bin/hashtable "$TEXT"


g++ -O3 src/unorderedmap.cc -o bin/unorderedmap
./bin/unorderedmap "$TEXT"




