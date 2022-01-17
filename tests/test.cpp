#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include <variant>
#include <iostream>
#include "../clio.h"
#include "../dict.h"

using namespace testing;
using namespace radix_dict;
using namespace cli_dict;


class RadixNode : public ::testing::Test { 
  protected :
    vector<char> s1 {'a'};
    vector<char> s2 {'b'};
    vector<char> s3 {'t', 'e', 's', 't'};
    vector<char> s4 {'t', 'e', 's', 't', '1'};
    vector<char> s5 {'t', 'e', 's', 't', '2'};

    const vector<char> t1_1 {'t'};
    const vector<char> t1_2 {'t','e'};
    const vector<char> t1_3 {'t', 'e', 's', 't'};
    const vector<char> t1_4 {'t', 'e', 's', 't', '1'};
    const vector<char> t1_5 {'t', 'e', 's', 't', '2'};

    RadixTree<char> radix{};
    virtual void SetUp() {
      auto a1 {s1}; 
      auto a2 {s2}; 
      auto a3 {s3}; 
      radix.ins(std::move(a1));
      radix.ins(std::move(a2));
      radix.ins(std::move(a3));
    }
};

TEST_F(RadixNode, NodeInsertBasic) {
  auto res1 = radix.get(vector<char>(s1));
  EXPECT_THAT(*res1, ContainerEq(s1));
  auto res2 = radix.get(vector<char>(s2));
  EXPECT_THAT(*res2, ContainerEq(s2));
  auto res3 = radix.get(vector<char>(s3));
}

TEST_F(RadixNode, NodeInsertTest12) {
  radix.ins(vector<char>(s4));
  radix.ins(vector<char>(s5));
  auto res1 = radix.get(vector<char>(t1_1));
  EXPECT_THAT(*res1, ContainerEq(s3));
  auto res2 = radix.get(vector<char>(t1_2));
  EXPECT_THAT(*res2, ContainerEq(s3));
  auto res3 = radix.get(vector<char>(t1_3));
  EXPECT_THAT(*res3, ContainerEq(s3));
  auto res4 = radix.get(vector<char>(t1_4));
  EXPECT_THAT(*res4, ContainerEq(t1_4));
  auto res5 = radix.get(vector<char>(t1_5));
  EXPECT_THAT(*res5, ContainerEq(t1_5));
}

TEST_F(RadixNode, NodeInsertTestTE) {
  radix.ins(vector<char>(t1_2));
  auto res1 = radix.get(vector<char>(t1_1));
  EXPECT_THAT(*res1, ContainerEq(t1_2));
  auto res2 = radix.get(vector<char>(t1_2));
  EXPECT_THAT(*res2, ContainerEq(t1_2));
  auto res3 = radix.get(vector<char>(t1_3));
  EXPECT_THAT(*res3, ContainerEq(s3));
}


class DictNode : public ::testing::Test {
  protected:
    vector<char> s0{'t', 'e', 's', 't'};
    vector<char> s1{'t', 'e', 's', 't', '1'};
    vector<char> s2 {'t', 'e', 's', 't', '1', '-', 't', 'e', 's', 't', '1'};
    vector<char> s3 {'t', 'e', 's', 't', '2', '-', 't', 'e', 's', 't', '2'};
    vector<char> s4 {'t', 'e', 's', 't', '3', '-', 't', 'e', 's', 't', '3'};
    vector<char> s5{'t', 'e', 's', 't', '1', '-'};
    vector<char> s6{'t', 'e', 's', 't', '1', '-', 't', 'e', 's', 't'};
    
    const vector<char> t1_1 {'t'};
    const vector<char> t1_2 {'t','e'};
    const vector<char> t1_3 {'t', 'e', 's', 't'};
    const vector<char> t1_4 {'t', 'e', 's', 't', '1'};
    const vector<char> t1_5 {'t', 'e', 's', 't', '1', '-'};
    const vector<char> t1_6 {'t', 'e', 's', 't', '1', '-', 't'};
    const vector<char> t1_7 {'t', 'e', 's', 't', '1', '-', 't', 'e', 's', 't'};
    const vector<char> t1_8 {'t', 'e', 's', 't', '1', '-', 't', 'e', 's', 't', '1'};

    const vector<char> t2_1 {'t', 'e', 's', 't', '2'};
    const vector<char> t2_2 {'t', 'e', 's', 't', '2', '-'};
    const vector<char> t2_3 {'t', 'e', 's', 't', '2', '-', 't'};
    const vector<char> t2_4 {'t', 'e', 's', 't', '2', '-', 't', 'e', 's', 't'};
    const vector<char> t2_5 {'t', 'e', 's', 't', '2', '-', 't', 'e', 's', 't', '2'};

