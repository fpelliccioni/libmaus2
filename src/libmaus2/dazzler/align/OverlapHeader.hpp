/*
    libmaus2
    Copyright (C) 2017 German Tischler

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
#if ! defined(LIBMAUS2_DAZZLER_ALIGN_OVERLAPHEADER_HPP)
#define LIBMAUS2_DAZZLER_ALIGN_OVERLAPHEADER_HPP

#include <libmaus2/types/types.hpp>
#include <ostream>

namespace libmaus2
{
	namespace dazzler
	{
		namespace align
		{
			struct OverlapHeader
			{
				bool inv;
				int64_t aread;
				int64_t bread;
				int64_t abpos;
				int64_t aepos;
				int64_t bbpos;
				int64_t bepos;
				int64_t diffs;

				OverlapHeader() {}
			};

			std::ostream & operator<<(std::ostream & out, OverlapHeader const & O);
		}
	}
}
#endif
