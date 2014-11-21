/**************************************************************
*
* map_engine.h
*
* Description
* ===========
*  map engine include file, automatically includes the lib
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef __MAP_ENGINE__H
#define __MAP_ENGINE__H

#include "map_globals.h"
#include "GContinuousRasterMap.h"
#include "GDiscreteRasterMap.h"
#include "CMultifaceMap.h"
#include "CLoneAreaMap.h"
#include "CCurveMap.h"
#include "CMultiface.h"
#include "CLoneArea.h"
#include "CCurve.h"
#include "CFace.h"
#include "CEdge.h"
#include "CPolygon.h"



#ifdef _DEBUG
#   pragma comment(lib,"map_engine_d.lib")
#endif
#ifdef NDEBUG
#   pragma comment(lib,"map_engine.lib")
#endif


#endif