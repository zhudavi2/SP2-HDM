/**************************************************************
*
* golem_sdk_constants.h
*
* Description
* ===========
*  Constants needed for the SDK to run correctly and to parametrize its 
*  behavior
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GL_SDK_CONSTANTS_H_
#define _GL_SDK_CONSTANTS_H_

namespace SDK
{

   //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
   //                  CLIENT / SERVER Constants
   //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
   CWSTR   SPAWNED_SERVER_MUTEX_NAME      = L"SPAWNED_SERVER_MUTEX";   //To see if Server is running
   const INT32  NUMBER_OF_CONNECTION_TRIES   = 240;
   const REAL32 NUMBER_OF_SECS_BETWEEN_TRIES   = 0.50f;   //sec

   //! Socket Send timeout in miliseconds
   const UINT32 c_iSendTimeoutMiliseconds   = 500;

   //=======================================================================
   // NODE names (some are object attributes)
   //=======================================================================
   CWSTR DIALOG_OBJECT_NODE               = L"OBJECT";
   CWSTR DIALOG_PROPERTIES_NODE           = L"PROPERTIES";
   CWSTR DIALOG_CUSTOM_PROPERTIES_NODE    = L"CustomProperties";
   CWSTR DIALOG_NAME_NODE                 = L"NAME";
   CWSTR DIALOG_ID_NODE                   = L"ID";
   CWSTR DIALOG_BASE_TYPE_NODE            = L"BASE";
   CWSTR DIALOG_CHILDREN_NODE             = L"CHILDREN";
   CWSTR DIALOG_ZORDER_NODE               = L"ZORDER";
   CWSTR DIALOG_TYPE_NODE                 = L"TYPE";   


   //=======================================================================
   // PROPERTIES NAMES (some are object attributes)
   //=======================================================================
   CWSTR DIALOG_HIGHLIGHT_ON_MOUSE_OVER_NODE = L"HilightOnMouseOver";

   // Up/Down                 U/D  | -> UNE UNG UHE DNE DNG DHE
   // Normal/Highlighted      N/H  | 
   // Enabled/Grayed          E/G  V

   CWSTR DIALOG_PICTURE_NORMAL            = L"Picture_Normal";
   CWSTR DIALOG_PICTURE_DISABLED          = L"Picture_Disabled";
   CWSTR DIALOG_PICTURE_HIGH              = L"Picture_High";
   CWSTR DIALOG_PICTURE_DOWN              = L"Picture_Down";
   CWSTR DIALOG_PICTURE_DISABLED_DOWN     = L"Picture_Disabled_Down";
   CWSTR DIALOG_PICTURE_HIGH_DOWN         = L"Picture_High_Down";

   CWSTR DIALOG_COLOR_NORMAL              = L"Color_Normal";
   CWSTR DIALOG_COLOR_DISABLED            = L"Color_Disabled";
   CWSTR DIALOG_COLOR_HIGH                = L"Color_High";
   CWSTR DIALOG_COLOR_DOWN                = L"Color_Down";
   CWSTR DIALOG_COLOR_DISABLED_DOWN       = L"Color_Disabled_Down";
   CWSTR DIALOG_COLOR_HIGH_DOWN           = L"Color_High_Down";

   CWSTR DIALOG_TEXT_COLOR_NORMAL         = L"Text_Color_Normal";
   CWSTR DIALOG_TEXT_COLOR_DISABLED       = L"Text_Color_Disabled";
   CWSTR DIALOG_TEXT_COLOR_HIGH           = L"Text_Color_High";
   CWSTR DIALOG_TEXT_COLOR_DOWN           = L"Text_Color_Down";
   CWSTR DIALOG_TEXT_COLOR_DISABLED_DOWN  = L"Text_Color_Disabled_Down";
   CWSTR DIALOG_TEXT_COLOR_HIGH_DOWN      = L"Text_Color_High_Down";

   CWSTR DIALOG_TEXT_ALPHA                = L"TextAlpha";
   CWSTR DIALOG_PICTURE_ALPHAMAP          = L"Picture_AlphaMap";
   CWSTR DIALOG_USE_BGPICTURE_ALPHAMAP    = L"UseImageAlphaMap";
   CWSTR DIALOG_ALPHA                     = L"Alpha";

   CWSTR DIALOG_BACKGROUND_ANIMATION_NODE    = L"BackgroundAnimation";
   CWSTR DIALOG_BORDERS_NODE                 = L"Borders";
   CWSTR DIALOG_SCROLLBAR_THICKNESS_NODE     = L"Thickness";
   
   CWSTR DIALOG_USE_MASTER_COLOR_NODE        = L"UseMasterColor";   
   CWSTR DIALOG_TOOLTIP_TEXT_NODE            = L"ToolTipText";

   //Positions
   CWSTR DIALOG_HEIGHT_NODE                  = L"Height";
   CWSTR DIALOG_WIDTH_NODE                   = L"Width";
   CWSTR DIALOG_TOP_POSITION_NODE            = L"Top";
   CWSTR DIALOG_LEFT_POSITION_NODE           = L"Left";
   CWSTR DIALOG_POSITIONS_RELATIVE_NODE      = L"Relativity";
   //Text properties
   CWSTR DIALOG_FONT_NAME_NODE               = L"FontName";
   CWSTR DIALOG_FONT_SIZE_NODE               = L"FontSize";
   CWSTR DIALOG_TEXT_ALIGN_NODE              = L"TextAlign";
   CWSTR DIALOG_TEXT_OFFSET_NODE             = L"TextOffset";
   CWSTR DIALOG_TEXT_NODE                    = L"Text";
   CWSTR DIALOG_ENABLED_NODE                 = L"Enabled";
   CWSTR DIALOG_ANCHOR_NODE                  = L"Anchor";
   CWSTR DIALOG_VSCROLL_NODE                 = L"VScrollBar";
   CWSTR DIALOG_HSCROLL_NODE                 = L"HScrollBar";
   CWSTR DIALOG_SHOW_HSCROLL_NODE            = L"Show_HScroll";
   CWSTR DIALOG_SHOW_VSCROLL_NODE            = L"Show_VScroll";
   CWSTR DIALOG_MULTILINE_NODE               = L"Multiline";

   //Editbox properties
   CWSTR EDIT_CHARACTER_CASING               = L"CharacterCasing";
   CWSTR EDIT_MAX_LENGTH                     = L"MaxLength";
   CWSTR EDIT_PASSWORD_CHARACTER             = L"PasswordChar";
   CWSTR EDIT_TEXT_STRING                    = L"Text_String";

   //Scrollbar properties
   CWSTR  SCROLL_BUTTON_ONE_NAME             = L"Button1";
   CWSTR  SCROLL_BUTTON_TWO_NAME             = L"Button2";
   CWSTR  SCROLL_CURSOR_NAME                 = L"Cursor";
   CWSTR  SCROLL_TRACKER_NAME                = L"ButtonTracker";
   CWSTR  SCROLL_MINIMUM_NAME                = L"Minimum";
   CWSTR  SCROLL_MAXIMUM_NAME                = L"Maximum";
   CWSTR  SCROLL_LARGE_CHANGE_NAME           = L"LargeChange";
   CWSTR  SCROLL_SMALL_CHANGE_NAME           = L"SmallChange";
   CWSTR  SCROLL_VALUE_NAME                  = L"Value";

   //Listbox properties
   CWSTR  LIST_ITEM_TYPE                     = L"Item_Type";
   CWSTR  LIST_ITEM_HEIGHT                   = L"Item_Height";
   CWSTR  LIST_ITEMS                         = L"Items";
   
   CWSTR  LIST_ITEM_HIGHLIGHT_COLOR          = L"Color_HighlightItem";
   CWSTR  LIST_ITEM_SELECTED_COLOR           = L"Color_SelectedItem";
   CWSTR  LIST_ITEM_NORMAL_COLOR             = L"Color_NormalItem";
   CWSTR  LIST_ITEM_DISABLED_COLOR           = L"Color_DisabledItem";

   CWSTR  LIST_ITEM_TEXT_HIGHLIGHT_COLOR     = L"Color_HighlightItemText";
   CWSTR  LIST_ITEM_TEXT_SELECTED_COLOR      = L"Color_SelectedItemText";
   CWSTR  LIST_ITEM_TEXT_NORMAL_COLOR        = L"Color_NormalItemText";
   CWSTR  LIST_ITEM_TEXT_DISABLED_COLOR      = L"Color_DisabledItemText";
   
   CWSTR  LIST_ITEM_SELECTED_PICTURE         = L"Picture_SelItem";
   CWSTR  LIST_ITEM_HIGHLIGHT_PICTURE        = L"Picture_HighItem";
   CWSTR  LIST_ITEM_NORMAL_PICTURE           = L"Picture_NormalItem";
   CWSTR  LIST_ITEM_DISABLED_PICTURE         = L"Picture_DisabledItem";
   
   CWSTR  LIST_VERTICAL_SCROLL_TYPE          = L"VScrollBar";
   CWSTR  LIST_HORIZONTAL_SCROLL_TYPE        = L"HScrollBar";
   CWSTR  LIST_SELECTION_MODE                = L"Selection_Mode";

   CWSTR  LIST_SELECT_MODE_NONE               = L"None";
   CWSTR  LIST_SELECT_MODE_ONE                = L"One";
   CWSTR  LIST_SELECT_MODE_MULTI_SIMPLE       = L"MultiSimple";
   CWSTR  LIST_SELECT_MODE_MULTI_EXTENDED     = L"MultiExtended";

   const INT32 LISTBOX_DEFAULT_ROW_HEIGHT     = 25;
   CWSTR  LISTBOX_ITEM                        = L"SP2_ListboxItem";

   //Tabbed control properties
   CWSTR  TABBED_TABS                         = L"Tabs";
   CWSTR  TABBED_ACTIVE_TAB                   = L"ActiveTab";
   CWSTR  TABBED_TABS_NAME                    = L"TabNames";
   CWSTR  TABBED_BUTTON_OFFSET_LEFT           = L"ButtonsOffsetLeft";
   CWSTR  TABBED_BUTTON_WIDTH                 = L"ButtonsWidth";
   CWSTR  TABBED_BUTTON_HEIGHT                = L"ButtonsHeight";
   CWSTR  TABBED_BUTTON_DISTANCE              = L"ButtonsDistance";
   CWSTR  TABBED_BUTTON_TYPE                  = L"ButtonType";
   CWSTR  TABBED_FRAME_OFFSET_TOP             = L"FrameOffsetTop";

   //Toggle properties
   CWSTR  TOGGLE_GROUP                        = L"ToggleGroup";
   CWSTR  TOGGLE_STATE                        = L"ToggledState";

   //Combo box properties
   CWSTR  COMBO_LIST_TYPE                      = L"List_Type";
   CWSTR  COMBO_EDIT_TYPE                      = L"Edit_Type";
   CWSTR  COMBO_BUTTON_TYPE                    = L"Button_Type";
   CWSTR  COMBO_BTN_WIDTH                      = L"Button_Width";
   CWSTR  COMBO_DROP_SIZE                      = L"Drop_Size";
   const INT32 COMBO_DEFAULT_DROP_SIZE              = 100;

   //Dialog Boolean values
   CWSTR DIALOG_BOOL_TRUE                      = L"True";
   CWSTR DIALOG_BOOL_FALSE                     = L"False";

   //Border Properties
   CWSTR BORDER_LEFT_WIDTH                     = L"LeftWidth";
   CWSTR BORDER_RIGHT_WIDTH                    = L"RightWidth";
   CWSTR BORDER_TOP_HEIGHT                     = L"TopHeight";
   CWSTR BORDER_BOTTOM_HEIGHT                  = L"BottomHeight";

   //================================
   // Possible values the relative field
   //================================
   CWSTR DIALOG_RELATIVE_TOP                  = L"Top";
   CWSTR DIALOG_RELATIVE_LEFT                  = L"Left";
   CWSTR DIALOG_RELATIVE_WIDTH                  = L"Width";
   CWSTR DIALOG_RELATIVE_HEIGHT               = L"Height";

   //======================================================================
   //                  DEFAULT VALUES CONSTANTS
   //======================================================================
   CWSTR  DEFAULT_FONT_NAME                     = L"Arial Black";
   const INT32 DEFAULT_FONT_SIZE                     = 20;
   CWSTR  DEFAULT_FONT_COLOR                  = L"RGBA: 255,0,255,255";
   CWSTR  DEFAULT_DIALOG_COLOR                  = L"RGBA: 255,0,255,255";
   CWSTR  DEFAULT_TEXT_ALIGN                  = L"MiddleCenter";
   const INT32 DEFAULT_WIDTH                        = 200;
   const INT32 DEFAULT_HEIGHT                        = 150;

   CWSTR COLOR_SEPARATOR                        = L",";
   CWSTR RGB_IDENTIFIER                        = L"RGBA:";
   CWSTR HS_IDENTIFIER                        = L"COL:";
   CWSTR HSB_IDENTIFIER                        = L"HSB:";
   CWSTR GRS_IDENTIFIER                       = L"GRS:";
   CWSTR INEXISTANT_VALUE                     = L"(none)";

   const INT32 SCROLLBAR_DEFAULT_THICKNESS         = 17;
   CWSTR  SCROLLBAR_LT_BUTTON_NAME            = L"ScrollButton_Left_Up";
   CWSTR  SCROLLBAR_RB_BUTTON_NAME            = L"ScrollButton_Right_Down";
   CWSTR  SCROLLBAR_CR_BUTTON_NAME            = L"ScrollButton_Cursor";
   CWSTR  SCROLLBAR_TZ_BUTTON_NAME            = L"ScrollButton_Tracker_Zone";

   const INT32 SCROLLBAR_DEFAULT_CURSOR_EVENT_ZONE = 100;

   const INT32 EMULATE_LEFT_CLICK                  = 27649;
   const INT32 EMULATE_RIGHT_CLICK                 = 29186;

   const INT32 THREAD_POOL_DEFAULT_NUMBER_THREADS  = 10;

   const INT32 c_iServerDefaultPort = 5555;
   const UINT32 c_iClientDefaultPort = 5556;

   //=======================================================================
   //                     VARIOUS MESSAGES
   //=======================================================================
   CWSTR UNKNOWN_OBJECT_TYPE               = L"Unknown base type used in the dialog file, is the definition after the declaration ??? Object type: ";
   CWSTR ERROR_SERVER_ALREADY_RUNNING      = L"Server is already running";
   CWSTR ERROR_CANT_CONNECT_SERVER         = L"Cannot connect to server";
   CWSTR ERROR_CANT_CREATE_GUI_OBJECT      = L"Cannot create GUI Object, is the constructor registered ??? Type : ";
   CWSTR ERROR_LISTBOX_INITIALIZE            = L"Cannot Initialize the listbox object correctly, invalid parameters ??? : ";

   const GColorRGBInt UNSPECIFIED_COLOR = GColorRGBInt(255,0,255,255);

   const UINT32 c_UDPEventGimmeLanGames      = 2948672; // Arbitrary
   const UINT32 c_UDPEventConnect            = 56454; // Arbitrary
   const UINT32 c_UDPEventTimeSyncRequest    = 1;
   const UINT32 c_UDPEventTimeSyncResponse   = 2;
   const UINT32 c_UDPEventTimeSyncTimeChange = 3;
   const UINT32 c_UDPEventTimeSyncSpeedChange= 4;

   const UINT32 c_iAckValue = 0x55AA55AA;

   //Official mod gamespy custom key
   const UINT32 GAMESPY_OFFICIAL_MOD_CKEY_VAL = 50;
   CWSTR GAMESPY_OFFICIAL_MOD_CKEY_NAME       = L"official_mod";
}

#endif //_GL_SDK_CONSTANTS_H_
