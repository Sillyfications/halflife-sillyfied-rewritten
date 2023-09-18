#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_radio, CRadio);

void CRadio::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_radio.mdl");
	m_iId = WEAPON_RADIO;
	m_iDefaultAmmo = -1; // How much ammo this weapon has on spawn
	FallInit();			 // get ready to fall down.
}

void CRadio::Precache()
{
	PRECACHE_MODEL("models/v_radio.mdl");
	PRECACHE_MODEL("models/w_radio.mdl");
	PRECACHE_MODEL("models/p_radio.mdl");



	PRECACHE_SOUND("weapons/radiosong.wav");
}

bool CRadio::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;	 // Which ammo type this weapon use
	p->iMaxAmmo1 = NULL; // What's the max ammo quantity for that kind of ammo
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = NULL;
	p->iMaxClip = -1; // How many ammo this weapon's clip or magazine has
	p->iSlot = 5;	  // Which "slot" (column) in the HUD this weapon is located (2 = same slot as HL1 MP5, shotgun, crossbow)
	p->iPosition = 2; // Which "position" (row) in the HUD this weapon is located (3 = after HL1 crossbow)
	p->iFlags = 0;	  // Special flags this weapon has
	p->iId = m_iId = WEAPON_RADIO;
	p->iWeight = MP5_WEIGHT; // How much "priority" this weapon has when auto-switch is triggered

	return true;
}

bool CRadio::Deploy()
{
	//  The last parameter is the animation set for the player model in thirdperson to use
	ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Searching Uplink... Connected to Uplink...");
	return DefaultDeploy("models/v_radio.mdl", "models/p_radio.mdl", RADIO_DRAW, "snark");
	
}

void CRadio::PrimaryAttack()
{


	// Size of the muzzle flash and how much volume in the world the firing sound produces
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_LIGHT;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(0.5);
	Vector vecDir = m_pPlayer->FireBulletsPlayer(0, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, -1, BULLET_PLAYER_MP5,
		1, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	// Play view model animation and firing sound
	SendWeaponAnim(RADIO_SHOOT1);
	
	ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Attempting to upload... Communicating to 932.95 UTF...");
	

	// Next time for attack and weapon idling
	m_flNextPrimaryAttack = 60; // 75 rpm
	ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Uplink reset... Upload cooldown 60 seconds...");
	m_flTimeWeaponIdle = 120;
}

void CRadio::Reload()
{
	return;
}


void CRadio::WeaponIdle()
{
	ResetEmptySound();

	// Not the time to idle
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// Play idle animation
	SendWeaponAnim(RADIO_IDLE);
}
