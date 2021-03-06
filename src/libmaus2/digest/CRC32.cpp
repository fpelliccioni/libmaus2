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
#include <libmaus2/digest/CRC32.hpp>

#if defined(LIBMAUS2_HAVE_x86_64)
#include <libmaus2/digest/CRC32_Core.hpp>
#endif

#include <zlib.h>

libmaus2::digest::CRC32::CRC32() : zintf(libmaus2::lz::ZlibInterface::construct()), initial(zintf->z_crc32(0L, Z_NULL, 0)), ctx(0) {}
libmaus2::digest::CRC32::~CRC32() {}

void libmaus2::digest::CRC32::init() { ctx = initial; }
void libmaus2::digest::CRC32::update(uint8_t const * t, size_t l)
{
#if defined(LIBMAUS2_HAVE_x86_64)
	ctx = libmaus2::digest::CRC32_Core::crc32_core(ctx,t,l);
#else
	ctx = zintf->z_crc32(ctx,t,l);
#endif
}
void libmaus2::digest::CRC32::digest(uint8_t * digest)
{
	digest[0] = (ctx >> 24) & 0xFF;
	digest[1] = (ctx >> 16) & 0xFF;
	digest[2] = (ctx >>  8) & 0xFF;
	digest[3] = (ctx >>  0) & 0xFF;
}

void libmaus2::digest::CRC32::copyFrom(CRC32 const & O)
{
	ctx = O.ctx;
}

void libmaus2::digest::CRC32::vinit() { init(); }
void libmaus2::digest::CRC32::vupdate(uint8_t const * u, size_t l) { update(u,l); }
