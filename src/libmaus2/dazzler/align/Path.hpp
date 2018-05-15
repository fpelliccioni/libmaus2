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
#if ! defined(LIBMAUS_DAZZLER_ALIGN_PATH_HPP)
#define LIBMAUS_DAZZLER_ALIGN_PATH_HPP

#include <libmaus2/dazzler/db/InputBase.hpp>
#include <libmaus2/dazzler/db/OutputBase.hpp>
#include <libmaus2/math/IntegerInterval.hpp>
#include <libmaus2/autoarray/AutoArray.hpp>
#include <libmaus2/util/utf8.hpp>
#include <utility>
#include <cassert>

namespace libmaus2
{
	namespace dazzler
	{
		namespace align
		{
			struct Path : public libmaus2::dazzler::db::InputBase, public libmaus2::dazzler::db::OutputBase
			{
				typedef std::pair<uint16_t,uint16_t> tracepoint;

				std::vector<tracepoint> path;
				int32_t tlen;
				int32_t diffs;
				int32_t abpos;
				int32_t bbpos;
				int32_t aepos;
				int32_t bepos;

				uint64_t simpleSerialise(std::ostream & out) const
				{
					uint64_t l = 0;
					l += libmaus2::util::UTF8::encodeUTF8(path.size(),out);
					for ( uint64_t i = 0; i < path.size(); ++i )
					{
						l += libmaus2::util::UTF8::encodeUTF8(path[i].first,out);
						l += libmaus2::util::UTF8::encodeUTF8(path[i].second,out);
					}
					l += libmaus2::util::UTF8::encodeUTF8(tlen,out);
					l += libmaus2::util::UTF8::encodeUTF8(diffs,out);
					l += libmaus2::util::UTF8::encodeUTF8(abpos,out);
					l += libmaus2::util::UTF8::encodeUTF8(aepos,out);
					l += libmaus2::util::UTF8::encodeUTF8(bbpos,out);
					l += libmaus2::util::UTF8::encodeUTF8(bepos,out);

					return l;
				}

				uint64_t simpleDeserialise(std::istream & in)
				{
					uint64_t l = 0;
					uint64_t const n = libmaus2::util::UTF8::decodeUTF8(in,l);
					path.resize(n);
					for ( uint64_t i = 0; i < path.size(); ++i )
					{
						path[i].first = libmaus2::util::UTF8::decodeUTF8(in,l);
						path[i].second = libmaus2::util::UTF8::decodeUTF8(in,l);
					}
					tlen = libmaus2::util::UTF8::decodeUTF8(in,l);
					diffs = libmaus2::util::UTF8::decodeUTF8(in,l);
					abpos = libmaus2::util::UTF8::decodeUTF8(in,l);
					aepos = libmaus2::util::UTF8::decodeUTF8(in,l);
					bbpos = libmaus2::util::UTF8::decodeUTF8(in,l);
					bepos = libmaus2::util::UTF8::decodeUTF8(in,l);

					return l;
				}

				libmaus2::math::IntegerInterval<int64_t> aint() const
				{
					return libmaus2::math::IntegerInterval<int64_t>(abpos,aepos-1);
				}

				libmaus2::math::IntegerInterval<int64_t> bint() const
				{
					return libmaus2::math::IntegerInterval<int64_t>(bbpos,bepos-1);
				}

				static unsigned int binshift()
				{
					return 8;
				}

				static void binreduce(int64_t & rl, int64_t & abpos, int64_t & aepos)
				{
					for ( unsigned int i = 0; i < binshift(); ++i )
					{
						rl    = (rl   +1)>>1;
						abpos = (abpos+0)>>1;
						aepos = (aepos+1)>>1;
					}
				}

				static uint64_t getNumBins(int64_t rl)
				{
					assert ( rl );

					int64_t abpos = 0;
					int64_t aepos = 0;

					binreduce(rl,abpos,aepos);

					uint64_t n = 0;
					while ( rl > 1 )
					{
						n += rl;
						rl = (rl+1)>>1;
					}

					assert ( rl == 1 );
					n += rl;

					return n;
				}

