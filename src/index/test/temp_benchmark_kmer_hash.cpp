/*
 * Copyright 2015 Georgia Institute of Technology
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file    test_kmer_reverse.cpp
 * @ingroup
 * @author  tpan
 * @brief
 * @details
 *
 */



#include "utils/logging.h"

// include google test
#include <gtest/gtest.h>
#include "utils/bitgroup_ops.hpp"

#include <random>
#include <cstdint>

#include "common/kmer.hpp"
#include "common/test/kmer_reverse_helper.hpp"

#include "common/alphabets.hpp"
#include "common/alphabet_traits.hpp"
#include "utils/benchmark_utils.hpp"


// include files to test

//TESTS: Sequential, SWAR/BSWAP, SSSE3, AVX2 versions of kmer reverse.
//TESTS: for each, test kmer size, different word type, and Alphabet (bit group size)
//       different offsets, different bit group sizes, different word types, and different byte array lengths.

template <typename T>
class KmerReverseBenchmark : public ::testing::Test {
  protected:

    static T kmer;
    static T rev_gold;
    static T revcomp_gold;

    static bliss::common::test::KmerReverseHelper<T> helper;

    static constexpr size_t iterations = 10000000;

    static std::vector<uint8_t> chars;

  public:
    static void SetUpTestCase()
    {
      srand(0);
      for (unsigned int i = 0; i < T::size; ++i) {
        kmer.nextFromChar(rand() % T::KmerAlphabet::SIZE);
      }

      srand(23);
      chars.resize(iterations);
      for (size_t i = 0; i < iterations; ++i) {
        chars[i] = rand() % T::KmerAlphabet::SIZE;
      }

      T km = kmer;
      for (size_t i = 0; i < iterations; ++i) {
        rev_gold ^= helper.reverse_swar(km);
        revcomp_gold ^= helper.reverse_complement_swar(km);

        km.nextFromChar(chars[i]);
      }
     }
    static void TearDownTestCast() {
    	std::vector<uint8_t>().swap(chars);
    }
};


template <typename T>
T KmerReverseBenchmark<T>::kmer;
template <typename T>
T KmerReverseBenchmark<T>::rev_gold;
template <typename T>
T KmerReverseBenchmark<T>::revcomp_gold;

template <typename T>
bliss::common::test::KmerReverseHelper<T> KmerReverseBenchmark<T>::helper;

template <typename T>
constexpr size_t KmerReverseBenchmark<T>::iterations;

template <typename T>
std::vector<uint8_t> KmerReverseBenchmark<T>::chars;


// to save some typing.  note that func is not a functor nor lambda function, so using macro is easier than as a templated function.
#define TEST_REV(name, func, kmertype, gold) do { \
    TypeParam km, rev, tmp; \
    km = KmerReverseBenchmark<kmertype>::kmer; \
    \
    BL_TIMER_START(km); \
    \
    for (size_t i = 0; i < KmerReverseBenchmark<kmertype>::iterations; ++i) { \
      \
      tmp = func; \
      \
      rev ^= tmp; \
      \
      km.nextFromChar(KmerReverseBenchmark<kmertype>::chars[i]); \
    } \
    BL_TIMER_END(km, name, KmerReverseBenchmark<kmertype>::iterations); \
    \
    EXPECT_TRUE(rev == KmerReverseBenchmark<kmertype>::gold); \
    } while (0)



// indicate this is a typed test
TYPED_TEST_CASE_P(KmerReverseBenchmark);

