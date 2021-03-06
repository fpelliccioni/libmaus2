/*
    libmaus2
    Copyright (C) 2009-2014 German Tischler
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
*/
#if ! defined(LIBMAUS2_BAMBAM_PARALLEL_DECOMPRESSEDBLOCK_HPP)
#define LIBMAUS2_BAMBAM_PARALLEL_DECOMPRESSEDBLOCK_HPP

#include <libmaus2/autoarray/AutoArray.hpp>
#include <libmaus2/bambam/parallel/InputBlock.hpp>
#include <libmaus2/bambam/parallel/MemInputBlock.hpp>
#include <libmaus2/lz/BgzfInflateZStreamBase.hpp>
#include <libmaus2/lz/BgzfInflateZStreamBaseAllocator.hpp>
#include <libmaus2/lz/BgzfInflateZStreamBaseTypeInfo.hpp>
#include <libmaus2/parallel/LockedFreeList.hpp>

namespace libmaus2
{
	namespace bambam
	{
		namespace parallel
		{
			/**
			 * class representing a decompressed bam block
			 **/
			struct DecompressedBlock
			{
				typedef DecompressedBlock this_type;
				typedef libmaus2::util::unique_ptr<this_type>::type unique_ptr_type;
				typedef libmaus2::util::shared_ptr<this_type>::type shared_ptr_type;

				//! decompressed data
				libmaus2::autoarray::AutoArray<char,libmaus2::autoarray::alloc_type_c> D;
				//! size of uncompressed data
				uint64_t uncompdatasize;
				//! next byte pointer
				char const * P;
				//! true if this is the last block in the stream
				bool final;
				//! stream id
				uint64_t streamid;
				//! block id
				uint64_t blockid;

				//! parse pointers
				libmaus2::autoarray::AutoArray<size_t,libmaus2::autoarray::alloc_type_c> PP;
				//! number of parse pointers
				size_t nPP;
				//! current parse pointer
				size_t cPP;

				size_t byteSize() const
				{
					return
						D.byteSize() +
						sizeof(uncompdatasize) +
						sizeof(P) +
						sizeof(final) +
						sizeof(streamid) +
						sizeof(blockid) +
						PP.byteSize() +
						sizeof(nPP) +
						sizeof(cPP);
				}

				char const * appendData(uint8_t const * d, size_t const c)
				{
					ptrdiff_t const o = P - D.begin();

					if ( uncompdatasize + c > D.size() )
						D.resize(uncompdatasize + c);

					P = D.begin() + o;

					std::copy(d,d+c,D.begin()+uncompdatasize);

					return D.begin() + uncompdatasize;
				}

				void pushData(uint8_t const * d, size_t const c)
				{
					size_t const nsize = uncompdatasize + c + sizeof(uint32_t);

					if ( nsize > D.size() )
					{
						ptrdiff_t const o = P - D.begin();
						D.resize(nsize);
						P = D.begin() + o;
					}

					D[uncompdatasize++] = (c >> 0) & 0xFF;
					D[uncompdatasize++] = (c >> 8) & 0xFF;
					D[uncompdatasize++] = (c >> 16) & 0xFF;
					D[uncompdatasize++] = (c >> 24) & 0xFF;
					std::copy(d,d+c,D.begin()+uncompdatasize);
					uncompdatasize += c;
				}

				void pushDataNoSize(uint8_t const * d, size_t const c)
				{
					size_t const nsize = uncompdatasize + c;

					if ( nsize > D.size() )
					{
						ptrdiff_t const o = P - D.begin();
						D.resize(nsize);
						P = D.begin() + o;
					}

					std::copy(d,d+c,D.begin()+uncompdatasize);
					uncompdatasize += c;
				}

				void pushParsePointer(char const * c)
				{
					size_t const o = c-D.begin();
					if ( nPP == PP.size() )
						PP.resize(PP.size()+1);
					assert ( nPP < PP.size() );
					PP[nPP++] = o;
				}

				char * getParsePointer(size_t i)
				{
					return D.begin() + PP[i];
				}

				char * getPrevParsePointer()
				{
					return D.begin() + PP[cPP-1];
				}

				void resetParseArray()
				{
					nPP = 0;
					cPP = 0;
				}

				size_t getNumParsePointers() const
				{
					return nPP;
				}

				char const * getNextParsePointer()
				{
					return D.begin() + PP[cPP++];
				}

				DecompressedBlock()
				:
					D(libmaus2::lz::BgzfConstants::getBgzfMaxBlockSize(),false),
					uncompdatasize(0),
					P(0),
					final(false),
					streamid(0),
					blockid(0),
					PP(0),
					nPP(0)
				{}

				/**
				 * computes and returns the crc32 of the block
				 **/
				uint32_t computeCrc(libmaus2::lz::BgzfInflateZStreamBase * decoder) const
				{
					uint32_t crc = decoder->zintf->z_crc32(0L,Z_NULL,0);
					crc = decoder->zintf->z_crc32(crc,reinterpret_cast<Bytef const*>(P),uncompdatasize);
					return crc;
				}

