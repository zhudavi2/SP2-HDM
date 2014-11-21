/**************************************************************
*
* hector.h
*
* Description
* ===========
*  Main include file for hector project.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef __GOLEM_HECTOR__H_
#define __GOLEM_HECTOR__H_

namespace Hector {};
using namespace Hector;

#include "golem_types.h"
#include "golem_float.h"
#include "golem_utils.h"
#include "golem_list.h"
#include "golem_math.h"
#include "golem_vector.h"
#include "golem_matrix.h"
#include "golem_quaternion.h"
#include "golem_tree.h"
#include "golem_string.h"
#include "golem_datastruct.h"
#include "golem_event.h"
#include "golem_smart_ptr.h"
#include "golem_callback_handler_itf.h"
#include "golem_event_manager.h"
#include "golem_event_handler.h"
#include "golem_registered_event.h"
#include "golem_engine_info.h"
#include "golem_md5.h"
#include "golem_memory_util.h"
#include "golem_memory_buffer.h"
#include "golem_scanner.h"
#include "golem_thread.h"
#include "golem_time.h"
#include "golem_mutex.h"
#include "golem_semaphore.h"
#include "golem_data_validate.h"
#include "golem_date_time.h"
#include "golem_time_span.h"
#include "golem_version.h"
#include "golem_console.h"
#include "golem_console_win32_text.h"
#include "golem_dll_loader.h"
#include "golem_logger.h"
#include "golem_simple_net.h"
#include "golem_compression.h"
#include "golem_tcp_compressed.h"
#include "golem_unicodefile.h"
#include "golem_wad_manager.h"
#include "golem_file_manager.h"
#include "golem_random.h"
#include "golem_console_win32_ui.h"
#include "golem_qsort.h"
#include "golem_geometry.h"
#include "golem_geometry_plane.h"
#include "golem_shape.h"
#include "golem_facet_list.h"
#include "golem_color.h"
#include "golem_pixel.h"
#include "golem_image_2d.h"
#include "golem_binary_heap.h"
#include "golem_buffer.h"
#include "golem_serializable_itf.h"
#include "golem_movie_controller.h"
#include "golem_indexed_data.h" //IndexedData namespace
#include "golem_udp_stream.h"
#include "golem_node.h"
#include "golem_node_file.h"
#include "golem_node_reader.h"
#include "golem_node_writer.h"
#include "golem_design_pattern_observer.h"
#include "golem_state_machine_itf.h"
#include "golem_symmetric_matrix.h"
#include "golem_joined_vector.h"
#include "WindowsVersion.h"

void *operator new(size_t n);


/***********************************************************
 *  Define which .LIB File must be included, this
 *  prevents the project that uses hector to add it to its
 *  needed .libs
 ***********************************************************/
#ifdef _DEBUG
#   pragma comment(lib,"hector_d.lib")
#endif // #ifdef _DEBUG

#ifdef NDEBUG
#   pragma comment(lib,"hector.lib")
#endif // #ifdef NDEBUG

#pragma comment(lib,"zlib.lib")
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"winmm.lib") //for GTimeCounter


#endif // #ifndef __GOLEM_HECTOR__H_
