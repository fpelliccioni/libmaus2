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

#include <libmaus2/lz/RAZFIndex.hpp>

std::ostream & libmaus2::lz::operator<<(std::ostream & out, libmaus2::lz::RAZFIndex const & index)
{
	out << "uncompressed size " << index.uncompressed << std::endl;
	out << "compressed size " << index.compressed << std::endl;
	out << "header length " << index.headerlength << std::endl;

	for ( uint64_t i = 0; i < index.binoffsets.size(); ++i )
		out << "binoffsets[" << i << "]=" << index.binoffsets[i] << std::endl;

	for ( uint64_t i = 0; i < index.celloffsets.size(); ++i )
		out << "celloffsets[" << i << "]=" << index.celloffsets[i] << std::endl;

	return out;
}