TYPED_TEST_P(KmerReverseBenchmark, reverse)
{
  BL_TIMER_INIT(km);

  TEST_REV("rev", km.reverse(), TypeParam, rev_gold);
//  TEST_REV("seq", KmerReverseBenchmark<TypeParam>::helper.reverse_serial(km), TypeParam, rev_gold);

  if ((TypeParam::bitsPerChar & (TypeParam::bitsPerChar - 1)) == 0) {
    TEST_REV("bswap", KmerReverseBenchmark<TypeParam>::helper.reverse_bswap(km), TypeParam, rev_gold);
    TEST_REV("swar", KmerReverseBenchmark<TypeParam>::helper.reverse_swar(km), TypeParam, rev_gold);

#ifdef __SSSE3__
    TEST_REV("ssse3", KmerReverseBenchmark<TypeParam>::helper.reverse_simd(km), TypeParam, rev_gold);
#endif

  }

  {
  TypeParam km, rev, tmp;
  km = KmerReverseBenchmark<TypeParam>::kmer;

  uint8_t* out = reinterpret_cast<uint8_t*>(tmp.getDataRef());
  const uint8_t* in = reinterpret_cast<uint8_t const *>(km.getConstData());

  BL_TIMER_START(km);

  for (size_t i = 0; i < KmerReverseBenchmark<TypeParam>::iterations; ++i) {

    memset(out, 0, TypeParam::nBytes);

    bliss::utils::bit_ops::reverse_seq<TypeParam::bitsPerChar>(out, in, TypeParam::nBytes);
    tmp.template right_shift_bits<(TypeParam::nBytes * 8 - TypeParam::nBits)>();  // shift by remainder/padding.

    rev ^= tmp;

    km.nextFromChar(KmerReverseBenchmark<TypeParam>::chars[i]);
  }
  BL_TIMER_END(km, "seqnew", KmerReverseBenchmark<TypeParam>::iterations);

  if (rev != KmerReverseBenchmark<TypeParam>::rev_gold) {
    std::cout << "rev: " << rev.toAlphabetString() << std::endl;
    std::cout << "rev_gold: " << KmerReverseBenchmark<TypeParam>::rev_gold.toAlphabetString() << std::endl;
    std::cout << "tmp: " << tmp.toAlphabetString() << std::endl;
  }
  EXPECT_TRUE(rev == KmerReverseBenchmark<TypeParam>::rev_gold);
  }

  BL_TIMER_REPORT(km);


}


TYPED_TEST_P(KmerReverseBenchmark, revcomp)
{
  BL_TIMER_INIT(km);

  TEST_REV("revC", km.reverse_complement(), TypeParam, revcomp_gold);
//  TEST_REV("seqC", KmerReverseBenchmark<TypeParam>::helper.reverse_complement_serial(km), TypeParam, revcomp_gold);

  if ((TypeParam::bitsPerChar & (TypeParam::bitsPerChar - 1)) == 0) {
    TEST_REV("bswapC", KmerReverseBenchmark<TypeParam>::helper.reverse_complement_bswap(km), TypeParam, revcomp_gold);
    TEST_REV("swarC", KmerReverseBenchmark<TypeParam>::helper.reverse_complement_swar(km), TypeParam, revcomp_gold);

#ifdef __SSSE3__
    TEST_REV("ssse3C", KmerReverseBenchmark<TypeParam>::helper.reverse_complement_simd(km), TypeParam, revcomp_gold);
#endif

  }
  BL_TIMER_REPORT(km);

}





//REGISTER_TYPED_TEST_CASE_P(KmerReverseBenchmark, rev_seq, rev_seq2, revcomp_seq, rev_bswap, revcomp_bswap, rev_swar, revcomp_swar, rev, revcomp, rev_ssse3, revcomp_ssse3);

REGISTER_TYPED_TEST_CASE_P(KmerReverseBenchmark, reverse, revcomp);

//////////////////// RUN the tests with different types.