    const vector<char> t3_1 {'t', 'e', 's', 't', '3'};
    const vector<char> t3_2 {'t', 'e', 's', 't', '3', '-'};
    const vector<char> t3_3 {'t', 'e', 's', 't', '3', '-', 't'};
    const vector<char> t3_4 {'t', 'e', 's', 't', '3', '-', 't', 'e', 's', 't'};
    const vector<char> t3_5 {'t', 'e', 's', 't', '3', '-', 't', 'e', 's', 't', '3'};

    Dict<char> dict{'-'};
    
    virtual void SetUp() {
      dict.ins(vector<char>(s1));
    }
};

TEST_F(DictNode, NodeInsertBasic) {
  auto res1 = dict.get(vector<char>(t1_1));
  EXPECT_THAT(*res1, ContainerEq(s1));
  auto res2 = dict.get(vector<char>(t1_2));
  EXPECT_THAT(*res2, ContainerEq(s1));
  auto res3 = dict.get(vector<char>(t1_3));
  EXPECT_THAT(*res3, ContainerEq(s1));
  auto res4 = dict.get(vector<char>(t1_4));
  EXPECT_THAT(*res4, ContainerEq(s1));
}

TEST_F(DictNode, NodeInserts2) {
  dict.ins(vector<char>(s2));
  auto res1 = dict.get(vector<char>(t1_1));
  EXPECT_THAT(*res1, ContainerEq(s1));
  auto res4 = dict.get(vector<char>(t1_4));
  EXPECT_THAT(*res4, ContainerEq(s1));
  auto res5 = dict.get(vector<char>(t1_5));
  EXPECT_THAT(*res5, ContainerEq(s5));
  auto res6 = dict.get(vector<char>(t1_6));
  EXPECT_THAT(*res6, ContainerEq(s2));
  auto res7 = dict.get(vector<char>(t1_7));
  EXPECT_THAT(*res7, ContainerEq(s2));
  auto res8 = dict.get(vector<char>(t1_8));
  EXPECT_THAT(*res8, ContainerEq(s2));
}

TEST_F(DictNode, NodeInserts23) {
  dict.ins(vector<char>(s2));
  dict.ins(vector<char>(s3));
  auto res1 = dict.get(vector<char>(t1_1));
  EXPECT_THAT(*res1, ContainerEq(s0));
  auto res4 = dict.get(vector<char>(t1_4));
  EXPECT_THAT(*res4, ContainerEq(s1));
  auto res5 = dict.get(vector<char>(t1_5));
  EXPECT_THAT(*res5, ContainerEq(s5));
  auto res6 = dict.get(vector<char>(t1_6));
  EXPECT_THAT(*res6, ContainerEq(s2));
  auto res7 = dict.get(vector<char>(t1_7));
  EXPECT_THAT(*res7, ContainerEq(s2));
  auto res8 = dict.get(vector<char>(t1_8));
  EXPECT_THAT(*res8, ContainerEq(s2));
  auto res9 = dict.get(vector<char>(t2_1));
  EXPECT_THAT(*res9, ContainerEq(t2_1));
  auto res10 = dict.get(vector<char>(t2_2));
  EXPECT_THAT(*res10, ContainerEq(t2_2));
  auto res11 = dict.get(vector<char>(t2_3));
  EXPECT_THAT(*res11, ContainerEq(s3));
  auto res12 = dict.get(vector<char>(t2_4));
  EXPECT_THAT(*res12, ContainerEq(s3));
}

TEST_F(DictNode, NodeInserts0) {
  dict.ins(vector<char>(s2));
  dict.ins(vector<char>(s3)); 
  dict.ins(vector<char>(s0));
  auto res1 = dict.get(vector<char>(t1_1));
  EXPECT_THAT(*res1, ContainerEq(s0)); 
  auto res2 = dict.get(vector<char>(t1_3));
  EXPECT_THAT(*res2, ContainerEq(s0));
  auto res3 = dict.get(vector<char>(t1_5));
  EXPECT_THAT(*res3, ContainerEq(s5));
  auto res4 = dict.get(vector<char>(t1_6));
  EXPECT_THAT(*res4, ContainerEq(s2));
  auto res5 = dict.get(vector<char>(t1_7));
  EXPECT_THAT(*res5, ContainerEq(s2));
  auto res6 = dict.get(vector<char>(t1_8));
  EXPECT_THAT(*res5, ContainerEq(s2));
  auto res7 = dict.get(vector<char>(t2_5));
  EXPECT_THAT(*res7, ContainerEq(s3));
}

