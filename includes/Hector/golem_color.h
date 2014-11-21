/**************************************************************
*
* golem_color.h
*
* Description
* ===========
*  Describes & implements classes GColor, GColorRGBInt,
*  GColorRGBReal, GColorRGB, GColorHSBReal, GColorHSReal
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_COLOR_H_
#define _GOLEM_COLOR_H_

#include "golem_types.h"
#include "golem_memory_util.h"

namespace Hector
{

   //! Describes RGBA colors in integer
   /*!
    * Class that describes a color with RGBA (red, green, blue & alpha) components using 
    * 8 bits unsigned integers for each component. This class content is the same as the 
    * PIXELFMT_A8R8G8B8 pixel format.
    **/
   class GColorRGBInt
   {
   public:
      //! Blue component
      UINT8    B;

      //! Green component
      UINT8    G;

      //! Red component
      UINT8    R;

      //! Alpha component
      UINT8    A;

      //! Default constructor (empty)
      inline GColorRGBInt()
      {
      }

      //! Copy constructor
      GColorRGBInt(const GColorRGBInt& in_Other);

      //! Initialization constructor
      GColorRGBInt(UINT8 in_iRed, UINT8 in_iGreen, UINT8 in_iBlue, UINT8 in_iAlpha);

      //! Addition operator overload
      GColorRGBInt operator + ( const GColorRGBInt& in_Other ) const;

      //! Substract operator overload
      GColorRGBInt operator - ( const GColorRGBInt& in_Other ) const;

      //! Multiplication operator overload
      GColorRGBInt operator * ( const GColorRGBInt& in_Other ) const;

      //! Multiplication with scalar operator overload
      GColorRGBInt operator * ( UINT8 in_iValue ) const;

      //! Assignation operator overload
      GColorRGBInt& operator = ( const GColorRGBInt& in_Other );

      //! Assignation addition operator overload
      GColorRGBInt& operator += ( const GColorRGBInt& in_Other );

      //! Assignation substract operator overload
      GColorRGBInt& operator -= ( const GColorRGBInt& in_Other );

      //! Assignation multiplication operator overload
      GColorRGBInt& operator *= ( const GColorRGBInt& in_Other );

      //! Assignation multiplication with scalar operator overload
      GColorRGBInt& operator *= ( UINT8 in_iValue );

      //! Comparison operator overload
      bool operator == ( const GColorRGBInt& in_Other ) const;

      //! Inverse comparison operator overload
      bool operator != ( const GColorRGBInt& in_Other ) const;
   };

   //! Describes RGBA colors in floating point
   /*!
    * Class that describes a color with RGBA (red, green, blue & alpha) components using 
    * 32 bits floating point numbers for each component. This class content is the same 
    * as the PIXELFMT_A32B32G32R32F pixel format.
    **/
   class GColorRGBReal
   {
   public:
      //! Red component
      REAL32    R;

      //! Green component
      REAL32    G;

      //! Blue component
      REAL32    B;

      //! Alpha component
      REAL32    A;

      //! Default constructor (empty)
      inline GColorRGBReal()
      {
      }

      //! Copy constructor
      GColorRGBReal(const GColorRGBReal& in_Other);

      //! Constructor from integer color
      GColorRGBReal(const GColorRGBInt& in_Other);

      //! Initialization constructor
      GColorRGBReal(REAL32 in_fRed, REAL32 in_fGreen, REAL32 in_fBlue, REAL32 in_fAlpha);

      //! Constructor from single value
      GColorRGBReal(REAL32 in_fValue);

      //! Casting to integer color overload
      operator GColorRGBInt () const;

      //! Clamp color value to the range 0-1
      void Clamp();

      //! Addition operator overload
      GColorRGBReal operator + ( const GColorRGBReal& in_Other ) const;

      //! Substract operator overload
      GColorRGBReal operator - ( const GColorRGBReal& in_Other ) const;

      //! Multiplication operator overload
      GColorRGBReal operator * ( const GColorRGBReal& in_Other ) const;

      //! Multiplication with scalar operator overload
      GColorRGBReal operator * ( REAL32 in_fFactor ) const;

      //! Assignation operator overload
      GColorRGBReal& operator = ( const GColorRGBReal& in_Other );

      //! Assignation addition operator overload
      GColorRGBReal& operator += ( const GColorRGBReal& in_Other );

      //! Assignation substract operator overload
      GColorRGBReal& operator -= ( const GColorRGBReal& in_Other );

      //! Assignation multiplication operator overload
      GColorRGBReal& operator *= ( const GColorRGBReal& in_Other );

      //! Assignation multiplication with scalar operator overload
      GColorRGBReal& operator *= ( REAL32 in_fFactor );

      //! Access component operator overload
      REAL32& operator [] ( UINT32 in_iIndex );

      //! Comparison operator overload
      bool operator == ( const GColorRGBReal& in_Other ) const;

      //! Inverse comparison operator overload
      bool operator != ( const GColorRGBReal& in_Other ) const;
   };

   //! Describes HSB colors in floating point
   /*!
    * Class that describes a color with HSB (hue, saturation & brillance) 
    * components using 32 bits floating point numbers for each component.
    * HSB is the same as HSL (hue, saturation & luminance)
    **/
   class GColorHSBReal
   {
   public:
      //! Hue component
      REAL32 H;

      //! Saturation component
      REAL32 S;

      //! Brillance (luminance) component
      REAL32 B;

      //! Default constructor (empty)
      inline GColorHSBReal()
      {
      }

      //! Initialization constructor
      GColorHSBReal(REAL32 in_fHue, REAL32 in_fSaturation, REAL32 in_fBrillance);

      //! Casting to integer color overload
      operator GColorRGBInt () const;

   private:
      REAL32 HueToRGB(REAL32 in_fNumber1,REAL32 in_fNumber2 ,REAL32 in_fHue) const;
   };

   //! Lists types of color
   /*!
      * Lists all types of color that can be described by the GColor class
      **/
   namespace EColorType { enum ENUM
   { 
      COLOR_UNKNOWN = 0,      //!< No or unknown color type
      COLOR_RGB,              //!< RGB color type
      COLOR_HSB,              //!< HSB color type
      COLOR_HS,               //!< HS color type
   };}

   //! Describes HS colors in floating point
   /*!
    * Class that describes a color with HS (hue, saturation)
    * components using 32 bits floating point numbers for each component.
    * This is useful to describe a color without its intensity.
    **/
   class GColorHSReal
   {
   public:
      //! Hue component
      REAL32 H;

      //! Saturation component
      REAL32 S;

      //! Default constructor (empty)
      GColorHSReal()
      {
      }

      //! Initialization constructor
      GColorHSReal(REAL32 in_hHue, REAL32 in_fSaturation);

      //! Casting to integer color overload
      operator GColorRGBInt () const;
   };

   //! Unify all color types
   /*!
    * Class that is used to unify all the color types under one single type.
    * Only one color is being held at the time, so only one of the members is 
    * valid at all times.
    **/
   class GColor
   {
   public:
      //! Default constructor (empty)
      GColor()
      {
         m_Type = EColorType::COLOR_UNKNOWN;
      }

      //! Initialization constructor from RGB color
      GColor(GColorRGBReal in_Color);

      //! Initialization constructor from HSB color
      GColor(GColorHSBReal in_Color);

      //! Initialization constructor from HS color
      GColor(GColorHSReal  in_Color);

      //! RGB color component
      GColorRGBReal     m_RGBColor;

      //! HSB color component
      GColorHSBReal     m_HSBColor;

      //! HS color component
      GColorHSReal      m_HSColor;

      //! Returns the type of color described by this object
      EColorType::ENUM Type();

      //! Convert the contained color to an integer RGB color
      GColorRGBInt  ConvertToInt();

      //! Convert the contained color to a floating point RGB color
      GColorRGBReal ConvertToReal();

   private:
      EColorType::ENUM m_Type;
   };

}

#endif //#ifndef _GOLEM_COLOR_H_
