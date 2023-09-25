//HALF-LIFE: Sillyfied weapon code
//Fallout New Vegas Radio
//This radio tries to connect to the broadcasting stations in the Mojave Wasteland.
//This weapon is classified as an equipment, since it does not do any damage.


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
	m_iDefaultAmmo = -1; // this doesnt use ammo, because it doesnt shoot anything????
	FallInit();			 // gravity bitch!
}

void CRadio::Precache()
{
	PRECACHE_MODEL("models/v_radio.mdl");
	PRECACHE_MODEL("models/w_radio.mdl");
	PRECACHE_MODEL("models/p_radio.mdl");


	// the songs do not play via code due to not being dynamic... dynamic sounds DO work on the QC file that the MDL model comes with
	PRECACHE_SOUND("weapons/radiosong.wav");
	PRECACHE_SOUND("weapons/radiosong2.wav");
	PRECACHE_SOUND("weapons/radiosong3.wav");

}

bool CRadio::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;	 // Which ammo type this weapon use
	p->iMaxAmmo1 = NULL; // What's the max ammo quantity for that kind of ammo
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = NULL;
	p->iMaxClip = -1; // How many ammo this weapon's clip or magazine has
	p->iSlot = 0;	  // Which "slot" (column) in the HUD this weapon is located (2 = same slot as HL1 MP5, shotgun, crossbow)
	p->iPosition = 1; // Which "position" (row) in the HUD this weapon is located (3 = after HL1 crossbow)
	p->iFlags = 0;	  // Special flags this weapon has
	p->iId = m_iId = WEAPON_RADIO;
	p->iWeight = MP5_WEIGHT; // How much "priority" this weapon has when auto-switch is triggered

	return true;
}

bool CRadio::Deploy()
{
	//  The last parameter is the animation set for the player model in thirdperson to use
	ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Searching nearby frequencies.... 2 found....");
	ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, ">Press Primary Attack to communicate to 932.95 UTF.");
	ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, ">Press Secondary Attack to communicate to 314.28 UTF.");
	return DefaultDeploy("models/v_radio.mdl", "models/p_radio.mdl", RADIO_DRAW, "snark");
	
}


//this plays Heartaches by the Number
void CRadio::PrimaryAttack()
{


	// i dont know if i have to remove part or not, just keeping it just in case bleh 
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_LIGHT;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(0.5);
	Vector vecDir = m_pPlayer->FireBulletsPlayer(0, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, -1, BULLET_PLAYER_MP5,
		1, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	// play viewmodel anim
	SendWeaponAnim(RADIO_SHOOT1);
	
	ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Attempting to request data... Communicating to 932.95 UTF...");
	

	
	m_flNextPrimaryAttack = 30;//to prevent song overlap
	ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Connected! Playing \"Heartaches by the Number\" in 5 seconds.");
	m_flTimeWeaponIdle = 120;
}

// this plays Lone Star
void CRadio::SecondaryAttack()
{


	// i dont know if i have to remove part or not, just keeping it just in case
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_LIGHT;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(0.5);
	Vector vecDir = m_pPlayer->FireBulletsPlayer(0, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, -1, BULLET_PLAYER_MP5,
		1, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	// play viewmodel anim 
	SendWeaponAnim(RADIO_SHOOT2);

	ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Attempting to request data... Communicating to 314.28 UTF...");

	m_flNextSecondaryAttack = 30; // to prevent song overlap
	ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Connected! Playing \"Let's Ride Into The Sunset Together\" in 5 seconds..");
	m_flTimeWeaponIdle = 120;
}

// this plays Let's Ride Into The Sunset Together
// this doesnt actually reload the gun, just a clever hack to not add a new attack button
// the old tuts doesnt support this verson of the code, which THIS SUCKS!!!
//code didnt work FUCK YOU GO KILL YOURSELF
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
