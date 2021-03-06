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
#if ! defined(LIBMAUS2_BAMBAM_PARALLEL_DECOMPRESSEDPENDINGOBJECT_HPP)
#define LIBMAUS2_BAMBAM_PARALLEL_DECOMPRESSEDPENDINGOBJECT_HPP

#include <libmaus2/bambam/parallel/DecompressedBlock.hpp>
#include <map>

namespace libmaus2
{
	namespace bambam
	{
		namespace parallel
		{
			struct DecompressedPendingObject : std::pair<uint64_t, DecompressedBlock::shared_ptr_type>
			{
				typedef std::pair<uint64_t, DecompressedBlock::shared_ptr_type> base_type;

				DecompressedPendingObject() : base_type(0,DecompressedBlock::shared_ptr_type()) {}
				DecompressedPendingObject(
					uint64_t const rid,
					DecompressedBlock::shared_ptr_type robj
				) : base_type(rid,robj) {}
			};
		}
	}
}
#endif
