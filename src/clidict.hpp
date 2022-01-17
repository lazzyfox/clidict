/*
Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Andrey Fokin lazzyfox@gmail.com.
Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*! \mainpage Project clidict Index Page
 *
 * \section intro_sec Introduction
 *
 *  Easy, single c++ file library to store and substitute command line commands
 *
 * \section description Radix tree
 *   
 *  Radix tree is  a typical solution for dictionary data store. Because of access time and usability
 *
 * \subsection time Access Time
 *  This structure is not a balanced binary one (like AVL or Red-Blake) and access time could not be O(log n)
 *  but usually it pretty close to.
 * 
 * \subsection use Usability
 *  Because every node contains unique part of a word it could be not only one letter but a set (sequence) of letters as well. 
 *  And in this case all sequence could be acceded by first letters and could be substituted to input request. 
 *  If there are two commands - test1 and test2 they are will be stored as tree nodes - test, 1, 2. And first letter of command - 't'
 *  will be enough to get biggest part of command - sequence 'test'.
 *
 * \section lib_using How to use library
 * 
 *  For single string commands - radix_dict::RadixTree<char> radix {{'t', 'e', 's', 't', '1'}, {'t', 'e', 's', 't', '2'}};
 *  For delimiter separated commands commands - 
 *  cli_dict::Dict<char> dict{'-', {{'t', 'e', 's', 't', '1', '-', 't', 'e', 's', 't', '1'}, {'t', 'e', 's', 't', '2', '-', 't', 'e', 's', 't', '2'}}};
 *
 * 
 * \section install Compilation/installation
 *  Could be used a c++ header file or installed as internal library:
 *    cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/your/installation/path
 *    cmake --build . --config Release --target install -- -j $(nproc)
 * 
 * 
 */


#ifndef DICT_H
#define DICT_H

#include <initializer_list>
#include <string_view>
#include <map>
#include <memory>
#include <string>
#include <forward_list>
#include <variant>
#include <concepts>
#include <vector>
#include <unordered_map>
#include <ranges>
#include <algorithm>
#include <utility>
#include <typeinfo>

using std::initializer_list;
using std::map;
using std::unique_ptr;
using std::make_unique;
using std::string;
using std::string_view;
using std::forward_list;
using std::variant;
using std::vector;
using std::unordered_map;
using std::pair;


namespace ranges = std::ranges;
namespace views = std::ranges::views;

namespace ranges = std::ranges;
namespace views = std::ranges::views;



namespace radix_dict{
  template <typename T> class RadixTree;

  template <typename T> struct TreeNode {
    TreeNode (unique_ptr<vector<T>> letters) : letters{std::move(letters)}{}
    TreeNode (const vector<T> &&letters) : letters{make_unique<vector<T>>(std::move(letters))}{}
    TreeNode (const vector<T> &&letters, vector<T> &&post_letters) : letters{make_unique<vector<T>>(std::move(letters))}, postfix_tree{make_unique<RadixTree<T>>(std::move(post_letters))}{}
    TreeNode (const vector<T>&& letters, unique_ptr<RadixTree<T>> postfix_tree) : letters{make_unique<vector<T>>(std::move(letters))}, postfix_tree{std::move(postfix_tree)}{}
    TreeNode (const vector<T>&& letters, unique_ptr<unordered_map<T, unique_ptr<TreeNode<T>>>> branch, unique_ptr<RadixTree<T>> postfix_tree) :
      letters{make_unique<vector<T>>(std::move(letters))}, main_branches{std::move(branch)}, postfix_tree{std::move(postfix_tree)}{}
    TreeNode (unique_ptr<vector<T>> letters, unique_ptr<unordered_map<T, unique_ptr<TreeNode<T>>>> branch) : letters{std::move(letters)}, main_branches{std::move(branch)}{}
    TreeNode (unique_ptr<vector<T>> letters, unique_ptr<unordered_map<T, unique_ptr<TreeNode<T>>>> branch, unique_ptr<RadixTree<T>> postfix_tree) :
      letters{std::move(letters)}, main_branches{std::move(branch)}, postfix_tree{std::move(postfix_tree)}{}
    TreeNode (unique_ptr<vector<T>> letters, unique_ptr<vector<T>> post_letters, unique_ptr<unordered_map<T, unique_ptr<TreeNode<T>>>> branch) :
      letters{std::move(letters)}, main_branches{std::move(branch)}, postfix_tree{std::move(postfix_tree)}{postfix_tree = make_unique<RadixTree<T>>(std::move(post_letters));}
    TreeNode (const vector<T>&& letters, vector<T>&& post_letters, unique_ptr<unordered_map<T, unique_ptr<TreeNode<T>>>> branch) :
      main_branches{std::move(branch)}, postfix_tree{std::move(postfix_tree)}{this->postfix_tree = make_unique<RadixTree<T>>(std::move(post_letters)); this->letters = make_unique<vector<T>>(letters);}


    unique_ptr<vector<T>> letters;
    unique_ptr<unordered_map<T, unique_ptr<TreeNode<T>>>> main_branches; // Branches to different letters
    unique_ptr<RadixTree<T>> postfix_tree; //  Branch to second word's part (postfix one)
  };

