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
#if ! defined(LIBMAUS2_DAZZLER_ALIGN_OVERLAP_HPP)
#define LIBMAUS2_DAZZLER_ALIGN_OVERLAP_HPP

#include <libmaus2/dazzler/align/Path.hpp>
#include <libmaus2/lcs/Aligner.hpp>
#include <libmaus2/math/IntegerInterval.hpp>
#include <libmaus2/dazzler/align/TraceBlock.hpp>
#include <libmaus2/dazzler/align/TracePoint.hpp>
#include <libmaus2/fastx/acgtnMap.hpp>
#include <libmaus2/dazzler/align/OverlapHeader.hpp>

namespace libmaus2
{
	namespace dazzler
	{
		namespace align
		{
			struct Overlap : public libmaus2::dazzler::db::InputBase, public libmaus2::dazzler::db::OutputBase
			{
				typedef Overlap this_type;
				typedef libmaus2::util::unique_ptr<this_type>::type unique_ptr_type;
				typedef libmaus2::util::shared_ptr<this_type>::type shared_ptr_type;

				Path path;
				uint32_t flags;
				int32_t aread;
				int32_t bread;

				uint64_t simpleSerialise(std::ostream & out) const
				{
					uint64_t l = 0;
					l += path.simpleSerialise(out);
					libmaus2::util::NumberSerialisation::serialiseNumber(out,flags,4);
					l += 4;
					l += libmaus2::util::UTF8::encodeUTF8(aread,out);
					l += libmaus2::util::UTF8::encodeUTF8(bread,out);
					return l;
				}

				uint64_t simpleDeserialise(std::istream & in)
				{
					uint64_t l = 0;

					l += path.simpleDeserialise(in);
					flags = libmaus2::util::NumberSerialisation::deserialiseNumber(in,4);
					l += 4;
					aread = libmaus2::util::UTF8::decodeUTF8(in,l);
					bread = libmaus2::util::UTF8::decodeUTF8(in,l);

					return l;
				}

				uint32_t getFlags() const
				{
					return flags;
				}

				int32_t getARead() const
				{
					return aread;
				}

				int32_t getBRead() const
				{
					return bread;
				}

				libmaus2::math::IntegerInterval<int64_t> aint() const
				{
					return path.aint();
				}

				libmaus2::math::IntegerInterval<int64_t> bint() const
				{
					return path.bint();
				}


				bool checkBSpan() const
				{
					return path.checkBSpan();
				}

				static uint64_t getBinList(libmaus2::autoarray::AutoArray < std::pair<uint64_t,uint64_t> > & A, int64_t const rl, int64_t const abpos, int64_t const aepos)
				{
					return Path::getBinList(A,rl,abpos,aepos);
				}

				static uint64_t getBin(int64_t const rl, int64_t const abpos, int64_t const aepos)
				{
					return Path::getBin(rl,abpos,aepos);
				}

				uint64_t getBinList(libmaus2::autoarray::AutoArray < std::pair<uint64_t,uint64_t> > & A, int64_t const rl) const
				{
					return path.getBinList(A,rl);
				}

				uint64_t getBin(int64_t const rl) const
				{
					return path.getBin(rl);
				}

				OverlapHeader getHeader() const
				{
					OverlapHeader OH;

					OH.inv = isInverse();
					OH.aread = aread;
					OH.bread = bread;
					OH.abpos = path.abpos;
					OH.aepos = path.aepos;
					OH.bbpos = path.bbpos;
					OH.bepos = path.bepos;
					OH.diffs = path.diffs;

					return OH;
				}

				int32_t getBBlockOffset(uint64_t const i) const
				{
					return path.getBBlockOffset(i);
				}

                                double getErrorRate() const
				{
					return path.getErrorRate();
                                }

				Overlap filter(std::pair<int32_t,int32_t> const & I, int64_t const tspace) const
				{
					Overlap OVL = *this;
					OVL.path = path.filter(I,tspace);
					return OVL;
				}

				template<typename iterator>
				uint64_t filterIntervals(
					iterator a,
					iterator e,
					libmaus2::autoarray::AutoArray < std::pair< int32_t,int32_t > > & O
				) const
				{
					return path.filterIntervals(a,e,O);
				}

				void alignToTracePoints(int64_t const tspace)
				{
					path.alignToTracePoints(tspace);
				}

				bool isEmpty() const
				{
					return path.isEmpty();
				}

				bool operator<(Overlap const & O) const
				{
					if ( aread != O.aread )
						return aread < O.aread;
					else if ( bread != O.bread )
						return bread < O.bread;
					else if ( isInverse() != O.isInverse() )
						return !isInverse();
					else if ( path.abpos != O.path.abpos )
						return path.abpos < O.path.abpos;
					#if 0
					else if ( path.aepos != O.path.aepos )
						return path.aepos < O.path.aepos;
					else if ( path.bbpos != O.path.bbpos )
						return path.bbpos < O.path.bbpos;
					else if ( path.bepos != O.path.bepos )
						return path.bepos < O.path.bepos;
					#endif
					else
						return false;
				}

				// 40

				uint64_t deserialise(std::istream & in)
				{
					uint64_t s = 0;

					s += path.deserialise(in);

					uint64_t offset = 0;
					flags = getUnsignedLittleEndianInteger4(in,offset);
					aread = getLittleEndianInteger4(in,offset);
					bread = getLittleEndianInteger4(in,offset);

					getLittleEndianInteger4(in,offset); // padding

					s += offset;

					// std::cerr << "flags=" << flags << " aread=" << aread << " bread=" << bread << std::endl;

					return s;
				}

				uint64_t serialise(std::ostream & out)
				{
					uint64_t s = 0;
					s += path.serialise(out);
					uint64_t offset = 0;
					putUnsignedLittleEndianInteger4(out,flags,offset);
					putLittleEndianInteger4(out,aread,offset);
					putLittleEndianInteger4(out,bread,offset);
					putLittleEndianInteger4(out,0,offset); // padding
					s += offset;
					return s;
				}

				static uint64_t serialise(
					std::ostream & out,
					int64_t const aread,
					int64_t const bread,
					int64_t const flags)
				{
					uint64_t offset = 0;
					putUnsignedLittleEndianInteger4(out,flags,offset);
					putLittleEndianInteger4(out,aread,offset);
					putLittleEndianInteger4(out,bread,offset);
					putLittleEndianInteger4(out,0,offset); // padding
					return offset;
				}

				uint64_t serialiseWithPath(std::ostream & out, bool const small) const
				{
					uint64_t s = 0;
					s += path.serialise(out);
					uint64_t offset = 0;
					putUnsignedLittleEndianInteger4(out,flags,offset);
					putLittleEndianInteger4(out,aread,offset);
					putLittleEndianInteger4(out,bread,offset);
					putLittleEndianInteger4(out,0,offset); // padding
					s += offset;
					s += path.serialisePath(out,small);
					return s;
				}

				bool operator==(Overlap const & O) const
				{
					return
						compareMeta(O) && path == O.path;
				}

				bool compareMeta(Overlap const & O) const
				{
					return
						path.comparePathMeta(O.path) &&
						flags == O.flags &&
						aread == O.aread &&
						bread == O.bread;
				}

				bool compareMetaLower(Overlap const & O) const
				{
					return
						path.comparePathMetaLower(O.path) &&
						flags == O.flags &&
						aread == O.aread &&
						bread == O.bread;
				}

				Overlap() : path(), flags(0), aread(0), bread(0)
				{

				}

