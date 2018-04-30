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
#if !defined(LIBMAUS2_UTIL_COMMANDCONTAINER_HPP)
#define LIBMAUS2_UTIL_COMMANDCONTAINER_HPP

#include <libmaus2/util/Command.hpp>

namespace libmaus2
{
	namespace util
	{
		struct CommandContainer
		{
			std::vector<Command> V;
			uint64_t attempt;
			uint64_t maxattempt;
			uint64_t id;
			uint64_t mem;
			uint64_t threads;
			std::vector<uint64_t> depid;
			std::vector<uint64_t> rdepid;

			CommandContainer() : attempt(0), maxattempt(0), id(0)
			{}
			CommandContainer(std::istream & in)
			{
				deserialise(in);
			}

			bool isComplete() const
			{
				bool iscomplete = true;

				for ( uint64_t i = 0; i < V.size(); ++i )
					iscomplete = iscomplete && V[i].isComplete();

				return iscomplete;
			}

			bool isFinished() const
			{
				bool isfinished = true;

				for ( uint64_t i = 0; i < V.size(); ++i )
					isfinished = isfinished && V[i].isFinished();

				return isfinished;
			}

			void serialise(std::ostream & out) const
			{
				libmaus2::util::NumberSerialisation::serialiseNumber(out,V.size());
				libmaus2::util::NumberSerialisation::serialiseNumber(out,attempt);
				libmaus2::util::NumberSerialisation::serialiseNumber(out,maxattempt);
				libmaus2::util::NumberSerialisation::serialiseNumber(out,id);
				libmaus2::util::NumberSerialisation::serialiseNumber(out,mem);
				libmaus2::util::NumberSerialisation::serialiseNumber(out,threads);

				libmaus2::util::NumberSerialisation::serialiseNumberVector(out,depid);
				libmaus2::util::NumberSerialisation::serialiseNumberVector(out,rdepid);

				std::vector<uint64_t> O;
				for ( uint64_t i = 0; i < V.size(); ++i )
				{
					uint64_t o = out.tellp();
					O.push_back(o);
					V[i].serialise(out);
				}

				for ( uint64_t i = 0; i < O.size(); ++i )
					libmaus2::util::NumberSerialisation::serialiseNumber(out,O[i]);
			}

			void deserialise(std::istream & in)
			{
				V.resize(libmaus2::util::NumberSerialisation::deserialiseNumber(in));
				attempt = libmaus2::util::NumberSerialisation::deserialiseNumber(in);
				maxattempt = libmaus2::util::NumberSerialisation::deserialiseNumber(in);
				id = libmaus2::util::NumberSerialisation::deserialiseNumber(in);
				mem = libmaus2::util::NumberSerialisation::deserialiseNumber(in);
				threads = libmaus2::util::NumberSerialisation::deserialiseNumber(in);
				depid = libmaus2::util::NumberSerialisation::deserialiseNumberVector<uint64_t>(in);
				rdepid = libmaus2::util::NumberSerialisation::deserialiseNumberVector<uint64_t>(in);
				for ( uint64_t i = 0; i < V.size(); ++i )
					V[i].deserialise(in);
			}

			int dispatch(uint64_t const i, std::string const & fn) const
			{
				return V.at(i).dispatch(fn);
			}

			static int dispatch(std::istream & istr, uint64_t const i, std::string const & fn)
			{
				istr.clear();
				istr.seekg(0,std::ios::beg);
				// number of commands
				uint64_t const n = libmaus2::util::NumberSerialisation::deserialiseNumber(istr);
				assert ( i < n );

				// obtain pointer to command
				istr.clear();
				istr.seekg(
					- static_cast<int64_t>( n * sizeof(uint64_t) ) + static_cast<int64_t>( i * sizeof(uint64_t) ),
					std::ios::end
				);

				uint64_t const p = libmaus2::util::NumberSerialisation::deserialiseNumber(istr);

				// seek to pointer
				istr.clear();
				istr.seekg(p);

				Command const C(istr);
				return C.dispatch(fn);
			}

			static int dispatch(std::string const & fn, uint64_t const i, std::string const & scriptfn)
			{
				libmaus2::aio::InputStreamInstance ISI(fn);
				return dispatch(ISI,i,scriptfn);
			}
		};

		std::ostream & operator<<(std::ostream & out, CommandContainer const & CC);
	}
}

#endif
