// include google test
#include <gtest/gtest.h>
//#include <boost/concept_check.hpp>

// include classes to test
#include <common/Kmer.hpp>
#include <common/alphabets.hpp>


template<unsigned int KMER_SIZE, typename ALPHABET, typename word_type=WordType>
class MyKmer : bliss::Kmer<KMER_SIZE, ALPHABET, word_type>
{
  public:
    using SuperType = typename bliss::Kmer<KMER_SIZE, ALPHABET, word_type>;

    explicit MyKmer(const SuperType& other) : SuperType(other) {};

    template<typename InputIterator>
    explicit MyKmer(InputIterator iter) : SuperType(iter) {};
    MyKmer() : SuperType() {};
    template <typename InputIterator, typename offset_t>
    // TODO: add option for bit offset in input sequence?
    unsigned int fillFromPackedStream(InputIterator& begin, offset_t& offset, bool stop_on_last = false) {
      return SuperType::fillFromPackedStream(begin, offset, stop_on_last);
    }
    template <typename InputIterator>
    void fillFromChars(InputIterator& begin, bool stop_on_last = false) {
      SuperType::fillFromChars(begin, stop_on_last);
    }
    template <typename InputIterator, typename offset_t>
    void nextFromPackedStream(InputIterator& begin, offset_t& offset) {
      SuperType::nextFromPackedStream(begin, offset);
    }
    void nextFromChar(char c) {
      SuperType::nextFromChar(c);
    }

    inline bool operator==(const MyKmer& rhs) const
    {
      return SuperType::operator==(rhs);
    }
    inline bool operator!=(const MyKmer& rhs) const
    {
      return SuperType::operator!=(rhs);
    }
    inline bool operator<(const MyKmer& rhs) const
    {
      return SuperType::operator<(rhs);
    }
    inline bool operator<=(const MyKmer& rhs) const
    {
      return SuperType::operator<=(rhs);
    }
    inline bool operator>(const MyKmer& rhs) const
    {
      return SuperType::operator>(rhs);
    }
    inline bool operator>=(const MyKmer& rhs) const
    {
      return SuperType::operator>=(rhs);
    }
    inline MyKmer operator^(const MyKmer& rhs) const
    {
      return MyKmer(SuperType::operator^(rhs).data);
    }
    inline MyKmer& operator^=(const MyKmer& rhs) const
    {
      return SuperType::operator^=(rhs);
    }
    inline MyKmer operator&(const MyKmer& rhs) const
    {
      return SuperType::operator&(rhs);
    }
    inline MyKmer& operator&=(const MyKmer& rhs) const
    {
      return SuperType::operator&=(rhs);
    }
    inline MyKmer operator|(const MyKmer& rhs) const
    {
      return SuperType::operator|(rhs);
    }
    inline MyKmer& operator|=(const MyKmer& rhs) const
    {
      return SuperType::operator|=(rhs);
    }
    inline MyKmer operator<<(const MyKmer& rhs) const
    {
      return SuperType::operator<<(rhs);
    }
    inline MyKmer& operator<<=(const MyKmer& rhs) const
    {
      return SuperType::operator<<=(rhs);
    }
    inline MyKmer operator>>(const MyKmer& rhs) const
    {
      return SuperType::operator>>(rhs);
    }
    inline MyKmer& operator>>=(const MyKmer& rhs) const
    {
      return SuperType::operator>>=(rhs);
    }
    MyKmer reversed_kmer() const
    {
      return MyKmer(SuperType::reversed_kmer());
    }
    std::string toString() const
    {
      return SuperType::toString();
    }

};


struct Bits4
{
  static constexpr AlphabetSizeType SIZE = 15;
  // lookup table for XYZ
  static constexpr uint8_t FROM_ASCII[256] =
  {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     'A'     'C'             'G'
    0,  0,  0,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,
//                 'T'
    0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     'a'     'c'             'g'
    0,  0,  0,  1,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,
//                 't'
    0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  };

  // reverse lookup table for XYZ
  static constexpr char TO_ASCII[SIZE] =
  {
    'A',  // = 0
    'C',  // = 1
    'G',  // = 2
    'T'  // = 3
  };

