/*
    libmaus2
    Copyright (C) 2016 German Tischler

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
#if ! defined(LIBMAUS2_LCS_ALIGNMENTONEAGAINSTMANYAVX2_HPP)
#define LIBMAUS2_LCS_ALIGNMENTONEAGAINSTMANYAVX2_HPP

#include <libmaus2/util/Destructable.hpp>
#include <libmaus2/autoarray/AutoArray.hpp>
#if ! defined(LIBMAUS2_HAVE_ALIGNMENT_ONE_TO_MANY_AVX2)
#include <libmaus2/lcs/NP.hpp>
#endif
#include <libmaus2/lcs/AlignmentOneAgainstManyInterface.hpp>

namespace libmaus2
{
	namespace lcs
	{
		struct AlignmentOneAgainstManyAVX2 : public libmaus2::lcs::AlignmentOneAgainstManyInterface
		{
			libmaus2::util::Destructable::unique_ptr_type context;


			#if defined(LIBMAUS2_HAVE_ALIGNMENT_ONE_TO_MANY_AVX2)
			libmaus2::autoarray::AutoArray < std::pair<uint64_t,uint64_t> > Vlen;
			#else
			libmaus2::lcs::NP np;
			#endif

			AlignmentOneAgainstManyAVX2();

			void process(
				uint8_t const * qa, uint8_t const * qe,
				std::pair<uint8_t const *,uint64_t> const * MA,uint64_t const MAo,
				libmaus2::autoarray::AutoArray<uint64_t> & E
			);
		};
	}
}
#endif
