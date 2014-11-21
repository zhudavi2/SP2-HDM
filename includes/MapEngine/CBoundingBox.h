#ifndef _CLASS_CBOUNDINGBOX
#define _CLASS_CBOUNDINGBOX

#include "map_globals.h"

namespace Map_Engine
{


/**********************************************************
    CBoundingBox

    Rectangle used to describe the spatial extent of a more
    complex geometric feature, such as a CEdge or a 
    CPolygon.
**********************************************************/

class CBoundingBox
{
public:
    CBoundingBox();
    CBoundingBox( REAL32 x_min, REAL32 x_max, REAL32 y_min, REAL32 y_max );
    CBoundingBox( const SPoint& point_1, const SPoint& point_2 );

    void readInBinaryFile( ifstream& in_file );
    void readInAsciiFile( ifstream& in_file );

    REAL32  getMinX() const;
    REAL32  getMaxX() const;
    REAL32  getMinY() const;
    REAL32  getMaxY() const;

    bool    contains( const SPoint& point ) const;
    bool    overlaps( const CBoundingBox& other_box ) const;
    REAL32  Width() const;
    REAL32  Height() const;

    void    setMinX( REAL32 x_min );
    void    setMaxX( REAL32 x_max );
    void    setMinY( REAL32 y_min );
    void    setMaxY( REAL32 y_max );

private:
    REAL32  x_min;
    REAL32  x_max;
    REAL32  y_min;
    REAL32  y_max;
};

};

#endif