TEST(TestIitList, RadixDict){
  vector<char> s1 {'t', 'e', 's', 't'};
  vector<char> s2 {'t', 'e', 's', 't', '1'};
  vector<char> s3 {'t', 'e', 's', 't', '2'};
  vector<char> s4 {'t', 'e', 's', 't'};
  vector<char> s5 {'t', 'e', 's', 't', '1'};
  vector<char> s6 {'t', 'e', 's', 't', '2'};

  const vector<char> t1_1 {'t'};
  const vector<char> t1_2 {'t','e'};
  const vector<char> t1_3 {'t', 'e', 's', 't'};
  const vector<char> t1_4 {'t', 'e', 's', 't', '1'};
  const vector<char> t1_5 {'t', 'e', 's', 't', '2'};
  
  RadixTree radix {s1, s2, s3};
  auto res1 = radix.get(vector<char>(t1_1));
  EXPECT_THAT(*res1, ContainerEq(s4));
  auto res2 = radix.get(vector<char>(t1_2));
  EXPECT_THAT(*res2, ContainerEq(s4));
  auto res3 = radix.get(vector<char>(t1_3));
  EXPECT_THAT(*res3, ContainerEq(s4));
  auto res4 = radix.get(vector<char>(t1_4));
  EXPECT_THAT(*res4, ContainerEq(s5));
  auto res5 = radix.get(vector<char>(t1_5));
  EXPECT_THAT(*res5, ContainerEq(s6));  
}

TEST(TestIitList, DictDict){
  vector<char> s1 {'t', 'e', 's', 't', '1', '-', 't', 'e', 's', 't', '1'};
  vector<char> s2 {'t', 'e', 's', 't', '2', '-', 't', 'e', 's', 't', '2'};
  vector<char> s3 {'t', 'e', 's', 't', '3', '-', 't', 'e', 's', 't', '3'};
  vector<char> s4 {'t', 'e', 's', 't', '1', '-'};
  vector<char> s5 {'t', 'e', 's', 't'};
  vector<char> s6 {'t', 'e', 's', 't', '1'};

  const vector<char> t1_1 {'t'};
  const vector<char> t1_2 {'t','e'};
  const vector<char> t1_3 {'t', 'e', 's', 't'};
  const vector<char> t1_4 {'t', 'e', 's', 't', '1'};
  const vector<char> t1_5 {'t', 'e', 's', 't', '1', '-'};
  const vector<char> t1_6 {'t', 'e', 's', 't', '1', '-', 't'};
  const vector<char> t1_7 {'t', 'e', 's', 't', '1', '-', 't', 'e', 's', 't'};
  const vector<char> t1_8 {'t', 'e', 's', 't', '1', '-', 't', 'e', 's', 't', '1'};

  const vector<char> t2_1 {'t', 'e', 's', 't', '2'};
  const vector<char> t2_2 {'t', 'e', 's', 't', '2', '-'};
  const vector<char> t2_3 {'t', 'e', 's', 't', '2', '-', 't'};
  const vector<char> t2_4 {'t', 'e', 's', 't', '2', '-', 't', 'e', 's', 't'};
  const vector<char> t2_5 {'t', 'e', 's', 't', '2', '-', 't', 'e', 's', 't', '2'};

  const vector<char> t3_1 {'t', 'e', 's', 't', '3'};
  const vector<char> t3_2 {'t', 'e', 's', 't', '3', '-'};
  const vector<char> t3_3 {'t', 'e', 's', 't', '3', '-', 't'};
  const vector<char> t3_4 {'t', 'e', 's', 't', '3', '-', 't', 'e', 's', 't'};
  const vector<char> t3_5 {'t', 'e', 's', 't', '3', '-', 't', 'e', 's', 't', '3'};
  
  Dict<char> dict{'-', {s1, s2, s3}};
  auto res1 = dict.get(vector<char>(t1_1));
  EXPECT_THAT(*res1, ContainerEq(s5));
  auto res2 = dict.get(vector<char>(t1_2));
  EXPECT_THAT(*res2, ContainerEq(s5));
  auto res3 = dict.get(vector<char>(t1_3));
  EXPECT_THAT(*res3, ContainerEq(s5));
  auto res4 = dict.get(vector<char>(t1_4));
  EXPECT_THAT(*res4, ContainerEq(s6));
  auto res5 = dict.get(vector<char>(t1_5));
  EXPECT_THAT(*res5, ContainerEq(s4));
  auto res6 = dict.get(vector<char>(t1_6));
  EXPECT_THAT(*res6, ContainerEq(s1));
  auto res7 = dict.get(vector<char>(t1_7));
  EXPECT_THAT(*res7, ContainerEq(s1));
  auto res8 = dict.get(vector<char>(t1_8));
  EXPECT_THAT(*res8, ContainerEq(s1));
  auto res9 = dict.get(vector<char>(t2_1));
  EXPECT_THAT(*res9, ContainerEq(t2_1));
  auto res10 = dict.get(vector<char>(t2_2));
  EXPECT_THAT(*res10, ContainerEq(t2_2));
  auto res11 = dict.get(vector<char>(t2_3));
  EXPECT_THAT(*res11, ContainerEq(s2));
  auto res12 = dict.get(vector<char>(t2_4));
  EXPECT_THAT(*res12, ContainerEq(s2)); 
}

GTEST_API_ int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}