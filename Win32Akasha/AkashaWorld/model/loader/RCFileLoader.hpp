#ifndef RC_FILELOADER__HPP
#define RC_FILELOADER__HPP


//Irrlicht################################################################
// #define _IRR_STATIC_LIB_
#include "IFileSystem.h"

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

//#include "ModelDefineFile.hpp"
#include "define/ModelDefineStruct.hpp"

namespace akasha
{
namespace model
{

namespace loader
{

class RCFileLoader
{
public:
	bool loadFromFile(
			irr::io::IFileSystem& sys,
			const irr::io::path&,
			ModelDefine<std::string>&);


private:

	//ConstractionData& m_data;

};

}
}
}

#endif //RC_FILELOADER__HPP