// max of 50 cases
//typedef ::testing::Types<
//    ::bliss::common::Kmer< 15, bliss::common::DNA,   uint64_t>,
//    ::bliss::common::Kmer< 32, bliss::common::DNA,   uint64_t>,
//    ::bliss::common::Kmer< 47, bliss::common::DNA,   uint64_t>,
//    ::bliss::common::Kmer< 64, bliss::common::DNA,   uint64_t>,
//    ::bliss::common::Kmer< 96, bliss::common::DNA,   uint64_t>,
//    ::bliss::common::Kmer<128, bliss::common::DNA,   uint64_t>,
//    ::bliss::common::Kmer<192, bliss::common::DNA,   uint64_t>,
//    ::bliss::common::Kmer<256, bliss::common::DNA,   uint64_t>,
//    ::bliss::common::Kmer< 15, bliss::common::DNA5,  uint64_t>,
//    ::bliss::common::Kmer< 32, bliss::common::DNA5,  uint64_t>,
//    ::bliss::common::Kmer< 47, bliss::common::DNA5,  uint64_t>,
//    ::bliss::common::Kmer< 64, bliss::common::DNA5,  uint64_t>,
//    ::bliss::common::Kmer< 96, bliss::common::DNA5,  uint64_t>,
//    ::bliss::common::Kmer<128, bliss::common::DNA5,  uint64_t>,
//    ::bliss::common::Kmer<192, bliss::common::DNA5,  uint64_t>,
//    ::bliss::common::Kmer<256, bliss::common::DNA5,  uint64_t>,
//    ::bliss::common::Kmer< 15, bliss::common::DNA16, uint64_t>,
//    ::bliss::common::Kmer< 16, bliss::common::DNA16, uint64_t>,
//    ::bliss::common::Kmer< 32, bliss::common::DNA16, uint64_t>,
//    ::bliss::common::Kmer< 47, bliss::common::DNA16, uint64_t>,
//    ::bliss::common::Kmer< 64, bliss::common::DNA16, uint64_t>,
//    ::bliss::common::Kmer< 96, bliss::common::DNA16, uint64_t>,
//    ::bliss::common::Kmer<128, bliss::common::DNA16, uint64_t>,
//    ::bliss::common::Kmer<192, bliss::common::DNA16, uint64_t>,
//    ::bliss::common::Kmer<256, bliss::common::DNA16, uint64_t>,
//    ::bliss::common::Kmer< 15, bliss::common::DNA_IUPAC, uint64_t>,
//    ::bliss::common::Kmer< 16, bliss::common::DNA_IUPAC, uint64_t>,
//    ::bliss::common::Kmer< 32, bliss::common::DNA_IUPAC, uint64_t>,
//    ::bliss::common::Kmer< 47, bliss::common::DNA_IUPAC, uint64_t>,
//    ::bliss::common::Kmer< 64, bliss::common::DNA_IUPAC, uint64_t>,
//    ::bliss::common::Kmer< 96, bliss::common::DNA_IUPAC, uint64_t>,
//    ::bliss::common::Kmer<128, bliss::common::DNA_IUPAC, uint64_t>,
//    ::bliss::common::Kmer<192, bliss::common::DNA_IUPAC, uint64_t>,
//    ::bliss::common::Kmer<256, bliss::common::DNA_IUPAC, uint64_t>,
//    ::bliss::common::Kmer< 7, bliss::common::ASCII, uint64_t>,
//    ::bliss::common::Kmer< 15, bliss::common::ASCII, uint64_t>,
//    ::bliss::common::Kmer< 16, bliss::common::ASCII, uint64_t>,
//    ::bliss::common::Kmer< 32, bliss::common::ASCII, uint64_t>,
//    ::bliss::common::Kmer< 47, bliss::common::ASCII, uint64_t>,
//    ::bliss::common::Kmer< 64, bliss::common::ASCII, uint64_t>,
//    ::bliss::common::Kmer< 96, bliss::common::ASCII, uint64_t>,
//    ::bliss::common::Kmer<128, bliss::common::ASCII, uint64_t>,
//    ::bliss::common::Kmer<192, bliss::common::ASCII, uint64_t>,
//    ::bliss::common::Kmer<256, bliss::common::ASCII, uint64_t>
//> KmerReverseBenchmarkTypes;
typedef ::testing::Types<
    ::bliss::common::Kmer< 192, bliss::common::DNA,   uint64_t>,
     ::bliss::common::Kmer< 96, bliss::common::DNA16,   uint64_t>
> KmerReverseBenchmarkTypes;
INSTANTIATE_TYPED_TEST_CASE_P(Bliss, KmerReverseBenchmark, KmerReverseBenchmarkTypes);

