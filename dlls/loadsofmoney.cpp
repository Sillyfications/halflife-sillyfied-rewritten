// Half-Life: Sillyfied Loads Of Money

// A Stack of 200 TL
// The Turkish Lire is becoming more worthless everyday, why not make it into a weapon? This weapon increased in damage every swing
// Ammo: None
// Damage: 5 probably and the rest,10 l,15 o,20 d, 25 s, 30 of, 35 e, 40 m, 45 o, 50 n, 55 e, what's that spell?, 100 LOADSAMONEY!
// Firerate: 60
// Magazine: None
// DPS: Can't really measure
// CND: No CND
// Accuracy: Accurate
// Recoil: No recoil

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_loadsofmoney, CLoadsOfMoney);

void CLoadsOfMoney::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_usp.mdl");
	m_iId = WEAPON_LOADSOFMONEY;
	FallInit();
}

void CLoadsOfMoney::Precache()
{
	PRECACHE_MODEL("models/v_money.mdl");
	PRECACHE_MODEL("models/w_usp.mdl");
	PRECACHE_MODEL("models/p_usp.mdl");
	
	//cache all the draw sounds
	PRECACHE_SOUND("weapons/loadsofmoney/deploy1.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/deploy2.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/deploy3.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/deploy4.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/deploy5.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/deploy6.wav");

	//cache the attack sounds
	PRECACHE_SOUND("weapons/loadsofmoney/l.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/o.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/d.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/s.wav");

	PRECACHE_SOUND("weapons/loadsofmoney/of.wav");

	PRECACHE_SOUND("weapons/loadsofmoney/e1.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/m.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/o.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/n.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/e2.wav");

	PRECACHE_SOUND("weapons/loadsofmoney/whatsthatspell.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/loadsamoney.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/probably.wav");

	PRECACHE_SOUND("weapons/loadsofmoney/seq1.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/seq2.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/seq3.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/seq4.wav");
	PRECACHE_SOUND("weapons/loadsofmoney/seq5.wav");
	
}

bool CLoadsOfMoney::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;		
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = -1;				 // 2 barrels
	p->iSlot = 0;					 // 2 = smg, shotgun, crossbow (BUG! sprite doesn't show in game? gotta ask for help)
	p->iPosition = 2;				 // below m40a1 scopeless		(BUG! same here)
	p->iFlags = 0;					 // no idea
	p->iId = m_iId = WEAPON_LOADSOFMONEY; // errm errm also no idea but must be important
	p->iWeight = MP5_WEIGHT;

	return true;
}

bool CLoadsOfMoney::Deploy()
{
	return DefaultDeploy("models/v_money.mdl", "models/p_usp.mdl", LOADSOFMONEY_DRAW, "crowbar");

	switch (m_LoadsOfMoneySequenceIncrementer)
	{
	case 0:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy1.wav", 1, ATTN_NORM);
		break;
	case 1:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy2.wav", 1, ATTN_NORM);
		break;
	case 2:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy3.wav", 1, ATTN_NORM);
		break;
	case 3:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy4.wav", 1, ATTN_NORM);
		break;
	case 4:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy5.wav", 1, ATTN_NORM);
		break;
	case 5:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy6.wav", 1, ATTN_NORM);
		break;
	case 6:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy1.wav", 1, ATTN_NORM);
		break;
	case 7:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy2.wav", 1, ATTN_NORM);
		break;
	case 8:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy3.wav", 1, ATTN_NORM);
		break;
	case 9:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy4.wav", 1, ATTN_NORM);
		break;
	case 10:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy5.wav", 1, ATTN_NORM);
		break;
	case 11:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy6.wav", 1, ATTN_NORM);
		break;
	case 12:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy1.wav", 1, ATTN_NORM);
		break;
	case 13:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy2.wav", 1, ATTN_NORM);
		break;
	case 14:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy3.wav", 1, ATTN_NORM);
		break;
	case 15:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy4.wav", 1, ATTN_NORM);
		break;
	case 16:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy5.wav", 1, ATTN_NORM);
		break;
	case 17:
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/deploy6.wav", 1, ATTN_NORM);
		break;
		

	}
}

// tweak some stuff before release
void CLoadsOfMoney::PrimaryAttack()
{
	

	// nuzzle flash and sound volume
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	switch (m_LoadsOfMoneySequenceIncrementer)
	{
	case 0:
	{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 10, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/l.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "L\n10 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
	}
		break;
	case 1:
		{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 15, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/o.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "O\n15 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
		}
		break;
	case 2:
		{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 20, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/d.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "D\n20 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
		}
			break;
	case 3:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 25, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/s.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "S\n25 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break;//LODS
	case 4:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 30, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/of.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "OF\n30 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break;//OF
	case 5:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 35, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/e1.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "E\n35 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break; 
	case 6:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 40, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/m.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "M\n40 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break; 
	case 7:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 45, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/o.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "O\n45 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break; 
	case 8:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 50, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/n.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "N\n50 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break; 
	case 9:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 55, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/e1.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "E\n55 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break;
	case 10:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 60, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/whatsthatspell.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "What's that spell?\n60 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break; 
	case 11:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 100, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/loadsamoney.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Loadsamoney!\n100 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break;
	case 12:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 5, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/probably.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Probably...\n5 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break; 
	case 13:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 5, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/seq1.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Right, let's do up the house\n5 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break; 
	case 14:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 5, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/seq2.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Bosh bosh, shoom shoom wallop\n5 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break; 
	case 15:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 5, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/seq3.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Dosh!\n5 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break; 
	case 16:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 5, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/seq4.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Bosh bosh, shoom shoom wallop\n5 damage.");
		m_LoadsOfMoneySequenceIncrementer++;
			}
			break;
	case 17:
			{
		Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_CROWBAR, 1, 5, m_pPlayer->pev, m_pPlayer->random_seed);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/loadsofmoney/seq5.wav", 1, ATTN_NORM);
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Dosh!\n5 damage.");
		m_LoadsOfMoneySequenceIncrementer = 0;
			}
			break;
    }
	

	// play view model animation and firing sound
	SendWeaponAnim(LOADSOFMONEY_SHOOT1 + RANDOM_LONG(0, 1));
	

	m_flNextPrimaryAttack = 1;
	m_flTimeWeaponIdle = 1.85;
}



void CLoadsOfMoney::Reload()
{
	return;
}


void CLoadsOfMoney::WeaponIdle()
{
	ResetEmptySound();


	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;


	SendWeaponAnim(LOADSOFMONEY_IDLE);
}

void CLoadsOfMoney::Holster()
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim(LOADSOFMONEY_HOLSTER);
}
