/************************************************
 * File: sp2_sphere_control_window.h
 * Owner: Jonathan Mercier
 * Description: Show Military and Politic control of  a specific region
 *
 *
 * History: 19/04/2004 - File Creation
 * ========
 *
 *************************************************/

#ifndef _CSP2_SphereControlWindow_H_
#define _CSP2_SphereControlWindow_H_

namespace SP2
{

   class GSphereControlWindow : public GUI::GFrame
   {
   public:
      GSphereControlWindow();
      ~GSphereControlWindow();

      bool OnCreate();

      void SetPoliticalControl(const GCountry* in_pCountry);
      void SetMilitaryControl(const GCountry* in_pCountry);
      void SetRegionName(const GString &in_string);

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:

      void UpdateSize();

      GUI::GFrame* m_pMilitaryFrame;
      GUI::GFrame* m_pPoliticalFrame;
      GUI::GLabel* m_pRegionNameLbl;

      GUI::GPictureBox *	m_pMilitaryControlPic;
      GUI::GLabel*			m_pMilitaryLabel;  

      GUI::GPictureBox *	m_pPoliticalControlPic;
      GUI::GLabel*		   m_pPoliticalLabel;

   };

   GUI::GBaseObject* CreateSphereControlWindow();
};

#endif //_CSP2_SphereControlWindow_H_