				static uint64_t getBinList(libmaus2::autoarray::AutoArray < std::pair<uint64_t,uint64_t> > & A, int64_t rl, int64_t abpos, int64_t aepos)
				{
					uint64_t o = 0;

					assert ( abpos <= aepos );
					assert ( aepos <= rl );

					binreduce(rl,abpos,aepos);

					if ( rl )
					{
						uint64_t offset = 0;

						while ( rl > 1 )
						{
							A.push(o,std::pair<uint64_t,uint64_t>(offset + abpos,offset + aepos));
							// round down
							abpos = (abpos+0)>>1;
							// round up
							aepos = (aepos+1)>>1;
							// update offset
							offset += rl;
							// round up
							rl = (rl+1)>>1;
						}

						assert ( rl == 1 );

						A.push(o,std::pair<uint64_t,uint64_t>(offset+abpos,offset+aepos));
					}

					return o;
				}

				static uint64_t getBin(int64_t rl, int64_t abpos, int64_t aepos)
				{
					assert ( rl );
					assert ( abpos < aepos );
					assert ( aepos <= rl );

					binreduce(rl,abpos,aepos);

					assert ( rl );
					assert ( abpos < aepos );
					assert ( aepos <= rl );

					int64_t offset = 0;

					while ( aepos-abpos > 1 )
					{
						offset += rl;

						// round down
						abpos = (abpos+0)>>1;
						// round up
						aepos = (aepos+1)>>1;
						// round up
						rl = (rl+1)>>1;
					}

					return abpos + offset;
				}

				uint64_t getBinList(libmaus2::autoarray::AutoArray < std::pair<uint64_t,uint64_t> > & A, int64_t const rl) const
				{
					return getBinList(A,rl,abpos,aepos);
				}

				uint64_t getBin(int64_t rl) const
				{
					return getBin(rl,abpos,aepos);
				}

				int32_t getBBlockOffset(uint64_t const i) const
				{
					uint64_t o = 0;

					for ( uint64_t j = 0; j < i; ++j )
						o += path[j].second;

					return o;
				}

				double getErrorRate() const
				{
					return (aepos > abpos) ? (static_cast<double>(diffs) / static_cast<double>(aepos-abpos)) : 0.0;
				}

				Path filter(std::pair<int32_t,int32_t> const & I, int64_t const tspace) const
				{
					assert ( I.first % tspace == 0 );
					assert ( I.second % tspace == 0 );

					Path P;

					assert ( I.first >= abpos );
					assert ( I.second <= aepos );

					uint64_t numskip = 0;
					P.abpos = abpos;
					P.bbpos = bbpos;

					for ( ; P.abpos < I.first; numskip++ )
					{
						P.bbpos += path[numskip].second;

						if ( P.abpos % tspace == 0 )
							P.abpos += tspace;
						else
							P.abpos = ( (P.abpos + tspace - 1) / tspace ) * tspace;
					}

					P.diffs = 0;
					P.aepos = P.abpos;
					P.bepos = P.bbpos;

					for ( ; P.aepos < I.second; numskip++ )
					{
						P.aepos += tspace;
						P.bepos += path[numskip].second;
						P.diffs += path[numskip].first;
						P.path.push_back(path[numskip]);
					}

					P.tlen = P.path.size() * 2;

					assert ( (P.bepos-P.bbpos) == static_cast<int64_t>(P.getBSpan()) );

					return P;
				}

				bool checkBSpan() const
				{
					return
						(bepos-bbpos) == static_cast<int64_t>(getBSpan());
				}

				bool isEmpty() const
				{
					return abpos == aepos;
				}

				uint64_t getBSpan() const
				{
					uint64_t b = 0;
					for ( uint64_t i = 0; i < path.size(); ++i )
						b += path[i].second;
					return b;
				}

				uint64_t getErrorSum() const
				{
					uint64_t b = 0;
					for ( uint64_t i = 0; i < path.size(); ++i )
						b += path[i].first;
					return b;
				}

				template<typename iterator>
				uint64_t filterIntervals(
					iterator a,
					iterator e,
					libmaus2::autoarray::AutoArray < std::pair< int32_t,int32_t > > & O
				) const
				{
					uint64_t o = 0;
					libmaus2::math::IntegerInterval<int64_t> const A(abpos,aepos-1);

					for ( ; a != e; ++a )
					{
						libmaus2::math::IntegerInterval<int64_t> const B(a->first,static_cast<int64_t>(a->second) - 1);
						libmaus2::math::IntegerInterval<int64_t> const I = libmaus2::math::IntegerInterval<int64_t>::intersection(A,B);

						if ( ! I.isEmpty() )
						{
							O.push(o,std::pair< int32_t,int32_t >(I.from,I.to+1));
						}
					}

					return o;
				}