				/**
				 * computes and returns the crc32 of the block
				 **/
				uint32_t computeCrc(
					libmaus2::parallel::LockedFreeList<
						libmaus2::lz::BgzfInflateZStreamBase,
						libmaus2::lz::BgzfInflateZStreamBaseAllocator,
						libmaus2::lz::BgzfInflateZStreamBaseTypeInfo
						> & deccont
				)
				{
					libmaus2::lz::BgzfInflateZStreamBase::shared_ptr_type decoder = deccont.get();
					uint32_t const crc = computeCrc(decoder.get());
					deccont.put(decoder);
					return crc;
				}

				uint64_t decompressBlock(
					libmaus2::lz::BgzfInflateZStreamBase * decoder,
					char * in,
					unsigned int const inlen,
					unsigned int const outlen,
					unsigned int const outoff = 0
				)
				{
					assert ( outoff + outlen <= D.size() );
					decoder->zdecompress(reinterpret_cast<uint8_t *>(in),inlen,D.begin()+outoff,outlen);
					return outlen;
				}

				uint64_t decompressBlock(
					libmaus2::lz::BgzfInflateZStreamBase * decoder,
					uint8_t * in,
					unsigned int const inlen,
					unsigned int const outlen,
					unsigned int const outoff = 0
				)
				{
					assert ( outoff + outlen <= D.size() );
					decoder->zdecompress(in,inlen,D.begin()+outoff,outlen);
					return outlen;
				}

				uint64_t decompressBlock(
					libmaus2::lz::BgzfInflateZStreamBase * decoder,
					InputBlock * inblock,
					unsigned int const outoff = 0
				)
				{
					uncompdatasize = inblock->uncompdatasize;
					final = inblock->final;
					P = D.begin();
					return decompressBlock(decoder,inblock->C.begin(),inblock->payloadsize,uncompdatasize,outoff);
				}

				uint64_t decompressBlock(
					libmaus2::lz::BgzfInflateZStreamBase * decoder,
					MemInputBlock * inblock,
					unsigned int const outoff = 0
				)
				{
					uncompdatasize = inblock->uncompdatasize;
					final = inblock->final;
					P = D.begin();
					return decompressBlock(decoder,inblock->C,inblock->payloadsize,uncompdatasize,outoff);
				}

				uint64_t decompressBlock(
					libmaus2::parallel::LockedFreeList<
						libmaus2::lz::BgzfInflateZStreamBase,
						libmaus2::lz::BgzfInflateZStreamBaseAllocator,
						libmaus2::lz::BgzfInflateZStreamBaseTypeInfo
						> & deccont,
					char * in,
					unsigned int const inlen,
					unsigned int const outlen,
					unsigned int const outoff = 0
				)
				{
					libmaus2::lz::BgzfInflateZStreamBase::shared_ptr_type decoder = deccont.get();
					uint64_t const r = decompressBlock(decoder.get(),in,inlen,outlen,outoff);
					deccont.put(decoder);
					return r;
				}

				uint64_t decompressBlock(
					libmaus2::parallel::LockedFreeList<
						libmaus2::lz::BgzfInflateZStreamBase,
						libmaus2::lz::BgzfInflateZStreamBaseAllocator,
						libmaus2::lz::BgzfInflateZStreamBaseTypeInfo
						> & deccont,
					uint8_t * in,
					unsigned int const inlen,
					unsigned int const outlen,
					unsigned int const outoff = 0
				)
				{
					libmaus2::lz::BgzfInflateZStreamBase::shared_ptr_type decoder = deccont.get();
					uint64_t const r = decompressBlock(decoder.get(),in,inlen,outlen,outoff);
					deccont.put(decoder);
					return r;
				}

				uint64_t decompressBlock(
					libmaus2::parallel::LockedFreeList<
						libmaus2::lz::BgzfInflateZStreamBase,
						libmaus2::lz::BgzfInflateZStreamBaseAllocator,
						libmaus2::lz::BgzfInflateZStreamBaseTypeInfo
						> & deccont,
					InputBlock & inblock
				)
				{
					uncompdatasize = inblock.uncompdatasize;
					final = inblock.final;
					P = D.begin();
					return decompressBlock(deccont,inblock.C.begin(),inblock.payloadsize,uncompdatasize);
				}

				uint64_t decompressBlock(
					libmaus2::parallel::LockedFreeList<
						libmaus2::lz::BgzfInflateZStreamBase,
						libmaus2::lz::BgzfInflateZStreamBaseAllocator,
						libmaus2::lz::BgzfInflateZStreamBaseTypeInfo
						> & deccont,
					MemInputBlock & inblock
				)
				{
					uncompdatasize = inblock.uncompdatasize;
					final = inblock.final;
					P = D.begin();
					return decompressBlock(deccont,inblock.C,inblock.payloadsize,uncompdatasize);
				}
			};
		}
	}
}
#endif
