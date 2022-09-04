            Easy, single c++ file library to store and substitute command line commands

[![Build](https://github.com/lazzyfox/clidict/actions/workflows/cmake_build.yml/badge.svg)](https://github.com/lazzyfox/clidict/actions/workflows/cmake_build.yml)

  Container structure
Booth kind of containers a Radix tree based on linked leasts. 
More details about Radix tree -  https://en.wikipedia.org/wiki/Radix_tree

  Why Radix tree
Radix tree is  a typical solution for dictionary data store. Because of access time and usability

 Access time
This structure is not a balanced binary one (like AVL or Red-Blak) and access time could not be O(log n) but usually it pretty close to.

 Usability
Because every node contains unique part of a word it could be not only one letter but a set (sequence) of letters as well. And in this case all sequence could be acceded by first letters and could be substituted to input request. If thre are two commands - test1 and test2 they are will be stored as tree nodes - test, 1, 2. And first letter of command - 't' will be enough to get biggest part of command - sequence 'test'.
 
   How to use library
For single string commands - radix_dict::RadixTree<char> radix {{'t', 'e', 's', 't', '1'}, {'t', 'e', 's', 't', '2'}};
For delimiter separated commands commands - 
 cli_dict::Dict<char> dict{'-', {{'t', 'e', 's', 't', '1', '-', 't', 'e', 's', 't', '1'}, {'t', 'e', 's', 't', '2', '-', 't', 'e', 's', 't', '2'}}};

   Compilation/installation
Could be used a c++ header file or installed as internal library:
cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/your/installation/path
cmake --build . --config Release --target install -- -j $(nproc)

  Rights to use
MIT license