  // reverse lookup table for DNA5
  static constexpr char TO_COMPLEMENT[SIZE] =
  {
    3,  // = 0
    2,  // = 1
    1,  // = 2
    0  // = 3
  };
};
struct Bits5
{
  static constexpr AlphabetSizeType SIZE = 31;
  // lookup table for XYZ
  static constexpr uint8_t FROM_ASCII[256] =
  {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     'A'     'C'             'G'
    0,  0,  0,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,
//                 'T'
    0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     'a'     'c'             'g'
    0,  0,  0,  1,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,
//                 't'
    0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  };

  // reverse lookup table for XYZ
  static constexpr char TO_ASCII[SIZE] =
  {
    'A',  // = 0
    'C',  // = 1
    'G',  // = 2
    'T'  // = 3
  };

  // reverse lookup table for DNA5
  static constexpr char TO_COMPLEMENT[SIZE] =
  {
    3,  // = 0
    2,  // = 1
    1,  // = 2
    0  // = 3
  };
};
struct Bits6
{
  static constexpr AlphabetSizeType SIZE = 63;
  // lookup table for XYZ
  static constexpr uint8_t FROM_ASCII[256] =
  {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     'A'     'C'             'G'
    0,  0,  0,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,
//                 'T'
    0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     'a'     'c'             'g'
    0,  0,  0,  1,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,
//                 't'
    0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  };

  // reverse lookup table for XYZ
  static constexpr char TO_ASCII[SIZE] =
  {
    'A',  // = 0
    'C',  // = 1
    'G',  // = 2
    'T'  // = 3
  };

  // reverse lookup table for DNA5
  static constexpr char TO_COMPLEMENT[SIZE] =
  {
    3,  // = 0
    2,  // = 1
    1,  // = 2
    0  // = 3
  };
};
struct Bits7
{
  static constexpr AlphabetSizeType SIZE = 127;
  // lookup table for XYZ
  static constexpr uint8_t FROM_ASCII[256] =
  {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     'A'     'C'             'G'
    0,  0,  0,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,
//                 'T'
    0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     'a'     'c'             'g'
    0,  0,  0,  1,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,
//                 't'
    0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  };

  // reverse lookup table for XYZ
  static constexpr char TO_ASCII[SIZE] =
  {
    'A',  // = 0
    'C',  // = 1
    'G',  // = 2
    'T'  // = 3
  };

  // reverse lookup table for DNA5
  static constexpr char TO_COMPLEMENT[SIZE] =
  {
    3,  // = 0
    2,  // = 1
    1,  // = 2
    0  // = 3
  };
};
struct Bits8
{
  static constexpr AlphabetSizeType SIZE = 255;
  // lookup table for XYZ
  static constexpr uint8_t FROM_ASCII[256] =
  {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     'A'     'C'             'G'
    0,  0,  0,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,
//                 'T'
    0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     'a'     'c'             'g'
    0,  0,  0,  1,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,
//                 't'
    0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  };

  // reverse lookup table for XYZ
  static constexpr char TO_ASCII[SIZE] =
  {
    'A',  // = 0
    'C',  // = 1
    'G',  // = 2
    'T'  // = 3
  };

  // reverse lookup table for DNA5
  static constexpr char TO_COMPLEMENT[SIZE] =
  {
    3,  // = 0
    2,  // = 1
    1,  // = 2
    0  // = 3
  };
};



// templated test function
template<typename kmer_word_type, typename input_word_type, unsigned int kmer_size=31, class Alphabet=DNA>
void test_kmer_with_word_type(input_word_type* kmer_data, uint64_t* kmer_ex, unsigned int nkmers, unsigned step=1) {

  typedef MyKmer<kmer_size, Alphabet, kmer_word_type> kmer_type;

  // create (fill) Kmer
  kmer_type kmer;

  input_word_type* kmer_pointer = kmer_data;
  // fill first kmer
  unsigned int offset = 0;
  offset = kmer.fillFromPackedStream(kmer_pointer, offset);
  kmer_type kmer_ex_0(reinterpret_cast<kmer_word_type*>(kmer_ex));
  EXPECT_EQ(kmer, kmer_ex_0) << "Kmer from stream should be equal to kmer from non-stream";


  // generate more kmers
  for (unsigned int i = step; i < nkmers; i += step)
  {
    kmer.nextFromPackedStream(kmer_pointer, offset);
    kmer_type kmer_ex_i(reinterpret_cast<kmer_word_type*>(kmer_ex+i));
    EXPECT_EQ(kmer_ex_i, kmer) << "Kmer compare unequal for sizeof(input)="<< sizeof(input_word_type) << ", sizeof(kmer_word)=" << sizeof(kmer_word_type) << ", size=" << kmer_size << ", bits=" << bliss::AlphabetTraits<Alphabet>::getBitsPerChar() << " i = " << i;
  }
}