				Overlap(std::istream & in)
				{
					deserialise(in);
				}

				Overlap(std::istream & in, uint64_t & s)
				{
					s += deserialise(in);
				}

				bool isInverse() const
				{
					return (flags & getInverseFlag()) != 0;
				}

				static uint64_t getInverseFlag()
				{
					return 1;
				}

				static uint64_t getPrimaryFlag()
				{
					return (1ull << 31);
				}

				static uint64_t getTrueFlag()
				{
					return (1ull << 30);
				}

				static uint64_t getACompFlag()
				{
					return 0x2;
				}

				static uint64_t getStartFlag()
				{
					return 0x4;
				}

				static uint64_t getNextFlag()
				{
					return 0x8;
				}

				static uint64_t getBestFlag()
				{
					return 0x10;
				}

				bool isAComp() const
				{
					return (flags & getACompFlag()) != 0;
				}

				bool isStart() const
				{
					return (flags & getStartFlag()) != 0;
				}

				bool isNext() const
				{
					return (flags & getNextFlag()) != 0;
				}

				bool isBest() const
				{
					return (flags & getBestFlag()) != 0;
				}

				bool isPrimary() const
				{
					return (flags & getPrimaryFlag()) != 0;
				}

				bool isTrue() const
				{
					return (flags & getTrueFlag()) != 0;
				}

				void setPrimary()
				{
					flags |= getPrimaryFlag();
				}

				uint64_t getNumErrors() const
				{
					return path.getNumErrors();
				}

				static Overlap computeOverlap(
					int64_t const flags,
					int64_t const aread,
					int64_t const bread,
					int64_t const abpos,
					int64_t const aepos,
					int64_t const bbpos,
					int64_t const bepos,
					int64_t const tspace,
					libmaus2::lcs::AlignmentTraceContainer const & ATC
				)
				{
					Overlap OVL;
					OVL.flags = flags;
					OVL.aread = aread;
					OVL.bread = bread;
					OVL.path = computePath(abpos,aepos,bbpos,bepos,tspace,ATC);
					return OVL;
				}

				static Path computePath(
					int64_t const abpos,
					int64_t const aepos,
					int64_t const bbpos,
					int64_t const bepos,
					int64_t const tspace,
					libmaus2::lcs::AlignmentTraceContainer const & ATC
				)
				{
					Path path;

					path.abpos = abpos;
					path.bbpos = bbpos;
					path.aepos = aepos;
					path.bepos = bepos;
					path.diffs = 0;

					// current point on A
					int64_t a_i = ( path.abpos / tspace ) * tspace;

					libmaus2::lcs::AlignmentTraceContainer::step_type const * tc = ATC.ta;

					int64_t bsum = 0;

					while ( a_i < aepos )
					{
						assert ( a_i % tspace == 0 );

						int64_t a_c = std::max(abpos,a_i);
						// block end point on A
						int64_t const a_i_1 = std::min ( static_cast<int64_t>(a_i + tspace), static_cast<int64_t>(path.aepos) );

						assert ( (a_i_1 == aepos) || (a_i_1%tspace == 0) );

						int64_t bforw = 0;
						int64_t err = 0;
						while ( a_c < a_i_1 )
						{
							assert ( tc < ATC.te );
							switch ( *(tc++) )
							{
								case libmaus2::lcs::AlignmentTraceContainer::STEP_MATCH:
									++a_c;
									++bforw;
									break;
								case libmaus2::lcs::AlignmentTraceContainer::STEP_MISMATCH:
									++a_c;
									++bforw;
									++err;
									break;
								case libmaus2::lcs::AlignmentTraceContainer::STEP_DEL:
									++a_c;
									++err;
									break;
								case libmaus2::lcs::AlignmentTraceContainer::STEP_INS:
									++bforw;
									++err;
									break;
								case libmaus2::lcs::AlignmentTraceContainer::STEP_RESET:
									break;
							}
						}

						assert ( a_c == a_i_1 );

						// consume rest of operations if we reached end of alignment on A read
						while ( a_c == static_cast<int64_t>(path.aepos) && tc != ATC.te )
						{
							switch ( *(tc++) )
							{
								case libmaus2::lcs::AlignmentTraceContainer::STEP_MATCH:
									++a_c;
									++bforw;
									break;
								case libmaus2::lcs::AlignmentTraceContainer::STEP_MISMATCH:
									++a_c;
									++bforw;
									++err;
									break;
								case libmaus2::lcs::AlignmentTraceContainer::STEP_DEL:
									++a_c;
									++err;
									break;
								case libmaus2::lcs::AlignmentTraceContainer::STEP_INS:
									++bforw;
									++err;
									break;
								case libmaus2::lcs::AlignmentTraceContainer::STEP_RESET:
									break;
							}
						}

						path.diffs += err;
						path.path.push_back(Path::tracepoint(err,bforw));
						bsum += bforw;

						a_i = a_i_1;
					}

					assert ( tc == ATC.te );
					assert ( a_i == aepos );
					assert ( bsum == bepos-bbpos );

					path.tlen = path.path.size() << 1;

					bool const ok = bsum == (bepos - bbpos);
					if ( ! ok )
					{
						libmaus2::exception::LibMausException lme;
						lme.getStream() << "Overlap::computePath: bsum=" << bsum << " != " << (bepos - bbpos) << std::endl;

						std::pair<uint64_t,uint64_t> SL = ATC.getStringLengthUsed(ATC.ta,ATC.te);
						lme.getStream() << "SL = " << SL.first << "," << SL.second << std::endl;
						lme.getStream() << "aepos-abpos=" << aepos-abpos << std::endl;
						lme.getStream() << "bepos-bbpos=" << bepos-bbpos << std::endl;
						lme.getStream() << "ATC.te - ATC.ta=" << (ATC.te-ATC.ta) << std::endl;
						lme.getStream() << "ATC.te - tc=" << (ATC.te-tc) << std::endl;

						lme.finish();
						throw lme;
					}

					return path;
				}

				static Path computePath(std::vector<TraceBlock> const & V)
				{
					Path path;

					if ( ! V.size() )
					{
						path.abpos = 0;
						path.bbpos = 0;
						path.aepos = 0;
						path.bepos = 0;
						path.diffs = 0;
						path.tlen = 0;
					}
					else
					{
						path.abpos = V.front().A.first;
						path.aepos = V.back().A.second;
						path.bbpos = V.front().B.first;
						path.bepos = V.back().B.second;
						path.diffs = 0;
						path.tlen = 2*V.size();

						for ( uint64_t i = 0; i < V.size(); ++i )
						{
							path.path.push_back(Path::tracepoint(V[i].err,V[i].B.second-V[i].B.first));
							path.diffs += V[i].err;
						}
					}

					return path;
				}