  ///  Radix tree to store commands - a dictionary, as a matter of fact
  template <typename T> class RadixTree {
    protected :
      unique_ptr<unordered_map<T, unique_ptr<TreeNode<T>>>> root_branches;
      //  Find a node of a tree with word
      pair<pair<size_t, size_t>, TreeNode<T>*> goTree(TreeNode<T>* root, vector<T> &&word) const noexcept {
        size_t vec_count {0};
        size_t total_count {0};

        //  Going down tree
        while (!word.empty()) {

          //  Check how many letters in new word are equal in in letters of existing one
          for (vec_count = 0; vec_count < word.size(); ++vec_count) {
            if (vec_count >= root->letters->size() - 1) {
              break;
            }
            if (word.at(vec_count) != root->letters->at(vec_count)) {
              break;
            }
          }

          //  Check if words are same
          if (vec_count == word.size()) {
            break;
          }

          //  There is a difference, case to modify a tree
          if (vec_count != root->letters->size() - 1) {
            break;
          }

          //  Going tree down (next level)
          if (total_count) {
            ++total_count;
          }
          total_count += vec_count;
          word.erase(word.begin(), word.begin() + vec_count + 1);
          if (root->main_branches && root->main_branches->contains(word.front())) {
            root = root->main_branches->at(word.front()).get();
          } else {
            break;
          }
        }
        if (total_count < vec_count) {
          total_count = vec_count;
        }
        return std::make_pair(std::make_pair(total_count - vec_count, vec_count), root);
      }
   
      public :
        ///  Creating an empty container
        RadixTree () : root_branches{make_unique<unordered_map<T, unique_ptr<TreeNode<T>>>>()}{}
        ///  Creating a container with a single commands instance
        RadixTree (vector<T> &&letters) : root_branches{make_unique<unordered_map<T, unique_ptr<TreeNode<T>>>>()}{ins(std::move(letters));}
        ///  Creating a container with a list of instances
        RadixTree (std::initializer_list<vector<T>> init): RadixTree() {ranges::for_each(init, [this](auto word){ins(std::move(word));});}
        ~RadixTree () = default;
        ///  Insert a new item to container
        bool ins(vector<T> &&word) noexcept {
          bool ret{false};
          TreeNode<T>* tree_node = nullptr;
          size_t word_last_pos {word.size() - 1};
          auto new_word = make_unique<vector<T>>();
          unique_ptr<vector<T>> new_branch_word;
          auto old_branch_word = make_unique<vector<T>>();
          size_t smallest_word {0};
          size_t vec_count {0};

          if (word.empty()) {
            return ret;
          }
          //  First insert to empty tree
          if (!root_branches->contains(word.front())) {
            T word_front {word.front()};
            root_branches->emplace(std::move(word_front), make_unique<TreeNode<T>>(std::move(word)));
            return true;
          } else {
            TreeNode<T>* root_branch = root_branches->at(word.front()).get();
            auto tree_pos = goTree(root_branch, vector<T>(word));
            vec_count = tree_pos.first.second;
            tree_node = tree_pos.second;

            //  Check if word already exists
            if (tree_node->letters && *tree_node->letters == word) {
              return ret;
            }
              word.erase(word.begin(), word.begin() + tree_pos.first.first);
              word_last_pos = word.size() - 1;
            }

             //  Probably new branch. Could be 3 cases :
             //  absolutely same - do nothing (out of scope)
             //  just add new branch if new word first symbols are same with all existing
             //  partially similar - a few symbols are same but there is a difference : by difference  creating new nodes (dividing existing one into tow part)
             if (tree_node && tree_node->letters) {
               smallest_word = std::min(word_last_pos, tree_node->letters->size() - 1);
               if (!word.empty()) {
                 size_t word_count = 0;
                 for (auto &count : *tree_node->letters) {
                   if (word_count == smallest_word + 1) {
                     break;
                  }
                  if (count != word.at(word_count)) {
                    break;
                  }
                  new_word->push_back(count);
                  ++word_count;
                }
              }
            }

            vector<T> word_rest {word.begin() + new_word->size(), word.end()};
            // Just add a rest of a word to a new branch (case #2)
            if (new_word->size() == tree_node->letters->size()) {
              auto key = word_rest.front();
              if (!tree_node->main_branches) {
                tree_node->main_branches = make_unique<unordered_map<T, unique_ptr<TreeNode<T>>>>();
              }
              tree_node->main_branches->emplace(key, make_unique<TreeNode<T>>(std::move(word_rest)));
              return ret = true;
            }

            //  Comparison was terminated before end of words, there is a difference
            //  between existing and new word. Case to reorder tree (#3).
            if (vec_count <= word.size() /*&& vec_count > 1*/) {
              new_word.swap(tree_node->letters);
              if (!word_rest.empty()) {
                new_branch_word = make_unique<vector<T>>();
                ranges::move(word_rest, std::back_inserter(*new_branch_word));
              }
              std::move(std::make_move_iterator(new_word->begin() + vec_count), std::make_move_iterator(new_word->end()), std::back_inserter(*old_branch_word));
              if (!old_branch_word->empty()) {
                auto old_word_key = old_branch_word->front();
                auto old_word_node = make_unique<TreeNode<T>>(std::move(old_branch_word), std::move(tree_node->main_branches));
                if (!tree_node->main_branches) {
                  tree_node->main_branches = make_unique<unordered_map<T, unique_ptr<TreeNode<T>>>>();
                }
                tree_node->main_branches->emplace(std::make_pair(old_word_key, std::move(old_word_node)));
              }
              if (new_branch_word) {
                auto new_word_key = new_branch_word->front();
                auto new_word_node = make_unique<TreeNode<T>>(std::move(new_branch_word));
                tree_node->main_branches->emplace(std::make_pair(new_word_key, std::move(new_word_node)));
              }
            }
            return ret;
        }
        ///  Get an item from container
        [[nodiscard]] unique_ptr<vector<T>> get(vector<T> const &&word) const noexcept {
          unique_ptr<vector<T>> ret;

          //  Check data
          if(word.empty()) {
            return ret;
          }

          if (root_branches && root_branches->contains(word.front())) {
            auto node = goTree(root_branches->at(word.front()).get(), vector<T>(word));
            if (node.second) {
              ret = make_unique<vector<T>>(word.begin(), word.begin() + node.first.first);
              ret->insert(ret->end(), node.second->letters->begin(), node.second->letters->end());
            }
          }
          return ret;
        }
  };
}