template<typename input_word_type, unsigned int kmer_size=31, class Alphabet=DNA>
void test_kmers_with_input_type(input_word_type* kmer_data, uint64_t* kmer_ex, unsigned int nkmers, unsigned int step=1)
{
  // test with various kmer base types
  test_kmer_with_word_type<uint8_t,  input_word_type, kmer_size, DNA>(reinterpret_cast<input_word_type*>(kmer_data), kmer_ex, nkmers, step);
  test_kmer_with_word_type<uint16_t, input_word_type, kmer_size, DNA>(reinterpret_cast<input_word_type*>(kmer_data), kmer_ex, nkmers, step);
  test_kmer_with_word_type<uint32_t, input_word_type, kmer_size, DNA>(reinterpret_cast<input_word_type*>(kmer_data), kmer_ex, nkmers, step);
  test_kmer_with_word_type<uint64_t, input_word_type, kmer_size, DNA>(reinterpret_cast<input_word_type*>(kmer_data), kmer_ex, nkmers, step);
}

template<typename input_word_type>
void test_kmers(input_word_type* kmer_data, uint64_t* kmer_ex, unsigned int nkmers)
{
  // test for bits per character: 2, 4, and 8 (no padding only!)
  test_kmers_with_input_type<input_word_type, 31, DNA>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 28, DNA>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 13, DNA>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 4,  DNA>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 1,  DNA>(kmer_data, kmer_ex, nkmers);

  test_kmers_with_input_type<input_word_type, 10, Bits4>(kmer_data, kmer_ex, nkmers, 2);
  test_kmers_with_input_type<input_word_type, 13, Bits4>(kmer_data, kmer_ex, nkmers, 2);

  test_kmers_with_input_type<input_word_type, 7, Bits8>(kmer_data, kmer_ex, nkmers, 4);
  test_kmers_with_input_type<input_word_type, 5, Bits8>(kmer_data, kmer_ex, nkmers, 4);

}

template<typename input_word_type>
void test_kmers_3(input_word_type* kmer_data, uint64_t* kmer_ex, unsigned int nkmers)
{
  // maximum in 64 bits is 21
  test_kmers_with_input_type<input_word_type, 21, DNA5>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 20, DNA5>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 13, DNA5>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 9,  DNA5>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 1,  DNA5>(kmer_data, kmer_ex, nkmers);
}

template<typename input_word_type>
void test_kmers_5(input_word_type* kmer_data, uint64_t* kmer_ex, unsigned int nkmers)
{
  // maximum in 64 bits is 12
  test_kmers_with_input_type<input_word_type, 12, Bits5>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 11, Bits5>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 10, Bits5>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 9,  Bits5>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 5,  Bits5>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 3,  Bits5>(kmer_data, kmer_ex, nkmers);
  test_kmers_with_input_type<input_word_type, 1,  Bits5>(kmer_data, kmer_ex, nkmers);
}

/**
 * Test k-mer generation with 2 bits for each character
 */
