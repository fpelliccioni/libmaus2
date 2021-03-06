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
#if ! defined(LIBMAUS2_FM_KMERTUPLE_HPP)
#define LIBMAUS2_FM_KMERTUPLE_HPP

#include <libmaus2/types/types.hpp>

#if defined(LIBMAUS2_HAVE_UNSIGNED_INT128)
namespace libmaus2
{
	namespace alignment
	{
		struct KmerTuple
		{
			libmaus2::uint128_t kmer;
			libmaus2::uint128_t shiftkmer;
			uint64_t pos;

			KmerTuple() : kmer(0), shiftkmer(0), pos(0) {}
			KmerTuple(
				libmaus2::uint128_t rkmer,
				libmaus2::uint128_t rshiftkmer,
				uint64_t rpos
			) : kmer(rkmer), shiftkmer(rshiftkmer), pos(rpos) {}

			bool operator<(KmerTuple const & O) const
			{
				if ( kmer != O.kmer )
					return kmer < O.kmer;
				else
					return pos < O.pos;
			}
		};
	}
}
#else
#error "libmaus2::alignment::KmerTuple requires 128 bit integer which is not supported by the compiler/system."
#endif

#endif