				template<typename path_iterator>
				static void computeTrace(
					path_iterator path,
					size_t pathlen,
					int32_t const abpos,
					int32_t const aepos,
					int32_t const bbpos,
					int32_t const bepos,
					uint8_t const * aptr,
					uint8_t const * bptr,
					int64_t const tspace,
					libmaus2::lcs::AlignmentTraceContainer & ATC,
					libmaus2::lcs::Aligner & aligner
				)
				{
					// current point on A
					int32_t a_i = ( abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = ( bbpos );

					if ( static_cast<int64_t>(ATC.capacity()) < (aepos-abpos)+(bepos-bbpos) )
						ATC.resize((aepos-abpos)+(bepos-bbpos));
					// reset trace container
					ATC.reset();

					for ( size_t i = 0; i < pathlen; ++i )
					{
						// block start on A
						int32_t const a_i_0 = std::max(a_i,abpos);
						// block end on A
						int32_t const a_i_1 = std::min(static_cast<int64_t>(a_i+tspace),static_cast<int64_t>(aepos));

						// block end point on B
						int32_t const b_i_1 = b_i + path[i].second;

						// block on A
						uint8_t const * asubsub_b = aptr + a_i_0;
						uint8_t const * asubsub_e = aptr + a_i_1;

						// block on B
						uint8_t const * bsubsub_b = bptr + b_i;
						uint8_t const * bsubsub_e = bptr + b_i_1;

						aligner.align(asubsub_b,(asubsub_e-asubsub_b),bsubsub_b,bsubsub_e-bsubsub_b);

						#if 0
						#endif

						bool const ok = (
							static_cast<int64_t>(aligner.getTraceContainer().getNumErrors())
							<=
							static_cast<int64_t>(path[i].first)
						);

						if ( ! ok )
						{
							libmaus2::exception::LibMausException lme;
							lme.getStream()
									<< " [" << a_i_0 << "," << a_i_1 << ") "
									<< "[" << b_i << "," << b_i_1 << ") "
									<< "e=" << static_cast<int64_t>(aligner.getTraceContainer().getNumErrors()) << " expected " << static_cast<int64_t>(path[i].first) << std::endl;
							lme.finish();
							throw lme;
						}


						// add trace to full alignment
						ATC.push(aligner.getTraceContainer());

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					assert ( a_i == aepos );
				}

				template<typename path_iterator>
				static std::pair<int32_t,int32_t> computeDiagBand(
					path_iterator path,
					size_t pathlen,
					int32_t const abpos,
					int32_t const aepos,
					int32_t const bbpos,
					int32_t const /* bepos */,
					int32_t const tspace
				)
				{
					// current point on A
					int32_t a_i = ( abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = bbpos;

					int32_t mindiag = abpos - bbpos;
					int32_t maxdiag = mindiag;

					for ( size_t i = 0; i < pathlen; ++i )
					{
						// actual block start in a
						int32_t const a_i_0 = std::max(a_i,abpos);

						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + path[i].second;

						int32_t const blockstartdiag = a_i_0 - b_i;
						// bounds, not actual values
						int32_t const blockmindiag = blockstartdiag - path[i].second;
						int32_t const blockmaxdiag = blockstartdiag + tspace;

						mindiag = std::min(mindiag,blockmindiag);
						maxdiag = std::max(maxdiag,blockmaxdiag);

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					return std::pair<int32_t,int32_t>(mindiag,maxdiag);
				}

				template<typename path_iterator>
				static std::pair<int32_t,int32_t> computeDiagBandTracePoints(
					path_iterator path,
					size_t pathlen,
					int32_t const abpos,
					int32_t const aepos,
					int32_t const bbpos,
					int32_t const bepos,
					int32_t const tspace
				)
				{
					// current point on A
					int32_t a_i = ( abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = bbpos;

					int32_t mindiag = abpos - bbpos;
					int32_t maxdiag = mindiag;

					for ( size_t i = 0; i < pathlen; ++i )
					{
						// actual block start in a
						int32_t const a_i_0 = std::max(a_i,abpos);

						int32_t const blockstartdiag = a_i_0 - b_i;
						assert ( (i!=0) || (blockstartdiag==abpos-bbpos) );
						mindiag = std::min(mindiag,blockstartdiag);
						maxdiag = std::max(maxdiag,blockstartdiag);

						int32_t const bforw = path[i].second;

						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + bforw;

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					int32_t const blockenddiag = aepos-bepos;
					mindiag = std::min(mindiag,blockenddiag);
					maxdiag = std::max(maxdiag,blockenddiag);

					return std::pair<int32_t,int32_t>(mindiag,maxdiag);
				}

				struct TracePointId
				{
					int32_t apos;
					int32_t bpos;
					int32_t id;
					int32_t subid;

					bool operator<(TracePointId const & O) const
					{
						if ( apos != O.apos )
							return apos < O.apos;
						else if ( bpos != O.bpos )
							return bpos < O.bpos;
						else
							return id < O.id;
					}
				};

				template<typename path_iterator>
				static uint64_t pushTracePoints(
					path_iterator path,
					size_t pathlen,
					int32_t const abpos,
					int32_t const aepos,
					int32_t const bbpos,
					int32_t const /* bepos */,
					int32_t const tspace,
					libmaus2::autoarray::AutoArray< TracePointId > & TP,
					uint64_t o,
					int32_t const id
				)
				{
					TP.ensureSize(o+(pathlen+1));

					// current point on A
					int32_t a_i = ( abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = bbpos;

					for ( size_t i = 0; i < pathlen; ++i )
					{
						// actual block start in a
						int32_t const a_i_0 = std::max(a_i,abpos);

						TracePointId TPI;
						TPI.apos = a_i_0;
						TPI.bpos = b_i;
						TPI.id = id;
						TPI.subid = i;
						TP[o++] = TPI;

						int32_t const bforw = path[i].second;

						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + bforw;

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					TracePointId TPI;
					TPI.apos = a_i;
					TPI.bpos = b_i;
					TPI.id = id;
					TPI.subid = pathlen;
					TP[o++] = TPI;

					return o;
				}

				template<typename path_iterator>
				static void computeTracePreMapped(
					path_iterator path,
					size_t pathlen,
					int32_t const abpos,
					int32_t const aepos,
					int32_t const bbpos,
					int32_t const bepos,
					uint8_t const * aptr,
					uint8_t const * bptr,
					int64_t const tspace,
					libmaus2::lcs::AlignmentTraceContainer & ATC,
					libmaus2::lcs::Aligner & aligner
				)
				{
					// current point on A
					int32_t a_i = ( abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = ( bbpos );

					// reset trace container
					if ( static_cast<int64_t>(ATC.capacity()) < (aepos-abpos)+(bepos-bbpos) )
						ATC.resize((aepos-abpos)+(bepos-bbpos));
					ATC.reset();

					int64_t bsum = 0;
					for ( size_t i = 0; i < pathlen; ++i )
					{
						// update sum
						bsum += path[i].second;

						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + path[i].second;

						// block on A
						uint8_t const * asubsub_b = aptr + std::max(a_i,abpos);
						uint8_t const * asubsub_e = asubsub_b + a_i_1-std::max(a_i,abpos);

						// block on B
						uint8_t const * bsubsub_b = bptr + b_i;
						uint8_t const * bsubsub_e = bsubsub_b + (b_i_1-b_i);

						aligner.alignPreMapped(asubsub_b,(asubsub_e-asubsub_b),bsubsub_b,bsubsub_e-bsubsub_b);

						// add trace to full alignment
						ATC.push(aligner.getTraceContainer());

						#if 0
						libmaus2::lcs::AlignmentStatistics astats = aligner.getTraceContainer().getAlignmentStatistics();
						assert ( astats.matches + astats.mismatches + astats.deletions  == (a_i_1 - std::max(a_i,abpos)) );
						assert ( astats.matches + astats.mismatches + astats.insertions == (b_i_1 - b_i) );
						#endif

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					#if ! defined(NDEBUG)
					assert ( bsum == (bepos-bbpos) );
					#endif

					#if 0
					libmaus2::lcs::AlignmentStatistics astats = ATC.getAlignmentStatistics();
					assert ( astats.matches + astats.mismatches + astats.deletions  == (aepos-abpos) );
					assert ( astats.matches + astats.mismatches + astats.insertions == (bepos-bbpos) );
					#endif
				}

				static void computeTrace(
					Path const & path,
					uint8_t const * aptr,
					uint8_t const * bptr,
					int64_t const tspace,
					libmaus2::lcs::AlignmentTraceContainer & ATC,
					libmaus2::lcs::Aligner & aligner
				)
				{
					computeTrace(path.path.begin(),path.path.size(),
						path.abpos,
						path.aepos,
						path.bbpos,
						path.bepos,
						aptr,bptr,
						tspace,
						ATC,
						aligner
					);
				}

				static void computeTrace(
					std::vector<TraceBlock> const & TBV,
					uint8_t const * aptr,
					uint8_t const * bptr,
					libmaus2::lcs::AlignmentTraceContainer & ATC,
					libmaus2::lcs::Aligner & aligner
				)
				{
					// reset trace container
					ATC.reset();

					for ( size_t i = 0; i < TBV.size(); ++i )
					{
						int32_t const a_i_0 = TBV[i].A.first;
						int32_t const a_i_1 = TBV[i].A.second;
						int32_t const b_i_0 = TBV[i].B.first;
						int32_t const b_i_1 = TBV[i].B.second;

						// block on A
						uint8_t const * asubsub_b = aptr + a_i_0;
						uint8_t const * asubsub_e = aptr + a_i_1;

						// block on B
						uint8_t const * bsubsub_b = bptr + b_i_0;
						uint8_t const * bsubsub_e = bptr + b_i_1;

						aligner.align(asubsub_b,(asubsub_e-asubsub_b),bsubsub_b,bsubsub_e-bsubsub_b);

						// add trace to full alignment
						ATC.push(aligner.getTraceContainer());
					}
				}

				int64_t computeMapping(
					uint8_t const * aptr,
					uint8_t const * bptr,
					int64_t const apos,
					int64_t const tspace,
					libmaus2::lcs::Aligner & aligner
				)
				{
					bool const inputok = apos >= path.abpos && apos < path.aepos;
					if ( ! inputok )
					{
						libmaus2::exception::LibMausException lme;
						lme.getStream() << "[E] Overlap::computeMapping apos=" << apos << " outside [" << path.abpos << "," << path.aepos << ")" << std::endl;
						lme.finish();
						throw lme;
					}

					// block on A containing apos
					int64_t const ablock = apos / tspace;
					// first block of alignment
					int64_t const fblock = path.abpos / tspace;
					// block offset on alignment
					int64_t const block = ablock - fblock;
					// block start for block containing apos
					int64_t const blockstart = std::max(static_cast<int64_t>(path.abpos),(ablock+0) * tspace);
					// block end for block containing apos
					int64_t const blockend   = std::min(static_cast<int64_t>(path.aepos),(ablock+1) * tspace);
					// offset inside block containing apos
					int64_t const offset = apos - blockstart;

					assert ( apos >= blockstart );
					assert ( offset < tspace );

					int64_t boff = path.bbpos;
					for ( int64_t i = 0; i < block; ++i )
						boff += path.path[i].second;

					aptr += blockstart;
					bptr += boff;

					int64_t const bsize = (block < static_cast<int64_t>(path.path.size())) ? path.path[block].second : 0;

					aligner.align(aptr,blockend-blockstart,bptr,bsize);

					std::pair<uint64_t,uint64_t> const adv = aligner.getTraceContainer().advanceA(apos-blockstart);
					std::pair<uint64_t,uint64_t> const SL = aligner.getTraceContainer().getStringLengthUsed(
						aligner.getTraceContainer().ta,
						aligner.getTraceContainer().ta + adv.second
					);

					return boff + SL.second;
				}

				/**
				 * compute alignment trace
				 *
				 * @param aptr base sequence of aread
				 * @param bptr base sequence of bread if isInverse() returns false, reverse complement of bread if isInverse() returns true
				 * @param tspace trace point spacing
				 * @param ATC trace container for storing trace
				 * @param aligner aligner
				 **/
				void computeTrace(
					uint8_t const * aptr,
					uint8_t const * bptr,
					int64_t const tspace,
					libmaus2::lcs::AlignmentTraceContainer & ATC,
					libmaus2::lcs::Aligner & aligner
				) const
				{
					try
					{
						computeTrace(path,aptr,bptr,tspace,ATC,aligner);
					}
					catch(std::exception const & ex)
					{
						std::string const what = ex.what();

						libmaus2::exception::LibMausException lme;
						lme.getStream() << "[E] aread=" << aread << " bread=" << bread << ": " << what;
						lme.finish();
						throw lme;
					}
				}

				/**
				 * get error block histogram
				 *
				 * @param tspace trace point spacing
				 **/
				void fillErrorHistogram(
					int64_t const tspace,
					std::map< uint64_t, std::map<uint64_t,uint64_t> > & H,
					int64_t const rlen
				) const
				{
					// current point on A
					int32_t a_i = ( path.abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = ( path.bbpos );

					for ( size_t i = 0; i < path.path.size(); ++i )
					{
						// block start on A
						int32_t const a_i_0 = std::max( a_i, path.abpos );
						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(path.aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + path.path[i].second;

						bool const blockleftaligned = (a_i_0 % tspace == 0);
						bool const blockrightaligned = (a_i_1 % tspace == 0 );
						bool const blocknonempty = (a_i_1 > a_i_0);
						bool const fullblock = blockleftaligned && blockrightaligned && blocknonempty;

						if ( fullblock )
						{
							H [ a_i / tspace ][path.path[i].first]++;
						}
						// last block on read
						else if ( blockleftaligned && (a_i_1 == rlen) )
						{
							// block size
							int64_t const l = rlen - a_i_0;
							assert ( l < tspace );
							// errors scaled
							uint64_t const ue = std::floor((path.path[i].first * (static_cast<double>(l) / static_cast<double>(tspace)))+0.5);
							// error fraction
							uint64_t const ur = std::floor((ue                 * (static_cast<double>(tspace) / static_cast<double>(l)))+0.5);

							// save
							if ( ur == path.path[i].second )
								H [ a_i / tspace ][ue]++;
						}

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}
				}

				/**
				 * get error block histogram
				 *
				 * @param tspace trace point spacing
				 **/
				void fillErrorHistogram(
					int64_t const tspace,
					std::map< uint64_t, std::map<double,uint64_t> > & H,
					int64_t const rlen
				) const
				{
					// current point on A
					int32_t a_i = ( path.abpos / tspace ) * tspace;

					for ( size_t i = 0; i < path.path.size(); ++i )
					{
						// block start on A
						int32_t const a_i_0 = std::max( a_i, path.abpos );
						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(path.aepos) );

						bool const blockleftaligned = (a_i_0 % tspace == 0);
						bool const blockrightaligned = (a_i_1 % tspace == 0 );
						bool const blocknonempty = (a_i_1 > a_i_0);
						bool const fullblock = blockleftaligned && blockrightaligned && blocknonempty;

						if ( fullblock )
							H [ a_i / tspace ][ static_cast<double>(path.path[i].first) / tspace ]++;
						// last block on read
						else if ( blockleftaligned && (a_i_1 == rlen) )
							H [ a_i / tspace ][ static_cast<double>(path.path[i].first) / (rlen - a_i_0) ]++;

						// update start points
						a_i = a_i_1;
					}
				}

				/**
				 * get error block histogram
				 *
				 * @param tspace trace point spacing
				 **/
				uint64_t fillErrorHistogram(
					int64_t const tspace,
					libmaus2::autoarray::AutoArray < std::pair < uint64_t, double > > & A,
					uint64_t o,
					int64_t const rlen
				) const
				{
					// current point on A
					int32_t a_i = ( path.abpos / tspace ) * tspace;

					for ( size_t i = 0; i < path.path.size(); ++i )
					{
						// block start on A
						int32_t const a_i_0 = std::max( a_i, path.abpos );
						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(path.aepos) );

						bool const blockleftaligned = (a_i_0 % tspace == 0);
						bool const blockrightaligned = (a_i_1 % tspace == 0 );
						bool const blocknonempty = (a_i_1 > a_i_0);
						bool const fullblock = blockleftaligned && blockrightaligned && blocknonempty;

						if ( fullblock )
							A.push(o,std::pair < uint64_t, double >(a_i/tspace,static_cast<double>(path.path[i].first) / tspace));
						// last block on read
						else if ( blockleftaligned && (a_i_1 == rlen) )
							A.push(o,std::pair < uint64_t, double >(a_i/tspace,static_cast<double>(path.path[i].first) / (rlen - a_i_0)));

						// update start points
						a_i = a_i_1;
					}

					return o;
				}

				/**
				 * get bases in full blocks and number of errors in these blocks
				 *
				 * @param tspace trace point spacing
				 **/
				std::pair<uint64_t,uint64_t> fullBlockErrors(int64_t const tspace) const
				{
					// current point on A
					int32_t a_i = ( path.abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = ( path.bbpos );

					uint64_t errors = 0;
					uint64_t length = 0;

					for ( size_t i = 0; i < path.path.size(); ++i )
					{
						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(path.aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + path.path[i].second;

						if (
							(a_i_1 - a_i) == tspace
							&&
							(a_i % tspace == 0)
							&&
							(a_i_1 % tspace == 0)
						)
						{
							errors += path.path[i].first;
							length += tspace;
						}

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					return std::pair<uint64_t,uint64_t>(length,errors);
				}

				Overlap getSwapped(
					int64_t const tspace,
					uint8_t const * aptr,
					int64_t const alen,
					// pointer to B or RC of B (check isInverse)
					uint8_t const * bptr,
					int64_t const blen,
					libmaus2::lcs::AlignmentTraceContainer & ATC,
					libmaus2::lcs::Aligner & aligner
				) const
				{
					computeTrace(path,aptr,bptr,tspace,ATC,aligner);
					ATC.swapRoles();

					Overlap OVL;
					OVL.flags = flags;
					OVL.aread = bread;
					OVL.bread = aread;

					if ( ! isInverse() )
					{
						OVL.path = computePath(
							path.bbpos,
							path.bepos,
							path.abpos,
							path.aepos,
							tspace,ATC);
						return OVL;
					}
					else
					{
						std::reverse(ATC.ta,ATC.te);

						OVL.path = computePath(
							blen - path.bepos,
							blen - path.bbpos,
							alen - path.aepos,
							alen - path.abpos,
							tspace,ATC);
						return OVL;

					}
				}

				bool bConsistent() const
				{
					int64_t bsum = 0;
					for ( uint64_t i = 0; i < path.path.size(); ++i )
						bsum += path.path[i].second;
					return bsum == (path.bepos-path.bbpos);
				}

				Overlap getSwappedPreMapped(
					int64_t const tspace,
					uint8_t const * aptr,
					int64_t const alen,
					uint8_t const * bptr,
					int64_t const blen,
					libmaus2::lcs::AlignmentTraceContainer & ATC,
					libmaus2::lcs::Aligner & aligner
				) const
				{
					if ( ! isInverse() )
					{
						computeTracePreMapped(
							path.path.begin(),
							path.path.size(),
							path.abpos,path.aepos,path.bbpos,path.bepos,aptr,bptr,tspace,
							ATC,aligner);

						ATC.swapRoles();
						// std::reverse(ATC.ta,ATC.te);
						Overlap OVL;
						OVL.flags = flags;
						OVL.aread = bread;
						OVL.bread = aread;
						OVL.path = computePath(
							path.bbpos,
							path.bepos,
							path.abpos,
							path.aepos,
							tspace,ATC);
						return OVL;
					}
					else
					{
						computeTracePreMapped(
							path.path.begin(),
							path.path.size(),
							path.abpos,path.aepos,path.bbpos,path.bepos,aptr,bptr,tspace,
							ATC,aligner);

						ATC.swapRoles();
						std::reverse(ATC.ta,ATC.te);
						Overlap OVL;
						OVL.flags = flags;
						OVL.aread = bread;
						OVL.bread = aread;
						OVL.path = computePath(
							blen - path.bepos,
							blen - path.bbpos,
							alen - path.aepos,
							alen - path.abpos,
							tspace,ATC);
						return OVL;

					}
				}

				struct GetSwappedPreMappedInvAux
				{
					typedef GetSwappedPreMappedInvAux this_type;
					typedef libmaus2::util::unique_ptr<this_type>::type unique_ptr_type;

					libmaus2::autoarray::AutoArray<uint8_t> T;
					libmaus2::autoarray::AutoArray<Path::tracepoint> TP;
					uint64_t apadlen;
					int64_t aread;

					GetSwappedPreMappedInvAux() : apadlen(0), aread(-1) {}

					void reset() { aread = -1; }

					void computePaddedRC(
						int const tspace,
						uint8_t const * aptr,
						int64_t const alen,
						int64_t const raread
					)
					{
						if ( raread != aread )
						{
							// padding to make length of a multiple of tspace
							uint64_t const apadbytes = (tspace - (alen % tspace)) % tspace;
							// padded length for a
							apadlen = alen + apadbytes;
							// allocate memory
							T.ensureSize(apadlen + 2);

							// end of allocated area
							uint8_t * t = T.begin() + (apadlen + 2);

							// terminator in back
							*(--t) = 4;
							// add reverse complement
							uint8_t const * s = aptr;
							uint8_t const * const se = s + alen;
							while ( s != se )
								*(--t) = (*(s++)) ^ 3;
							// fill pad bytes and front terminator
							while ( t != T.begin() )
								*(--t) = 4;

							#if 0
							// check
							uint8_t const * uc = T.begin() + 1 + apadbytes;
							for ( int64_t i = 0; i < alen; ++i )
								assert ( *(uc++) == (aptr[alen-i-1] ^ 3) );
							#endif

							aread = raread;
						}
					}

					void computeReversePathVector(Path const & path)
					{
						// compute reverse trace vector
						TP.ensureSize(path.path.size());
						Path::tracepoint * tp = TP.begin() + path.path.size();
						for ( uint64_t i = 0; i < path.path.size(); ++i )
							*(--tp) = path.path[i];
						assert ( tp == TP.begin() );
					}
				};

				/*
				 * aread assumed short
				 */
				Overlap getSwappedPreMappedInv(
					int64_t const tspace,
					uint8_t const * aptr,
					int64_t const alen,
					uint8_t const * bptr,
					int64_t const blen,
					libmaus2::lcs::AlignmentTraceContainer & ATC,
					libmaus2::lcs::Aligner & aligner,
					GetSwappedPreMappedInvAux & aux
				) const
				{
					if ( ! isInverse() )
					{
						computeTracePreMapped(
							path.path.begin(),
							path.path.size(),
							path.abpos,path.aepos,path.bbpos,path.bepos,aptr,bptr,tspace,
							ATC,aligner);

						ATC.swapRoles();
						// std::reverse(ATC.ta,ATC.te);
						Overlap OVL;
						OVL.flags = flags;
						OVL.aread = bread;
						OVL.bread = aread;
						OVL.path = computePath(
							path.bbpos,
							path.bepos,
							path.abpos,
							path.aepos,
							tspace,ATC);
						return OVL;
					}
					else
					{
						aux.computePaddedRC(tspace,aptr,alen,aread);
						aux.computeReversePathVector(path);

						// compute trace
						computeTracePreMapped(
							aux.TP.begin(),
							path.path.size(),
							aux.apadlen - path.aepos,
							aux.apadlen - path.abpos,
							blen - path.bepos,
							blen - path.bbpos,
							aux.T.begin() + 1 /* terminator */,
							bptr,
							tspace,
							ATC,aligner
						);

						ATC.swapRoles();

						Overlap OVL;
						OVL.flags = flags;
						OVL.aread = bread;
						OVL.bread = aread;
						OVL.path = computePath
						(
							blen - path.bepos,
							blen - path.bbpos,
							alen - path.aepos,
							alen - path.abpos,
							tspace,ATC
						);
						return OVL;
					}
				}

				struct OffsetInfo
				{
					int64_t apos;
					int64_t bpos;
					uint64_t offset;

					OffsetInfo() {}
					OffsetInfo(
						int64_t const rapos,
						int64_t const rbpos,
						uint64_t const roffset
					) : apos(rapos), bpos(rbpos), offset(roffset) {}
				};

				template<typename path_type>
				static int64_t getMaxDif(
					OffsetInfo const & A,
					OffsetInfo const & B,
					path_type const path
				)
				{
					int64_t maxdif = 0;

					for ( uint64_t i = A.offset; i < B.offset; ++i )
						maxdif = std::max(maxdif,static_cast<int64_t>(path[i].first));

					return maxdif;
				}

				template<typename path_type>
				static OffsetInfo getBforAOffset(
					int64_t const tspace,
					int64_t const abpos,
					int64_t const aepos,
					int64_t const bbpos,
					int64_t const /* bepos */,
					int64_t const a,
					path_type const path,
					uint64_t const pathsize
				)
				{
					// current point on A
					int32_t a_i = ( abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = ( bbpos );

					for ( size_t i = 0; i < pathsize; ++i )
					{
						// block start point on A
						int32_t const a_i_0 = std::max ( a_i, static_cast<int32_t>(abpos) );
						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + path[i].second;

						if ( a_i_0 == a )
							return OffsetInfo(a_i_0,b_i,i);

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					if ( a_i == a )
						return OffsetInfo(a_i,b_i,pathsize);

					libmaus2::exception::LibMausException lme;
					lme.getStream() << "[E] Overlap::getBforAOffset: no result for a=" << a << std::endl;
					lme.finish();
					throw lme;
				}

				template<typename path_type>
				static Overlap subOverlap(
					int64_t const /* tspace */,
					int64_t const aread,
					int64_t const bread,
					bool const inv,
					OffsetInfo const & from,
					OffsetInfo const & to,
					path_type const path,
					uint64_t const /* pathsize */
				)
				{
					Overlap OVL;
					OVL.aread = aread;
					OVL.bread = bread;
					OVL.flags = inv ? getInverseFlag() : 0;
					OVL.path.abpos = from.apos;
					OVL.path.aepos = to.apos;
					OVL.path.bbpos = from.bpos;
					OVL.path.bepos = to.bpos;
					OVL.path.path.resize(to.offset-from.offset);

					OVL.path.diffs = 0;
					for ( uint64_t i = from.offset; i < to.offset; ++i )
					{
						OVL.path.path[i-from.offset] = path[i];
						OVL.path.diffs += path[i].first;
					}

					OVL.path.tlen = 2*OVL.path.path.size();

					return OVL;
				}


				Path getSwappedPath(int64_t const tspace) const
				{
					return computePath(getSwappedTraceBlocks(tspace));
				}

				Path getSwappedPathInverse(int64_t const tspace, int64_t const alen, int64_t const blen) const
				{
					return computePath(getSwappedTraceBlocksInverse(tspace,alen,blen));
				}

				std::vector<TraceBlock> getSwappedTraceBlocks(int64_t const tspace) const
				{
					std::vector<TraceBlock> V = getTraceBlocks(tspace);

					for ( uint64_t i = 0; i < V.size(); ++i )
						V[i].swap();

					return V;
				}

				std::vector<TraceBlock> getSwappedTraceBlocksInverse(int64_t const tspace, int64_t const alen, int64_t const blen) const
				{
					std::vector<TraceBlock> V = getTraceBlocks(tspace);

					for ( uint64_t i = 0; i < V.size(); ++i )
						V[i].swap(alen,blen);
					std::reverse(V.begin(),V.end());

					return V;
				}

				std::vector<TraceBlock> getTraceBlocks(int64_t const tspace) const
				{
					// current point on A
					int32_t a_i = ( path.abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = ( path.bbpos );

					std::vector < TraceBlock > V;

					for ( size_t i = 0; i < path.path.size(); ++i )
					{
						// block start point on A
						int32_t const a_i_0 = std::max ( a_i, static_cast<int32_t>(path.abpos) );
						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(path.aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + path.path[i].second;

						V.push_back(
							TraceBlock(
								std::pair<int64_t,int64_t>(a_i_0,a_i_1),
								std::pair<int64_t,int64_t>(b_i,b_i_1),
								path.path[i].first
							)
						);

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					return V;
				}

				uint64_t getTraceBlocks(int64_t const tspace, libmaus2::autoarray::AutoArray<TraceBlock> & A, uint64_t o = 0, bool fullonly = false) const
				{
					// current point on A
					int32_t a_i = ( path.abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = ( path.bbpos );

					for ( size_t i = 0; i < path.path.size(); ++i )
					{
						// block start point on A
						int32_t const a_i_0 = std::max ( a_i, static_cast<int32_t>(path.abpos) );
						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(path.aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + path.path[i].second;

						if ( (! fullonly) || (a_i_1-a_i_0 == tspace) )
							A.push(o,
								TraceBlock(
									std::pair<int64_t,int64_t>(a_i_0,a_i_1),
									std::pair<int64_t,int64_t>(b_i,b_i_1),
									path.path[i].first
								)
							);

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					return o;
				}

				std::vector<TracePoint> getTracePoints(int64_t const tspace, uint64_t const traceid) const
				{
					std::vector<TracePoint> V;
					getTracePoints(tspace,traceid,V);
					return V;
				}

				void getTracePoints(int64_t const tspace, uint64_t const traceid, std::vector<TracePoint> & V) const
				{
					// current point on A
					int32_t a_i = ( path.abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = ( path.bbpos );

					for ( size_t i = 0; i < path.path.size(); ++i )
					{
						// block start point on A
						int32_t const a_i_0 = std::max ( a_i, static_cast<int32_t>(path.abpos) );
						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(path.aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + path.path[i].second;

						V.push_back(TracePoint(a_i_0,b_i,traceid));

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					if ( V.size() )
						V.push_back(TracePoint(a_i,b_i,traceid));
					else
						V.push_back(TracePoint(path.abpos,path.bbpos,traceid));
				}

				uint64_t getTracePoints(int64_t const tspace, uint64_t const traceid, libmaus2::autoarray::AutoArray<TracePoint> & V, uint64_t o = 0) const
				{
					// current point on A
					int32_t a_i = ( path.abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = ( path.bbpos );

					for ( size_t i = 0; i < path.path.size(); ++i )
					{
						// block start point on A
						int32_t const a_i_0 = std::max ( a_i, static_cast<int32_t>(path.abpos) );
						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(path.aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + path.path[i].second;

						V.push(o,TracePoint(a_i_0,b_i,traceid));

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					if ( o )
						V.push(o,TracePoint(a_i,b_i,traceid));
					else
						V.push(o,TracePoint(path.abpos,path.bbpos,traceid));

					return o;
				}

				std::vector<TracePoint> getSwappedTracePoints(int64_t const tspace, uint64_t const traceid) const
				{
					std::vector<TracePoint> V;
					getSwappedTracePoints(tspace,traceid,V);
					return V;
				}

				void getSwappedTracePoints(int64_t const tspace, uint64_t const traceid, std::vector<TracePoint> & V) const
				{
					// current point on A
					int32_t a_i = ( path.abpos / tspace ) * tspace;
					// current point on B
					int32_t b_i = ( path.bbpos );

					for ( size_t i = 0; i < path.path.size(); ++i )
					{
						// block start point on A
						int32_t const a_i_0 = std::max ( a_i, static_cast<int32_t>(path.abpos) );
						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(path.aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + path.path[i].second;

						V.push_back(TracePoint(a_i_0,b_i,traceid));

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					if ( V.size() )
						V.push_back(TracePoint(a_i,b_i,traceid));
					else
						V.push_back(TracePoint(path.abpos,path.bbpos,traceid));

					for ( uint64_t i = 0; i < V.size(); ++i )
						V[i].swap();
				}

				std::vector<uint64_t> getFullBlocks(int64_t const tspace) const
				{
					std::vector<TraceBlock> const TB = getTraceBlocks(tspace);
					std::vector<uint64_t> B;
					for ( uint64_t i = 0; i < TB.size(); ++i )
						if (
							TB[i].A.second - TB[i].A.first == tspace
						)
						{
							assert ( TB[i].A.first % tspace == 0 );

							B.push_back(TB[i].A.first / tspace);
						}
					return B;
				}

				static bool haveOverlappingTraceBlock(std::vector<TraceBlock> const & VA, std::vector<TraceBlock> const & VB)
				{
					uint64_t b_low = 0;

					for ( uint64_t i = 0; i < VA.size(); ++i )
					{
						while ( (b_low < VB.size()) && (VB[b_low].A.second <= VA[i].A.first) )
							++b_low;

						assert ( (b_low == VB.size()) || (VB[b_low].A.second > VA[i].A.first) );

						uint64_t b_high = b_low;
						while ( b_high < VB.size() && (VB[b_high].A.first < VA[i].A.second) )
							++b_high;

						assert ( (b_high == VB.size()) || (VB[b_high].A.first >= VA[i].A.second) );

						if ( b_high-b_low )
						{
							for ( uint64_t j = b_low; j < b_high; ++j )
							{
								assert ( VA[i].overlapsA(VB[j]) );
								if ( VA[i].overlapsB(VB[j]) )
									return true;
							}
						}
					}

					return false;

				}

				static bool haveOverlappingTraceBlock(Overlap const & A, Overlap const & B, int64_t const tspace)
				{
					return haveOverlappingTraceBlock(A.getTraceBlocks(tspace),B.getTraceBlocks(tspace));
				}

				/**
				 * fill number of spanning reads for each sparse trace point on read
				 *
				 * @param tspace trace point spacing
				 **/
				void fillSpanHistogram(
					int64_t const tspace,
					int64_t const rlen,
					double const ethres,
					uint64_t const bthres,
					libmaus2::autoarray::AutoArray < bool > & A,
					libmaus2::autoarray::AutoArray < uint64_t > & S
				) const
				{
					// current point on A
					int32_t a_i = (path.abpos / tspace) * tspace;
					// current point on B
					int32_t b_i = ( path.bbpos );

					int64_t firstfullblock = -1;
					uint64_t o = 0;

					assert ( static_cast<int64_t>(S.size()) == (rlen + tspace - 1)/tspace );

					for ( size_t i = 0; i < path.path.size(); ++i )
					{
						// start point on A
						int32_t const a_i_0 = std::max ( a_i, path.abpos );
						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(path.aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + path.path[i].second;

						bool const fullblock = ((a_i_1-a_i_0) == tspace) && ((a_i_0 % tspace) == 0);
						bool const lastblock = (a_i_0 % tspace == 0) && (a_i_1 == rlen);
						bool const recordblock = fullblock || lastblock;

						double const erate = static_cast<double>(path.path[i].first) / (a_i_1-a_i_0);

						if ( recordblock && (firstfullblock < 0) )
							firstfullblock = a_i_0 / tspace;

						// std::cerr << "[" << a_i_0 << "," << a_i_1 << ") fullblock=" << fullblock << " lastblock=" << lastblock << " recordblock=" << recordblock << std::endl;

						if ( recordblock )
						{
							// std::cerr << "block " << ((firstfullblock+o)*tspace) << " erate=" << erate << " mark " << (erate <= ethres) << std::endl;
							A.push(o,erate <= ethres);
						}

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					// do we have the complete end block?
					bool const haveend = (path.aepos == rlen) && (path.abpos <= ((path.aepos/tspace)*tspace));

					#if 0
					std::cerr << getHeader() << " rlen=" << rlen << std::endl;
					#endif

					uint64_t olow = 0;
					while ( olow < o )
					{
						// find end of bad region
						uint64_t obad = olow;
						while ( obad < o && !A[obad] )
							++obad;

						#if 0
						if ( olow < obad )
							std::cerr << "bad " << (olow+firstfullblock)*tspace << "," << std::min(static_cast<int64_t>((obad+firstfullblock)*tspace),rlen) << std::endl;
						#endif
						olow = obad;

						// find end of good region
						uint64_t ohigh = olow;
						while ( ohigh < o && A[ohigh] )
							++ohigh;

						if ( olow < ohigh )
						{
							#if 0
							std::cerr << "good " << (olow+firstfullblock)*tspace << "," << std::min(static_cast<int64_t>((ohigh+firstfullblock)*tspace),rlen) << std::endl;
							#endif

							uint64_t const od = ohigh-olow;

							// range is too small, mark all as bad
							if ( od <= bthres )
							{
								#if 0
								std::cerr << "too short, marking all as bad" << std::endl;
								#endif
								for ( uint64_t i = olow; i < ohigh; ++i )
									A.at(i) = false;
							}
							else
							{
								bool const keepfirst = ( olow == 0 && firstfullblock == 0 );
								bool const keeplast = ( ohigh == o && haveend );

								#if 0
								std::cerr << "keepfirst=" << keepfirst << " keeplast=" << keeplast << std::endl;
								#endif

								// mark first and last bthres as bad
								for ( uint64_t i = 0; i < bthres; ++i )
								{
									if ( ! keepfirst )
									{
										A.at(olow+i) = false;
										#if 0
										std::cerr << "marking " << (olow+firstfullblock+i)*tspace << "," << std::min(static_cast<int64_t>((olow+firstfullblock+i+1)*tspace),rlen) << " as bad" << std::endl;
										#endif
									}
									if ( ! keeplast )
									{
										A.at(ohigh-i-1) = false;
										#if 0
										std::cerr << "marking " << (olow+firstfullblock+ohigh-i-1)*tspace << "," << std::min(static_cast<int64_t>((olow+firstfullblock+ohigh-i-1+1)*tspace),rlen) << " as bad" << std::endl;
										#endif
									}
								}
							}
						}

						olow = ohigh;
					}

					for ( uint64_t i = 0; i < o; ++i )
						if ( A[i] )
						{
							assert ( firstfullblock + i < S.size() );
							S.at(firstfullblock+i)++;
						}
				}

				/**
				 * fill number of spanning reads for each sparse trace point on read
				 *
				 * @param tspace trace point spacing
				 **/
				void fillSpanHistogram(
					int64_t const tspace,
					std::map< uint64_t, uint64_t > & H,
					uint64_t const ethres,
					uint64_t const cthres,
					uint64_t const rlen
				) const
				{
					std::vector<uint64_t> M(path.path.size(),std::numeric_limits<uint64_t>::max());

					// id of lowest block in alignment
					uint64_t const lowblockid = (path.abpos / tspace);
					// number of blocks
					uint64_t const numblocks = (rlen+tspace-1)/tspace;
					// current point on A
					int32_t a_i = lowblockid * tspace;
					// current point on B
					int32_t b_i = ( path.bbpos );

					for ( size_t i = 0; i < path.path.size(); ++i )
					{
						// start point on A
						int32_t const a_i_0 = std::max ( a_i, path.abpos );
						// block end point on A
						int32_t const a_i_1 = std::min ( static_cast<int32_t>(a_i + tspace), static_cast<int32_t>(path.aepos) );
						// block end point on B
						int32_t const b_i_1 = b_i + path.path[i].second;

						bool const fullblock = ((a_i_0-a_i_1) == tspace) && ((a_i_0 % tspace) == 0);
						assert ( (!fullblock) || (a_i_1 % tspace == 0) );

						if ( fullblock && path.path[i].first <= ethres )
							M . at ( i ) = path.path[i].first;

						// update start points
						b_i = b_i_1;
						a_i = a_i_1;
					}

					// number of valid blocks on the right
					uint64_t numleft = 0;
					uint64_t numright = 0;
					for ( uint64_t i = 0; i < M.size(); ++i )
						if ( M[i] != std::numeric_limits<uint64_t>::max() )
							numright++;

					// mark all blocks as spanned if we find at least cthres ok blocks left and right of it (at any distance)
					for ( uint64_t i = 0; i < M.size(); ++i )
					{
						bool const below = M[i] != std::numeric_limits<uint64_t>::max();

						if ( below )
							numright -= 1;

						if (
							(numleft >= cthres && numright >= cthres)
						)
							H [ lowblockid + i ] += 1;

						if ( below )
							numleft += 1;
					}

					// mark first cthres blocks as spanned if all first cthres blocks are ok
					if ( (path.abpos == 0) && M.size() >= 2*cthres )
					{
						uint64_t numok = 0;
						for ( uint64_t i = 0; i < 2*cthres; ++i )
							if ( M.at(i) != std::numeric_limits<uint64_t>::max() )
								++numok;
						if ( numok == 2*cthres )
						{
							for ( uint64_t i = 0; i < cthres; ++i )
								H [ i ] += 1;
						}
					}
					// mark last cthres blocks as spanned if all last 2*cthres blocks are ok
					if ( (path.aepos == static_cast<int64_t>(rlen)) && M.size() >= 2*cthres )
					{
						uint64_t numok = 0;
						for ( uint64_t i = 0; i < 2*cthres; ++i )
							if ( M.at(M.size()-i-1) != std::numeric_limits<uint64_t>::max() )
								++numok;
						if ( numok == 2*cthres )
						{
							for ( uint64_t i = 0; i < cthres; ++i )
								H [ numblocks - i - 1 ] += 1;
						}
					}
				}

				uint64_t getErrorSum() const
				{
					return path.getErrorSum();
				}
			};

			struct OverlapComparator
			{
				bool operator()(Overlap const & lhs, Overlap const & rhs) const
				{
					if ( lhs.aread != rhs.aread )
						return lhs.aread < rhs.aread;
					else if ( lhs.bread != rhs.bread )
						return lhs.bread < rhs.bread;
					else if ( lhs.isInverse() != rhs.isInverse() )
						return !lhs.isInverse();
					else if ( lhs.path.abpos != rhs.path.abpos )
						return lhs.path.abpos < rhs.path.abpos;
					else
						return false;
				}

			};

			struct OverlapFullComparator
			{
				static bool compare(Overlap const & lhs, Overlap const & rhs)
				{
					if ( lhs.aread != rhs.aread )
						return lhs.aread < rhs.aread;
					else if ( lhs.bread != rhs.bread )
						return lhs.bread < rhs.bread;
					else if ( lhs.isInverse() != rhs.isInverse() )
						return !lhs.isInverse();
					else if ( lhs.path.abpos != rhs.path.abpos )
						return lhs.path.abpos < rhs.path.abpos;
					else if ( lhs.path.aepos != rhs.path.aepos )
						return lhs.path.aepos < rhs.path.aepos;
					else if ( lhs.path.bbpos != rhs.path.bbpos )
						return lhs.path.bbpos < rhs.path.bbpos;
					else if ( lhs.path.bepos != rhs.path.bepos )
						return lhs.path.bepos < rhs.path.bepos;
					else
						return false;
				}

				bool operator()(Overlap const & lhs, Overlap const & rhs) const
				{
					return compare(lhs,rhs);
				}
			};

			struct OverlapComparatorBReadARead
			{
				bool operator()(Overlap const & lhs, Overlap const & rhs) const
				{
					if ( lhs.bread != rhs.bread )
						return lhs.bread < rhs.bread;
					else if ( lhs.aread != rhs.aread )
						return lhs.aread < rhs.aread;
					else if ( lhs.isInverse() != rhs.isInverse() )
						return !lhs.isInverse();
					else if ( lhs.path.abpos != rhs.path.abpos )
						return lhs.path.abpos < rhs.path.abpos;
					else
						return false;
				}

			};

			struct OverlapComparatorAIdBId
			{
				bool operator()(Overlap const & lhs, Overlap const & rhs) const
				{
					if ( lhs.aread != rhs.aread )
						return lhs.aread < rhs.aread;
					else if ( lhs.bread != rhs.bread )
						return lhs.bread < rhs.bread;
					else
						return false;
				}

			};

			struct OverlapComparatorBIdAId
			{
				bool operator()(Overlap const & lhs, Overlap const & rhs) const
				{
					if ( lhs.bread != rhs.bread )
						return lhs.bread < rhs.bread;
					else if ( lhs.aread != rhs.aread )
						return lhs.aread < rhs.aread;
					else
						return false;
				}

			};

			struct OverlapComparatorAIdAPos
			{
				bool operator()(Overlap const & lhs, Overlap const & rhs) const
				{
					if ( lhs.aread != rhs.aread )
						return lhs.aread < rhs.aread;
					else if ( lhs.path.abpos != rhs.path.abpos )
						return lhs.path.abpos < rhs.path.abpos;
					else if ( lhs.path.aepos != rhs.path.aepos )
						return lhs.path.aepos > rhs.path.aepos;
					else
						return false;
				}

			};

			std::ostream & operator<<(std::ostream & out, Overlap const & P);
		}
	}
}
#endif
