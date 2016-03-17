/**
    libmaus2
    Copyright (C) 2009-2016 German Tischler
    Copyright (C) 2011-2014 Genome Research Limited

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/
#if ! defined(LIBMAUS2_SUFFIXSORT_BWTB3M_BWTMERGEOPTIONS_HPP)
#define LIBMAUS2_SUFFIXSORT_BWTB3M_BWTMERGEOPTIONS_HPP

#include <libmaus2/util/ArgInfo.hpp>
#include <libmaus2/parallel/NumCpus.hpp>
#include <libmaus2/util/OutputFileNameTools.hpp>
#include <libmaus2/math/numbits.hpp>

namespace libmaus2
{
	namespace suffixsort
	{
		namespace bwtb3m
		{
			struct BwtMergeSortOptions
			{
				enum bwt_merge_input_type {
					bwt_merge_input_type_bytestream,
					bwt_merge_input_type_compactstream,
					bwt_merge_input_type_pac,
					bwt_merge_input_type_pacterm,
					bwt_merge_input_type_lz4,
					bwt_merge_input_type_utf_8
				};

				uint64_t numthreads;
				std::string fn;
				uint64_t wordsperthread;
				bool bwtonly;
				uint64_t mem;
				std::string tmpfilenamebase;
				std::string sparsetmpfilenamebase;
				uint64_t isasamplingrate;
				uint64_t sasamplingrate;
				bool copyinputtomemory;
				bool computeTermSymbolHwt;
				uint64_t maxblocksize;
				uint64_t maxpreisasamplingrate;
				std::string defoutfn;
				std::string outfn;
				std::string sinputtype;
				bwt_merge_input_type inputtype;

				static bwt_merge_input_type parseInputType(std::string const & sinputtype)
				{
					typedef std::pair<char const *, bwt_merge_input_type> pair_type;
					pair_type valid[] = {
						std::pair<char const *, bwt_merge_input_type>("bytestream",bwt_merge_input_type_bytestream),
						std::pair<char const *, bwt_merge_input_type>("compactstream",bwt_merge_input_type_compactstream),
						std::pair<char const *, bwt_merge_input_type>("pac",bwt_merge_input_type_pac),
						std::pair<char const *, bwt_merge_input_type>("pacterm",bwt_merge_input_type_pacterm),
						std::pair<char const *, bwt_merge_input_type>("lz4",bwt_merge_input_type_lz4),
						std::pair<char const *, bwt_merge_input_type>("utf-8",bwt_merge_input_type_utf_8),
						std::pair<char const *, bwt_merge_input_type>(0,bwt_merge_input_type_bytestream),
					};

					for ( pair_type const * p = &valid[0]; p->first; ++p )
						if ( std::string(p->first) == sinputtype )
							return p->second;

					libmaus2::exception::LibMausException lme;
					lme.getStream() << "libmaus2::suffixsort::bwtb3m::BwtMergeSortOptions::parseInputType: unknown input type " << sinputtype << std::endl;
					lme.finish();
					throw lme;
				}

				static std::string getDefaultInputType()
				{
					return "bytestream";
				}

				static uint64_t getDefaultSaSamplingRate()
				{
					return 32;
				}

				static uint64_t getDefaultIsaSamplingRate()
				{
					return 256*1024;
				}

				static bool getDefaultCopyInputToMemory()
				{
					return false;
				}

				static bool getDefaultBWTOnly()
				{
					return false;
				}

				static uint64_t getDefaultWordsPerThread()
				{
					return 64ull*1024ull;
				}

				static uint64_t getDefaultMem()
				{
					return 2ull * 1024ull * 1024ull * 1024ull;
				}
				
				static uint64_t getDefaultNumThreads()
				{
					return libmaus2::parallel::NumCpus::getNumLogicalProcessors();
				}

				static std::string computeDefaultOutputFileName(std::string const & fn)
				{
					std::vector<std::string> endClipS;
					endClipS.push_back(".txt");
					endClipS.push_back(".compact");
					libmaus2::autoarray::AutoArray<char const *> endClipC(endClipS.size()+1);
					for ( uint64_t i = 0; i < endClipS.size(); ++i )
						endClipC[i] = endClipS[i].c_str();
					endClipC[endClipS.size()] = 0;
					std::string const defoutfn = libmaus2::util::OutputFileNameTools::endClip(fn,endClipC.begin()) + ".bwt";
					return defoutfn;	
				}
				
				BwtMergeSortOptions() {}
				
				BwtMergeSortOptions(libmaus2::util::ArgInfo const & arginfo) :
				  numthreads(arginfo.getValueUnsignedNumeric<unsigned int>("numthreads", getDefaultNumThreads())),
				  fn(arginfo.getUnparsedRestArg(0)),
				  wordsperthread(std::max(static_cast<uint64_t>(1),arginfo.getValueUnsignedNumeric<uint64_t>("wordsperthread",getDefaultWordsPerThread()))),
				  bwtonly(arginfo.getValue<unsigned int>("bwtonly",getDefaultBWTOnly())),
				  mem(std::max(static_cast<uint64_t>(1),arginfo.getValueUnsignedNumeric<uint64_t>("mem",getDefaultMem()))),
				  tmpfilenamebase(arginfo.getUnparsedValue("tmpprefix",arginfo.getDefaultTmpFileName())),
				  sparsetmpfilenamebase(arginfo.getUnparsedValue("sparsetmpprefix",tmpfilenamebase)),
				  isasamplingrate(::libmaus2::math::nextTwoPow(arginfo.getValueUnsignedNumeric<uint64_t>("isasamplingrate",getDefaultIsaSamplingRate()))),
				  sasamplingrate(::libmaus2::math::nextTwoPow(arginfo.getValueUnsignedNumeric<uint64_t>("sasamplingrate",getDefaultSaSamplingRate()))),
				  copyinputtomemory(arginfo.getValue<uint64_t>("copyinputtomemory",getDefaultCopyInputToMemory())),
				  computeTermSymbolHwt(arginfo.getValue<int>("computeTermSymbolHwt",false)),
				  maxblocksize(arginfo.getValueUnsignedNumeric<uint64_t>("maxblocksize", std::numeric_limits<uint64_t>::max())),
				  maxpreisasamplingrate(::libmaus2::math::nextTwoPow(arginfo.getValueUnsignedNumeric<uint64_t>("preisasamplingrate",bwtonly ? 64 : 256*1024))),
				  defoutfn(computeDefaultOutputFileName(computeDefaultOutputFileName(fn))),
				  outfn(arginfo.getValue<std::string>("outputfilename",defoutfn)),
				  sinputtype(arginfo.getValue<std::string>("inputtype",getDefaultInputType())),
				  inputtype(parseInputType(sinputtype))
				{
				
				}
			};
		}
	}
}
#endif