TEST(KmerGeneration, TestKmerGenerationUnpadded2)
{
  // test sequence: 0xabba56781234deadbeef01c0ffee

  // expected kmers:
  // generated by the python commands (thank you python for integrated bigints)
  /*
   * val = 0xabba56781234deadbeef01c0ffee
   * print(",\n".join([" "*24 + "0x" + hex(val >> 2*i)[-16:] for i in range(0,25)]))
   */
  uint64_t kmer_ex[25] = {0xdeadbeef01c0ffee,
                          0x37ab6fbbc0703ffb,
                          0x4deadbeef01c0ffe,
                          0xd37ab6fbbc0703ff,
                          0x34deadbeef01c0ff,
                          0x8d37ab6fbbc0703f,
                          0x234deadbeef01c0f,
                          0x48d37ab6fbbc0703,
                          0x1234deadbeef01c0,
                          0x048d37ab6fbbc070,
                          0x81234deadbeef01c,
                          0xe048d37ab6fbbc07,
                          0x781234deadbeef01,
                          0x9e048d37ab6fbbc0,
                          0x6781234deadbeef0,
                          0x59e048d37ab6fbbc,
                          0x56781234deadbeef,
                          0x959e048d37ab6fbb,
                          0xa56781234deadbee,
                          0xe959e048d37ab6fb,
                          0xba56781234deadbe,
                          0xee959e048d37ab6f,
                          0xbba56781234deadb,
                          0xaee959e048d37ab6,
                          0xabba56781234dead};

  // unpadded stream (bits_per_char is 2 => no padding)
  /* python:
   * ", ".join(hex((val >> i*16) & 0xffff) for i in range(0, 8))
   */
  uint16_t kmer_data[8] = {0xffee, 0x01c0, 0xbeef, 0xdead, 0x1234, 0x5678, 0xabba, 0x0000};

  // test with this data
  test_kmers<uint16_t>(kmer_data, kmer_ex, 25);

  // NO padding, therefore we can test for different input types as well

  // 8 bit input
  test_kmers<uint8_t>(reinterpret_cast<uint8_t*>(kmer_data), kmer_ex, 25);

  // 32 bit input
  test_kmers<uint32_t>(reinterpret_cast<uint32_t*>(kmer_data), kmer_ex, 25);

  // 64 bit input
  test_kmers<uint64_t>(reinterpret_cast<uint64_t*>(kmer_data), kmer_ex, 25);
}

/**
 * Test k-mer generation with 3 bits and thus padded input
 */
TEST(KmerGeneration, TestKmerGenerationPadded3)
{
  // test sequence: 0xabba56781234deadbeef01c0ffee

  // expected kmers:
  // generated by the python commands (thank you python for integrated bigints)
  /*
   * val = 0xabba56781234deadbeef01c0ffee
   * print(",\n".join([" "*24 + "0x" + hex(val >> 3*i)[-16:] for i in range(0,17)]))
   */
  uint64_t kmer_ex[17] = {0xdeadbeef01c0ffee,
                          0x9bd5b7dde0381ffd,
                          0xd37ab6fbbc0703ff,
                          0x1a6f56df7780e07f,
                          0x234deadbeef01c0f,
                          0x2469bd5b7dde0381,
                          0x048d37ab6fbbc070,
                          0xc091a6f56df7780e,
                          0x781234deadbeef01,
                          0xcf02469bd5b7dde0,
                          0x59e048d37ab6fbbc,
                          0x2b3c091a6f56df77,
                          0xa56781234deadbee,
                          0x74acf02469bd5b7d,
                          0xee959e048d37ab6f,
                          0x5dd2b3c091a6f56d,
                          0xabba56781234dead};

  // unpadded stream (bits_per_char is 3 => 1 bit padding)
  /* python:
   * ", ".join(hex((val >> i*15) & 0x7fff) for i in range(0, 9))
   */
  uint16_t kmer_data[9] = {0x7fee, 0x381, 0x7bbc, 0x756d, 0x234d, 0x4f02, 0x6e95, 0x55, 0x0};

  // test with this data
  test_kmers_3<uint16_t>(kmer_data, kmer_ex, 17);

  /* python:
   *  ", ".join(hex((val >> i*6) & 0x3f) for i in range(0, 20))
   */
  uint8_t kmer_data_8[20] = {0x2e, 0x3f, 0xf, 0x30, 0x1, 0x3c, 0x2e, 0x2f, 0x2d, 0x3a, 0xd, 0xd, 0x12, 0x20, 0x27, 0x15, 0x3a, 0x2e, 0xa, 0x0};
  // test with 8 bit (padded by 2 bits)
  test_kmers_3<uint8_t>(kmer_data_8, kmer_ex, 17);

  /* python:
   * ", ".join(hex((val >> i*30) & 0x3fffffff) for i in range(0, 5))
   */
  // 32 bits:
  uint32_t kmer_data_32[5] = {0x1c0ffee, 0x3ab6fbbc, 0x2781234d, 0x2aee95, 0x0};
  test_kmers_3<uint32_t>(kmer_data_32, kmer_ex, 17);

  /* python:
   * ", ".join(hex((val >> i*63) & 0x7fffffffffffffff) for i in range(0, 3))
   */
  // 64 bits:
  uint64_t kmer_data_64[3] = {0x5eadbeef01c0ffee, 0x15774acf02469, 0x0};
  test_kmers_3<uint64_t>(kmer_data_64, kmer_ex, 17);
}