				void alignToTracePoints(int64_t const tspace)
				{
					assert ( tlen = path.size() * 2 );
					assert ( (bepos-bbpos) == static_cast<int64_t>(getBSpan()) );

					int64_t const nabpos = ((abpos + tspace - 1) / tspace)*tspace;
					int64_t const naepos = (aepos / tspace) * tspace;

					if ( nabpos == naepos )
					{
						path.resize(0);
						tlen = diffs = abpos = aepos = bbpos = bepos = 0;
					}
					else
					{
						if ( nabpos != abpos )
						{
							assert ( path.size() != 0 );
							tracepoint const tpf = path.front();
							std::copy(path.begin()+1,path.end(),path.begin());
							path.pop_back();

							tlen -= 2;
							abpos  = nabpos;
							bbpos += tpf.second;
							diffs -= tpf.first;

							assert ( tlen = path.size() * 2 );
							assert ( (bepos-bbpos) == static_cast<int64_t>(getBSpan()) );
						}
						if ( naepos != aepos )
						{
							assert ( path.size() != 0 );
							tracepoint const tpe = path.back();
							path.pop_back();

							tlen -= 2;
							aepos = naepos;
							bepos -= tpe.second;
							diffs -= tpe.first;

							assert ( tlen = path.size() * 2 );
							assert ( (bepos-bbpos) == static_cast<int64_t>(getBSpan()) );
						}
					}

					assert ( abpos % tspace == 0 );
					assert ( aepos % tspace == 0 );
					assert ( (aepos-abpos)/tspace == static_cast<int64_t>(path.size()) );
					assert ( tlen = path.size() * 2 );
					assert ( (bepos-bbpos) == static_cast<int64_t>(getBSpan()) );
				}

				bool pathValidSmall(bool const small) const
				{
					bool ok = true;
					if ( small )
					{
						for ( uint64_t i = 0; ok && i < path.size(); ++i )
							ok = ok && (path[i].first < 256) && (path[i].second < 256);
					}
					return ok;
				}

				double getErrorRateA() const
				{
					return static_cast<double>(diffs) / (aepos-abpos);
				}

				double getErrorRateB() const
				{
					return static_cast<double>(diffs) / (bepos-bbpos);
				}

				bool operator==(Path const & O) const
				{
					return
						comparePathMeta(O) && path == O.path;
				}

				bool comparePathMeta(Path const & O) const
				{
					return
						tlen == O.tlen &&
						diffs == O.diffs &&
						abpos == O.abpos &&
						bbpos == O.bbpos &&
						aepos == O.aepos &&
						bepos == O.bepos;
				}

				bool comparePathMetaLower(Path const & O) const
				{
					return
						tlen == O.tlen &&
						diffs <= O.diffs &&
						abpos == O.abpos &&
						bbpos == O.bbpos &&
						aepos == O.aepos &&
						bepos == O.bepos;
				}

				uint64_t deserialise(std::istream & in)
				{
					uint64_t offset = 0;
					tlen = getLittleEndianInteger4(in,offset);
					diffs = getLittleEndianInteger4(in,offset);
					abpos = getLittleEndianInteger4(in,offset);
					bbpos = getLittleEndianInteger4(in,offset);
					aepos = getLittleEndianInteger4(in,offset);
					bepos = getLittleEndianInteger4(in,offset);
					return offset;
				}

				uint64_t serialise(std::ostream & out) const
				{
					uint64_t offset = 0;
					putLittleEndianInteger4(out,tlen,offset);
					putLittleEndianInteger4(out,diffs,offset);
					putLittleEndianInteger4(out,abpos,offset);
					putLittleEndianInteger4(out,bbpos,offset);
					putLittleEndianInteger4(out,aepos,offset);
					putLittleEndianInteger4(out,bepos,offset);
					return offset;
				}

