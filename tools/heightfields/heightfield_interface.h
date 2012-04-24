// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 14.02.2012 (m,d,y)

#ifndef __H__UG__heightfield_interface__
#define __H__UG__heightfield_interface__

#include "common/types.h"

namespace ug
{

///	Heightfields can be used to transform a geometry
class IHeightfield
{
	public:
		virtual bool loads_from_file() = 0;
		virtual const char* file_name_extensions() {return "";}
		virtual bool initialize(const char* filename, number xMin, number yMin, number xMax, number yMax) = 0;

		virtual number height(number x, number y) = 0;
		virtual ~IHeightfield() {}
};

}//	end of namespace

#endif