/**
 * Test k-mer generation with 5 bits and thus padded input
 */
TEST(KmerGeneration, TestKmerGenerationPadded5)
{
  // test sequence: 0xabba56781234deadbeef01c0ffee

  // expected kmers:
  // generated by the python commands (thank you python for integrated bigints)
  /*
   * val = 0xabba56781234deadbeef01c0ffee
   * print(",\n".join([" "*24 + "0x" + hex(val >> 5*i)[-16:] for i in range(0,11)]))
   */
  uint64_t kmer_ex[11] = {0xdeadbeef01c0ffee,
                          0xa6f56df7780e07ff,
                          0x8d37ab6fbbc0703f,
                          0x2469bd5b7dde0381,
                          0x81234deadbeef01c,
                          0x3c091a6f56df7780,
                          0x59e048d37ab6fbbc,
                          0x4acf02469bd5b7dd,
                          0xba56781234deadbe,
                          0x5dd2b3c091a6f56d,
                          0x2aee959e048d37ab};

  // unpadded stream (bits_per_char is 5 => 1 bit padding)
  /* python:
   * ", ".join(hex((val >> i*15) & 0x7fff) for i in range(0, 9))
   */
  uint16_t kmer_data[9] = {0x7fee, 0x381, 0x7bbc, 0x756d, 0x234d, 0x4f02, 0x6e95, 0x55, 0x0};

  // test with this data
  test_kmers_5<uint16_t>(kmer_data, kmer_ex, 11);

  /* python:
   *  ", ".join(hex((val >> i*5) & 0x1f) for i in range(0, 24))
   */
  uint8_t kmer_data_8[24] = {0xe, 0x1f, 0x1f, 0x1, 0x1c, 0x0, 0x1c, 0x1d, 0x1e, 0xd, 0xb, 0x1d, 0xd, 0x1a, 0x8, 0x2, 0x18, 0x13, 0x15, 0x14, 0x1b, 0x15, 0x2, 0x0};
  // test with 8 bit (padded by 3 bits)
  test_kmers_5<uint8_t>(kmer_data_8, kmer_ex, 11);

  /* python:
   * ", ".join(hex((val >> i*30) & 0x3fffffff) for i in range(0, 5))
   */
  // 32 bits (2 bits padding)
  uint32_t kmer_data_32[5] = {0x1c0ffee, 0x3ab6fbbc, 0x2781234d, 0x2aee95, 0x0};
  test_kmers_5<uint32_t>(kmer_data_32, kmer_ex, 11);

  /* python:
   * ", ".join(hex((val >> i*60) & 0x0fffffffffffffff) for i in range(0, 3))
   */
  // 64 bits (4 bits padding)
  uint64_t kmer_data_64[3] = {0xeadbeef01c0ffee, 0xabba56781234d, 0x0};
  test_kmers_5<uint64_t>(kmer_data_64, kmer_ex, 11);
}

/**
 * Testing k-mer comparison operators.
 */
TEST(KmerComparison, TestKmerComparison1)
{
  // the main kmer value
  uint16_t kmer_val[] = {0xffee, 0x1c0, 0xbeef, 0xdead, 0x1234, 0x5678, 0xabba};
  // smaller value in 4th block::
  uint16_t kmer_val_s4[] = {0xffee, 0x1c0, 0xbeef, 0x1111, 0x1234, 0x5678, 0xabba};
  // greater value in 3rd block:
  uint16_t kmer_val_g3[] = {0xffee, 0x1c0, 0xfeef, 0xdead, 0x1234, 0x5678, 0xabba};

  MyKmer<41, DNA, uint16_t> kmer(kmer_val);
  MyKmer<41, DNA, uint16_t> kmer_s(kmer_val_s4);
  MyKmer<41, DNA, uint16_t> kmer_g(kmer_val_g3);

  EXPECT_TRUE(kmer > kmer_s);
  EXPECT_TRUE(kmer == kmer);
  EXPECT_TRUE(kmer_g > kmer);
  EXPECT_FALSE(kmer_g <= kmer);
  EXPECT_TRUE(kmer <= kmer);
  EXPECT_TRUE(kmer >= kmer);
  EXPECT_FALSE(kmer < kmer);
  EXPECT_FALSE(kmer > kmer);
  EXPECT_TRUE(kmer != kmer_g);
  EXPECT_TRUE(kmer != kmer_s);

}