namespace cli_dict {
  template <typename T> using Leaf = radix_dict::TreeNode<T>;
  template <typename T> using Node = radix_dict::RadixTree<T>;

  ///  Container to store command dived by any kind of delimiters
  template <typename T> class Dict : public Node<T> {
    private :
      T delim;
      unique_ptr<unordered_map<T, unique_ptr<Leaf<T>>>> root;
    public :
      ///  Creating an empty dictionary container with a delimetr
      explicit Dict (const T delim) : Node<T>() {this->delim = delim; root = make_unique<unordered_map<T, unique_ptr<Leaf<T>>>>();};
      ///  Creating a dictionary container with a delimetr by a list of items
      Dict (const T delim, std::initializer_list<vector<T>> init_lst) : Dict (delim) {
        for (auto comm : init_lst) {
          ins (std::move(comm));
        }
      }
      ///  Insert a new item to container
      bool ins(vector<T> &&letters) noexcept {
        bool ret {false};
        vector<T> ins_word, existing_word;
        vector<T> equal_str;

        //  Check data
        if (letters.empty()) {
          return ret;
        }

        //  Divide string into pre and post parts
        vector<T> pre_str, post_str;
        if (auto delim_it {ranges::find(letters, delim)}; delim_it != letters.end()) {
          std::move (std::make_move_iterator(delim_it + 1), std::make_move_iterator(letters.end()), std::back_inserter<vector<T>>(post_str));
          std::move (std::make_move_iterator(letters.begin()), std::make_move_iterator(delim_it), std::back_inserter<vector<T>>(pre_str));
        } else {
          ranges::move(letters, std::back_inserter<vector<T>>(pre_str));
        }

        //  Find position to insert new string
        if (auto key {pre_str.front()}; root->contains(key)) {
          auto tree_position = Node<T>::goTree(root->at(key).get(), vector<T>(pre_str.begin(), pre_str.end()));
          auto tree_node = tree_position.second;
          pre_str.erase(pre_str.begin(), pre_str.begin() + tree_position.first.first);

          //  Just add a new postfix part
          if (pre_str == *tree_position.second->letters) {
            if (!tree_position.second->postfix_tree) {
              tree_position.second->postfix_tree = make_unique<Node<T>>(std::move(pre_str));
              ret = true;
            } else {
              ret = tree_position.second->postfix_tree->ins(std::move(pre_str));
            }
            return ret;
          }
          size_t ex_count = 0;
          for (auto count : pre_str) {
            if (ex_count < tree_node->letters->size() && count == tree_node->letters->at(ex_count)) {
              equal_str.push_back(count);
            } else {
              break;
            }
            ++ex_count;
          }
          auto equal_str_cmp = tree_node->letters->size() <=> equal_str.size();

          //  Existing word is smaller then common words part - case to divide existing node to two branches
          if (equal_str_cmp > 0) {

            //  Creating words part for new branch
            vector<T> new_word_ins, old_word_ins;
            std::copy(tree_node->letters->begin() + equal_str.size(), tree_node->letters->end(), std::back_inserter<vector<T>>(old_word_ins));
            if (pre_str.size() > equal_str.size()) {
              std::copy(pre_str.begin() + equal_str.size(), pre_str.end(), std::back_inserter<vector<T>>(new_word_ins));
            }

            //  Modifing existing node
            std::swap(equal_str, *tree_node->letters);
            auto old_key = old_word_ins.front();
            unique_ptr<Leaf<T>> old_node;
            if (tree_node->main_branches) {
              old_node = make_unique<Leaf<T>>(std::move(old_word_ins), std::move(tree_node->main_branches), std::move(tree_node->postfix_tree));
            } else {
              old_node = make_unique<Leaf<T>>(std::move(old_word_ins), std::move(tree_node->postfix_tree));
            }
            tree_node->main_branches = make_unique<unordered_map<T, unique_ptr<Leaf<T>>>>();
            tree_node->main_branches->emplace(std::make_pair(old_key, std::move(old_node)));
            if (!new_word_ins.empty()) {
              auto new_key = new_word_ins.front();
              auto new_node = make_unique<Leaf<T>>(std::move(new_word_ins), std::move(post_str));
              tree_node->main_branches->emplace(std::make_pair(new_key, std::move(new_node)));
            }
          }

          //  Just add a new postfix part or creating a new node by a rest of a new word
          if (equal_str_cmp == 0) {

            //  A new node case
            if (pre_str.size() > equal_str.size()) {
              if (!tree_node->main_branches) {
                tree_node->main_branches = make_unique<unordered_map<T, unique_ptr<Leaf<T>>>>();
              }
              vector<T> new_word_ins;
              std::copy(pre_str.begin() + equal_str.size(), pre_str.end(), std::back_inserter<vector<T>>(new_word_ins));
              auto new_key = new_word_ins.front();
              auto new_node = make_unique<Leaf<T>>(std::move(new_word_ins), std::move(post_str));
              tree_node->main_branches->emplace(std::make_pair(new_key, std::move(new_node)));
            }

            //  Just add a post part
            if (pre_str.size() == equal_str.size()) {
              if (!tree_node->postfix_tree) {
                tree_node->postfix_tree = make_unique<Node<T>>(std::move(post_str));
              } else {
                tree_node->postfix_tree->ins(std::move(post_str));
              }
            }
          }
        } else {  // Creating new node for empty root node (root leaf)
          T new_key = letters.front();
          unique_ptr<Leaf<T>> new_node;
          if (!post_str.empty()) {
            new_node = make_unique<Leaf<T>> (std::move(pre_str), std::move(post_str));
          } else {
            new_node = make_unique<Leaf<T>> (std::move(pre_str));
          }
          root->emplace(std::make_pair(new_key, std::move(new_node)));
          return true;
        }
        return ret;
      }
      ///  Get an item from container
      [[nodiscard]] unique_ptr<vector<T>> get(vector<T> const &&word) const noexcept {
        unique_ptr<vector<T>> ret;
        bool postfix {false};
        vector<T> pre_str, post_str;

        //  Check data
        if (word.empty()) {
          return ret;
        }

        //  Divide string into pre and post parts
        if (auto delim_it = ranges::find(word, delim); delim_it != word.end()) {
          postfix = true;
          ranges::move(std::make_move_iterator(delim_it + 1), std::make_move_iterator(word.end()), std::back_inserter(post_str));
          ranges::move(std::make_move_iterator(word.begin()), std::make_move_iterator(delim_it), std::back_inserter(pre_str));
        } else {
          ranges::move(std::make_move_iterator(word.begin()), std::make_move_iterator(word.end()), std::back_inserter(pre_str));
        }

        if (root->contains(pre_str.front())) {
          auto node = Node<T>::goTree(root->at(pre_str.front()).get(), vector<T>{pre_str});
          if (node.second) {
            ret = make_unique<vector<T>>(pre_str.begin(), pre_str.begin() + node.first.first);
            ret->insert(ret->end(), node.second->letters->begin(), node.second->letters->end());
            if (postfix) {
              ret->push_back(delim);
              if (!post_str.empty()) {
                auto post_part = node.second->postfix_tree->get(vector<T>{post_str});
                if (post_part) {
                  std::move(post_part->begin(), post_part->end(), std::back_inserter(*ret));
                }
              }
            }
          }
        }
        return ret;
      }
  };

}

#endif