/**************************************************************
*
* sp2_war.h
*
* Description
* ===========
*  War class used in SP2
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_WAR_H_
#define _GOLEM_WAR_H_

namespace SP2
{	

/*!
 * Class that represents a war between many countries
 **/
class GWar : public GSerializable
{
public:
   GWar();
   ~GWar();
   
	bool						Serialize(GIBuffer&) const;    //!<Fill buffer for network transmission
   bool						Unserialize(GOBuffer&);        //!<Fill object from network reception

	UINT32               ID() const;                    //!<Get ID
   void                 ID(const UINT32);              //!<Set ID
	UINT32					MasterAttacking() const;
	UINT32					MasterDefending() const;
	void						MasterAttacking(UINT32 in_iAttacking);
	void						MasterDefending(UINT32 in_iDefending);
	void						Date(const GDateTime& in_Date);
	const	GDateTime&		Date() const; 
	const set<UINT32>&	AttackingSide() const;
	const set<UINT32>&	DefendingSide() const;
	void						AddCountryToAttackingSide(UINT32);
	void						AddCountryToDefendingSide(UINT32);
	void						RemoveCountryFromAttackingSide(UINT32 in_iAttacking);
	void						RemoveCountryFromDefendingSide(UINT32 in_iDefending);
	bool						MasterAttackingWantsPeace() const;
	bool						MasterDefendingWantsPeace() const;
	void						MasterAttackingWantsPeace(bool in_bWantsPeace);
	void						MasterDefendingWantsPeace(bool in_bWantsPeace);
	
protected:
private:

	UINT32		m_iID;
	UINT32		m_iMasterAttacking;
	UINT32		m_iMasterDefending;
	GDateTime	m_StartOfWar;
	set<UINT32>	m_AttackingSide;
	set<UINT32>	m_DefendingSide;
	bool			m_bMasterAttackingWantsPeace;
	bool			m_bMasterDefendingWantsPeace;

	
};//end class GWar
};//end namespace SP2
#endif //_GOLEM_WAR_H_
