
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

class CFuncSlidicle : public CBaseEntity
{
public:
	void Spawn() override;
	
	int ObjectCaps() override { return CBaseEntity::ObjectCaps() | FCAP_IMPULSE_USE; }

	void Use( CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value ) override;
	void Think() override;

private:
	CBasePlayer* driver = nullptr;
};

LINK_ENTITY_TO_CLASS( func_slidicle, CFuncSlidicle );

void CFuncSlidicle::Spawn()
{
	pev->movetype = MOVETYPE_PUSHSTEP;
	pev->solid = SOLID_BSP;
	SET_MODEL( edict(), STRING( pev->model ) );
}

void CFuncSlidicle::Use( CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value )
{
	if ( !pActivator->IsPlayer() )
	{
		return;
	}

	// A potential driver wants to enter this vehicle
	if ( !driver )
	{
		driver = static_cast<CBasePlayer*>(pActivator);
		// Hack: lock the player in place
		driver->pev->maxspeed = 0.01f;

		// Think immediately
		pev->nextthink = gpGlobals->time + 0.01f;
		return;
	}
	
	// The current driver wants to eject
	if ( driver && pActivator == driver )
	{
		driver->pev->maxspeed = 0.0f;
		driver = nullptr;
	}
}

void CFuncSlidicle::Think()
{
	// If we got no driver, then we got nothing
	if ( !driver )
	{
		return;
	}

	constexpr float VehicleAcceleration = 16.0f;

	const int& buttons = driver->pev->button;
	Vector addVelocity = vec3_origin;

#if 1
	if ( buttons & IN_FORWARD )
		addVelocity.x += VehicleAcceleration;

	if ( buttons & IN_BACK )
		addVelocity.x -= VehicleAcceleration;
	
	if ( buttons & IN_MOVELEFT )
		addVelocity.y += VehicleAcceleration;
	
	if ( buttons & IN_MOVERIGHT )
		addVelocity.y -= VehicleAcceleration;
#else
	if ( buttons & IN_MOVELEFT )
		pev->angles.y += 1.0f;
	if ( buttons & IN_MOVERIGHT )
		pev->angles.y -= 1.0f;

	UTIL_MakeVectors( pev->angles );

	if ( buttons & IN_FORWARD )
		addVelocity = addVelocity + gpGlobals->v_forward * VehicleAcceleration;

	if ( buttons & IN_BACK )
		addVelocity = addVelocity - gpGlobals->v_forward * VehicleAcceleration;
#endif

	pev->velocity = pev->velocity + addVelocity;
	pev->nextthink = gpGlobals->time + 0.01f;
}
