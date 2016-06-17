/*************************************************************************
 * Copyright (c) 2014 eProsima. All rights reserved.
 *
 * This copy of eProsima Fast RTPS is licensed to you under the terms described in the
 * FASTRTPS_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

/**
 * @file FragmentNumber.h
 */

#ifndef RPTS_ELEM_FRAGNUM_H_
#define RPTS_ELEM_FRAGNUM_H_
#include "../../fastrtps_dll.h"
#include "Types.h"

#include <set>
#include <cmath>
#include <algorithm>
#include <sstream>
namespace eprosima{
namespace fastrtps{
namespace rtps{

typedef uint32_t FragmentNumber_t;

//!Structure FragmentNumberSet_t, contains a group of fragmentnumbers.
//!@ingroup COMMON_MODULE
class FragmentNumberSet_t {
public:
	//!Base fragment number
	FragmentNumber_t base;

	/**
	* Assignment operator
	* @param set2 FragmentNumberSet_t to copy the data from
	*/
	FragmentNumberSet_t& operator=(const FragmentNumberSet_t& set2)
	{
		base = set2.base;
		set = set2.set;
		return *this;
	}

   FragmentNumberSet_t(): base(0), set()
   {
   }

   FragmentNumberSet_t(const std::set<FragmentNumber_t>& set2): base(0)
   {
      set = set2;
      auto min = set.begin();
      if (min != set.end())
         base = *min;
   }

	/**
	* Compares object with other FragmentNumberSet_t.
	* @param other FragmentNumberSet_t to compare
	* @return True if equal
	*/
	bool operator==(const FragmentNumberSet_t& other) {

		if (base != other.base)
			return false;
      return other.set == set;	
   }


	/**
	* Add a fragment number to the set
	* @param in FragmentNumberSet_t to add
	* @return True on success
	*/
	bool add(FragmentNumber_t in)
	{
		if (in >= base && in <= base + 255)
			set.insert(in);
		else
			return false;
		return true;
	}

	/**
	* Check if the set is empty
	* @return True if the set is empty
	*/
	bool isSetEmpty() const
	{
		return set.empty();
	}

	/**
	* Get the begin of the set
	* @return Vector iterator pointing to the begin of the set
	*/
	std::set<FragmentNumber_t>::const_iterator get_begin() const
	{
		return set.begin();
	}

	/**
	* Get the end of the set
	* @return Vector iterator pointing to the end of the set
	*/
	std::set<FragmentNumber_t>::const_iterator get_end() const
	{
		return set.end();
	}

	/**
	* Get the number of FragmentNumbers in the set
	* @return Size of the set
	*/
	size_t get_size()
	{
		return set.size();
	}

	/**
	* Get a string representation of the set
	* @return string representation of the set
	*/
	std::string print()
	{
		std::stringstream ss;
		ss << base << ":";
		for (auto it = set.begin(); it != set.end(); ++it)
			ss << *it << "-";
		return ss.str();
	}

   FragmentNumberSet_t& operator-=(const FragmentNumberSet_t& rhs)
   {
      for ( auto element : rhs.set)
         set.erase(element);
      return *this;
   }

   FragmentNumberSet_t& operator+=(const FragmentNumberSet_t& rhs)
   {
      for ( auto element : rhs.set )
         add(element);
      return *this;
   }

	std::set<FragmentNumber_t> set;
};

/**
* Prints a Fragment Number set
* @param output Output Stream
* @param sns SequenceNumber set
* @return OStream.
*/
inline std::ostream& operator<<(std::ostream& output, FragmentNumberSet_t& sns){
	return output << sns.print();
}

inline FragmentNumberSet_t operator-(FragmentNumberSet_t lhs, const FragmentNumberSet_t& rhs)
{
   for ( auto element : rhs.set)
      lhs.set.erase(element);
   return lhs;
}

inline FragmentNumberSet_t operator+(FragmentNumberSet_t lhs, const FragmentNumberSet_t& rhs)
{
   for ( auto element : rhs.set)
      lhs.add(element);
   return lhs;
}

}
}
}

#endif /* RPTS_ELEM_FRAGNUM_H_ */