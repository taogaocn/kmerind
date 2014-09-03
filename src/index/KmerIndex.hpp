/**
 * @file    KmerIndex.hpp
 * @ingroup
 * @author  tpan
 * @brief
 * @details
 *
 * Copyright (c) 2014 Georgia Institute of Technology.  All Rights Reserved.
 *
 * TODO add License
 */
#ifndef KMERINDEX_HPP_
#define KMERINDEX_HPP_

#if defined(USE_MPI)
#include "mpi.h"
#endif

#if defined(USE_OPENMP)
#include "omp.h"
#endif

#include <unistd.h>     // sysconf
#include <sys/stat.h>   // block size.

#include "common/base_types.hpp"
#include "common/AlphabetTraits.hpp"
#include "io/fastq_loader.hpp"
#include "index/kmer_index_element.hpp"
#include "index/kmer_index_functors.hpp"
#include "index/kmer_index_generator.hpp"
#include "io/CommunicationLayer.hpp"
#include "index/distributed_map.hpp"

namespace bliss
{
  namespace index
  {

    /**
     * @class    bliss::index::KmerIndex
     * @brief    base type  for Kmer Index
     * @details
     *
     */
    template<unsigned int Kmer_Size, typename Alphabet, typename FileFormat = bliss::io::FASTQ>
    class KmerPositionIndex {

    };

    template<unsigned int KmerSize, typename Alphabet, typename QualityType = float, typename FileFormat = bliss::io::FASTQ>
    class KmerPositionAndQualityIndex
    {

      protected:
        static_assert(std::is_floating_point<QualityType>::value, "QualityType should be floating point (float or double).");

        //==================== COMMON TYPES

        /// DEFINE kmer index type, also used for reverse complement
        using KmerType = bliss::Kmer<KmerSize, bliss::AlphabetTraits<Alphabet>::BITS_PER_CHAR>;

        /// DEFINE file loader.  this only provides the L1 and L2 blocks, not reads.
        // raw data type :  use CharType
        using FileLoaderType = bliss::io::FASTQLoader<CharType, false, true>;

        // from FileLoader type, get the block iter type and range type
        using FileBlockIterType = typename FileLoaderType::L2BlockType::iterator;
        using RangeType = typename FileLoaderType::RangeType;

        /// DEFINE the iterator parser to get fastq records.  this determines both the file format and whether quality scoring is used or not.
        using ParserType = bliss::io::FASTQParser<FileBlockIterType, QualityType>;

        // once have ParserType, then get the sequence type and sequence id type
        using SeqType = typename ParserType::SequenceType;
        using IdType = typename SeqType::IdType;

        /// DEFINE the transform iterator type for parsing the FASTQ file into sequence records.
        using SeqIterType = bliss::io::SequencesIterator<ParserType>;


        //==========below are to be redefined

        /// define kmer quality GENERATOR types
        typedef bliss::index::SangerToLogProbCorrect<QualityType>               QualityEncoderType;
        typedef bliss::index::generate_qual<SeqType, KmerSize, QualityEncoderType > QualOpType;


        ///////////// INDEX TYPES
        /// kmer index element type and corresponding generator type
        typedef bliss::index::KmerIndexElementWithIdAndQuality<KmerType, IdType, QualityType>   KmerIndexValueType;
        typedef bliss::index::generate_kmer<SeqType, KmerIndexValueType>                                  KmerOpType;




        /// define the index storage type
        typedef bliss::index::distributed_multimap<KmerType, KmerIndexValueType, bliss::io::CommunicationLayer> IndexType;
        /// define the KmerIndexElement Generator type.
        typedef bliss::index::KmerIndexGeneratorWithQuality<KmerOpType, IndexType, QualOpType>            KmerIndexComputeType;

        /// actual index to store data
        IndexType index;

        /// MPI communicator used by index and fileloader.
        MPI_Comm comm;

        /// MPI rank within the communicator
        int rank;

      public:
        /**
         * initializes the index
         * @param comm
         * @param comm_size
         */
        KmerPositionAndQualityIndex(MPI_Comm _comm, int _comm_size) : index(_comm, _comm_size), comm(_comm) {
          MPI_Comm_rank(comm, &rank);
        };
        virtual ~KmerPositionAndQualityIndex() {};

        /**
         * build index, default to num of threads = system PAGE SIZE
         * @param filename
         * @param nthreads
         */
        void build(const std::string &filename, const int &nthreads) {
           build(filename, nthreads, sysconf(_SC_PAGE_SIZE));
        }

