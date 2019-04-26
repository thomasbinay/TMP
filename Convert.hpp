#include <string>
#include <sstream>

namespace convert
{
    template < typename T > std::string to_string( const T& n ) //Converts any type to string
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}


