/*
    libmaus2
    Copyright (C) 2015 German Tischler

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
#if ! defined(LIBMAUS2_DAZZLER_DB_TRACKANNOINTERFACE_HPP)
#define LIBMAUS2_DAZZLER_DB_TRACKANNOINTERFACE_HPP

#include <libmaus2/util/unique_ptr.hpp>
#include <libmaus2/util/shared_ptr.hpp>
#include <libmaus2/types/types.hpp>

namespace libmaus2
{
	namespace dazzler
	{
		namespace db
		{
			struct TrackAnnoInterface
			{
				typedef TrackAnnoInterface this_type;
				typedef libmaus2::util::unique_ptr<this_type>::type unique_ptr_type;
				typedef libmaus2::util::shared_ptr<this_type>::type shared_ptr_type;

				virtual ~TrackAnnoInterface() {}
				virtual uint64_t operator[](uint64_t const i) const = 0;
				virtual void shift(uint64_t const s) = 0;
				virtual uint64_t size() const = 0;
			};
		}
	}
}
#endif