/**
 * Testing kmer reverse
 */
TEST(KmerReverse, TestKmerReverse112)
{
  /*
   * python code to generate the reverse (by n bits each) for a given value:
   *
   * n = 2 # or n = 3
   * b = bin(val)
   * hex(int("".join(reversed([b[i+2+((len(b)-2) % n):][:n] for i in range(0, len(b)-2, n)])),2))
   */

  // testing with 112 bit sequence
  // test sequence: val = 0xabba56781234deadbeef01c0ffee
  // n = 2:
  // reverse seq:   val = 0xbbff0340fbbe7ab71c842d95aeea
  // n = 3
  // reverse seq:   val = 0x6bff23113ebedabd34a427952faa
  // n = 4
  // reverse seq:   val = 0xeeff0c10feebdaed43218765abba
  // n = 5
  // reverse seq:   val = 0x1dff8780e77cd5f5ba40b13ad375
  // n = 7
  // reverse seq:   val = 0xddfc18ee1777d6bda6440cf2b755

  /* python:
   * ", ".join(hex((val >> i*16) & 0xffff) for i in range(0, 8))
   */
  uint16_t kmer_val[] = {0xffee, 0x1c0, 0xbeef, 0xdead, 0x1234, 0x5678, 0xabba};
  uint16_t kmer_ex[] = {0xaeea, 0x2d95, 0x1c84, 0x7ab7, 0xfbbe, 0x340, 0xbbff};
  uint16_t kmer_ex_3[] = {0x2faa, 0x2795, 0x34a4, 0xdabd, 0x3ebe, 0x2311, 0x6bff};
  uint16_t kmer_ex_4[] = {0xabba, 0x8765, 0x4321, 0xdaed, 0xfeeb, 0xc10, 0xeeff};
  uint16_t kmer_ex_5[] = {0xd375, 0xb13a, 0xba40, 0xd5f5, 0xe77c, 0x8780, 0x1dff};
  uint16_t kmer_ex_7[] = {0xb755, 0xcf2, 0xa644, 0xd6bd, 0x1777, 0x18ee, 0xddfc};


  /* test for bits_per_char = 2 */
  MyKmer<7*8, DNA, uint16_t> kmer_in(kmer_val);
  MyKmer<7*8, DNA, uint16_t> kmer_ex_rev(kmer_ex);
  // get the reverse
  MyKmer<7*8, DNA, uint16_t> kmer_rev = kmer_in.reversed_kmer();
  EXPECT_EQ(kmer_ex_rev, kmer_rev);

  /* test for bits_per_char = 3 */
  MyKmer<37, DNA5, uint16_t> kmer3_in(kmer_val);
  MyKmer<37, DNA5, uint16_t> kmer3_ex_rev(kmer_ex_3);
  // get the reverse
  MyKmer<37, DNA5, uint16_t> kmer3_rev = kmer3_in.reversed_kmer();
  EXPECT_EQ(kmer3_ex_rev, kmer3_rev);

  /* test for bits_per_char = 4 */
  MyKmer<28, Bits4, uint16_t> kmer4_in(kmer_val);
  MyKmer<28, Bits4, uint16_t> kmer4_ex_rev(kmer_ex_4);
  // get the reverse
  MyKmer<28, Bits4, uint16_t> kmer4_rev = kmer4_in.reversed_kmer();
  EXPECT_EQ(kmer4_ex_rev, kmer4_rev);

  /* test for bits_per_char = 5 */
  MyKmer<22, Bits5, uint16_t> kmer5_in(kmer_val);
  MyKmer<22, Bits5, uint16_t> kmer5_ex_rev(kmer_ex_5);
  // get the reverse
  MyKmer<22, Bits5, uint16_t> kmer5_rev = kmer5_in.reversed_kmer();
  EXPECT_EQ(kmer5_ex_rev, kmer5_rev);

  /* test for bits_per_char = 7 */
  MyKmer<16, Bits7, uint16_t> kmer7_in(kmer_val);
  MyKmer<16, Bits7, uint16_t> kmer7_ex_rev(kmer_ex_7);
  // get the reverse
  MyKmer<16, Bits7, uint16_t> kmer7_rev = kmer7_in.reversed_kmer();
  EXPECT_EQ(kmer7_ex_rev, kmer7_rev);
  // TODO: tests for smaller sizes and bigger bits_per_char

}