        /**
         * @brief build index from a file.
         * @param filename
         * @param nthreads
         * @param chunkSize
         */
        void build(const std::string &filename, const int &nthreads, const int chunkSize) {

//
//          std::chrono::high_resolution_clock::time_point t1, t2;
//          std::chrono::duration<double> time_span;


          //////////////// now partition and open the file.
          // call "load" with the partition range.


          // get the file ready for read
          {
//            t1 = std::chrono::high_resolution_clock::now();
            // create FASTQ Loader

            FileLoaderType loader(comm, filename, nthreads, chunkSize);  // this handle is alive through the entire execution.

            //===  repeatedly load the next L1 Block.
            while (loader.getNextL1Block().getRange().size() > 0) {


//            t2 = std::chrono::high_resolution_clock::now();
//            time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
//            std::cout << "MMap rank " << rank << " elapsed time: " << time_span.count() << "s." << std::endl;

//            std::cout << rank << " file partition: " << loader.getCurrentL1Block().getRange() << std::endl;



              //====  now process the file .
//              t1 = std::chrono::high_resolution_clock::now();
              buildForL1Block(loader, index, rank, nthreads, chunkSize);

//            t2 = std::chrono::high_resolution_clock::now();
//            time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
//            std::cout << "Compute rank " << rank << " elapsed time: " << time_span.count() << "s." << std::endl;

            }
          }  // scope to ensure file loader is destroyed.

          printf("MPI number of entries in index for rank %d is %lu\n", rank, index.local_size());
        }


        //============== what makes sense as return types for results?


      protected:

        void buildForL1Block(FileLoaderType &loader, IndexType &index, const int& rank,
                        const int& nthreads, const int& chunkSize)
        {
//          std::chrono::high_resolution_clock::time_point t1, t2;
//          std::chrono::duration<double> time_span;
//          t1 = std::chrono::high_resolution_clock::now();
//
//          INFO("buildIndex rank.tid = " << rank << "." << omp_get_thread_num());

          // local variables for information only.
          int nReads = 0;    // read count
          int nChunks = 0;    // chunk count

          // uses the fastq iterator as the queue itself, instead of master/slave.
          //   at this point, no strong difference.
          #pragma omp parallel num_threads(nthreads) shared(loader, nthreads, index, rank) reduction(+:nReads, nChunks) OMP_SHARE_DEFAULT
          {
            //== instantiate a local parser
            ParserType parser;

            int tid = 0;
            #ifdef USE_OPENMP
                tid = omp_get_thread_num();
            #endif

            //== instantiate a local kmer generator
            KmerIndexComputeType op;

            //== local variables for loop
            typename FileLoaderType::L2BlockType chunk;
            SeqType read;

            //== initialize the loop by getting the first chunk
            chunk = loader.getNextL2Block(tid);
            while (chunk.getRange().size() > 0) {
              //== get the chunk of data

              //==  and wrap the chunk inside an iterator that emits Reads.
              SeqIterType fastq_start(parser, chunk.begin(), chunk.end(), chunk.getRange().start);
              SeqIterType fastq_end(chunk.end());

              //== loop over the reads
              for (; fastq_start != fastq_end; ++fastq_start)
              {
                // first get read
                read = *fastq_start;

                // then compute and store into index (this will generate kmers and insert into index)
                op(read, index);
                ++nReads;

                // do a little status print.
                if (nReads % 20000 == 0)
                  INFO("buildIndex rank.tid=" << rank << "." <<  tid << " nReads=" << nReads);
              }

              //== get read for next loop iteration.
              chunk = loader.getNextL2Block(tid);
              ++nChunks;
            }

            INFO("buildIndex rank.tid=" << rank << "." << tid << " nChunks=" << nChunks);

          }  // compute threads parallel

//          t2 = std::chrono::high_resolution_clock::now();
//          time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
//          INFO("buildIndex rank=" << rank << " nReads=" << nReads << " nChunks=" << nChunks << " elapsed time: " << time_span.count() << "s.");

//          /// this MPI process is done.  now flush the index to all other nodes.
//          t1 = std::chrono::high_resolution_clock::now();
          index.flush();
//          t2 = std::chrono::high_resolution_clock::now();
//          time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
//          INFO("flushIndex rank=" << rank << " elapsed time: " << time_span.count() << "s.");

        }


    };



//    ////////////// COUNT TYPES
//    /// kmer index element type and corresponding generator type
//    typedef bliss::index::KmerIndexElement<KmerType>                                                 KmerCountType;
//    typedef bliss::index::generate_kmer<SeqType, KmerCountType>                                       KmerCountOpType;
//
//    /// count indices
//    typedef bliss::index::distributed_counting_map<KmerType, bliss::io::CommunicationLayer>           CountIndexType;
//    /// define the KmerIndexElement Generator for counting maptype.
//    typedef bliss::index::KmerIndexGenerator<KmerCountOpType, CountIndexType>                         KmerCountComputeType;



  } /* namespace index */
} /* namespace bliss */

#endif /* KMERINDEX_HPP_ */