				static uint64_t serialise(
					std::ostream & out,
					int64_t const tlen,
					int64_t const diffs,
					int64_t const abpos,
					int64_t const aepos,
					int64_t const bbpos,
					int64_t const bepos)
				{
					uint64_t offset = 0;
					putLittleEndianInteger4(out,tlen,offset);
					putLittleEndianInteger4(out,diffs,offset);
					putLittleEndianInteger4(out,abpos,offset);
					putLittleEndianInteger4(out,bbpos,offset);
					putLittleEndianInteger4(out,aepos,offset);
					putLittleEndianInteger4(out,bepos,offset);
					return offset;
				}


				uint64_t serialiseWithPath(std::ostream & out, bool const small) const
				{
					uint64_t s = 0;
					s += serialise(out);
					s += serialisePath(out,small);
					return s;
				}

				uint64_t serialisePath(std::ostream & out, bool const small) const
				{
					uint64_t s = 0;
					if ( small )
					{
						for ( uint64_t i = 0; i < path.size(); ++i )
						{
							if (
								static_cast<int64_t>(path[i].first)  > static_cast<int64_t>(std::numeric_limits<uint8_t>::max())
								||
								static_cast<int64_t>(path[i].second) > static_cast<int64_t>(std::numeric_limits<uint8_t>::max())
							)
							{
								libmaus2::exception::LibMausException lme;
								lme.getStream() << "Path::serialisePath: path element (" << path[i].first << "," << path[i].second << ") is too large for small=" << small << std::endl;
								lme.finish();
								throw lme;
							}
							out.put(path[i].first);
							if ( ! out )
							{
								libmaus2::exception::LibMausException lme;
								lme.getStream() << "Path::serialisePath: output error" << std::endl;
								lme.finish();
								throw lme;
							}
							s += 1;
							out.put(path[i].second);
							if ( ! out )
							{
								libmaus2::exception::LibMausException lme;
								lme.getStream() << "Path::serialisePath: output error" << std::endl;
								lme.finish();
								throw lme;
							}
							s += 1;
						}
						return s;
					}
					else
					{
						uint64_t offset = 0;

						for ( uint64_t i = 0; i < path.size(); ++i )
						{
							putLittleEndianInteger2(out,path[i].first,offset);
							putLittleEndianInteger2(out,path[i].second,offset);
						}

						return offset;
					}
				}

				template<typename iterator>
				static uint64_t serialisePath(
					std::ostream & out,
					iterator const & path,
					uint64_t const pathsize,
					bool const small
				)
				{
					uint64_t s = 0;
					if ( small )
					{
						for ( uint64_t i = 0; i < pathsize; ++i )
						{
							if (
								static_cast<int64_t>(path[i].first)  > static_cast<int64_t>(std::numeric_limits<uint8_t>::max())
								||
								static_cast<int64_t>(path[i].second) > static_cast<int64_t>(std::numeric_limits<uint8_t>::max())
							)
							{
								libmaus2::exception::LibMausException lme;
								lme.getStream() << "Path::serialisePath: path element (" << path[i].first << "," << path[i].second << ") is too large for small=" << small << std::endl;
								lme.finish();
								throw lme;
							}
							out.put(path[i].first);
							if ( ! out )
							{
								libmaus2::exception::LibMausException lme;
								lme.getStream() << "Path::serialisePath: output error" << std::endl;
								lme.finish();
								throw lme;
							}
							s += 1;
							out.put(path[i].second);
							if ( ! out )
							{
								libmaus2::exception::LibMausException lme;
								lme.getStream() << "Path::serialisePath: output error" << std::endl;
								lme.finish();
								throw lme;
							}
							s += 1;
						}
						return s;
					}
					else
					{
						uint64_t offset = 0;

						for ( uint64_t i = 0; i < pathsize; ++i )
						{
							putLittleEndianInteger2(out,path[i].first,offset);
							putLittleEndianInteger2(out,path[i].second,offset);
						}

						return offset;
					}
				}

				Path() : path(), tlen(0), diffs(0), abpos(0), bbpos(0), aepos(0), bepos(0)
				{

				}
				Path(std::istream & in)
				{
					deserialise(in);
				}
				Path(std::istream & in, uint64_t & s)
				{
					s += deserialise(in);
				}

				uint64_t getNumErrors() const
				{
					uint64_t s = 0;
					for ( uint64_t i = 0; i < path.size(); ++i )
						s += path[i].first;
					return s;
				}
			};

			std::ostream & operator<<(std::ostream & out, Path const & P);
		}
	}
}
#endif
