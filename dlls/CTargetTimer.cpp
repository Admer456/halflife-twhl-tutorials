
#include "extdll.h"
#include "util.h"
#include "cbase.h"

#define SF_TIMER_START_ON 1

class CTargetTimer : public CBaseEntity
{
public:
	void		Spawn() override;
	void		KeyValue( KeyValueData* pkvd ) override;

	int			Save( CSave& save );
	int			Restore( CRestore& restore );
	static TYPEDESCRIPTION m_SaveData[];

	void		Use( CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value ) override;
	void EXPORT	TimerThink();

	bool		IsEnabled() const;

private:
	CBaseEntity* m_pActivator = nullptr;
	float		m_flTimerDelay = 1.0f; // 1s of delay by default
	float		m_flRandomMax = 0.0f;
};

LINK_ENTITY_TO_CLASS( target_timer, CTargetTimer );

TYPEDESCRIPTION CTargetTimer::m_SaveData[] =
{
	DEFINE_FIELD( CTargetTimer, m_flTimerDelay, FIELD_FLOAT ),
	DEFINE_FIELD( CTargetTimer, m_flRandomMax, FIELD_FLOAT ),
	DEFINE_FIELD( CTargetTimer, m_pActivator, FIELD_CLASSPTR )
};

IMPLEMENT_SAVERESTORE( CTargetTimer, CBaseEntity );

void CTargetTimer::Spawn()
{
	SetThink( nullptr );

	if ( pev->spawnflags & SF_TIMER_START_ON )
	{
		// This will simply turn on the entity, as
		// if it triggered itself
		Use( this, this, USE_TOGGLE, 0.0f );
	}
	else if ( !pev->targetname )
	{
		ALERT( at_console, "target_timer with no name, deleting...\n" );
		UTIL_Remove( this );
	}
}

void CTargetTimer::KeyValue( KeyValueData* pkvd )
{
	// Keyvalue parsing works by comparing the current keyvalue's name
	// to the keyvalue name we are looking for, then we parse the value itself
	if ( FStrEq( pkvd->szKeyName, "delay" ) )
	{
		m_flTimerDelay = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "random" ) )
	{
		m_flRandomMax = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
	{	// In case no matching keyvalues are found, we must pass it on
		// to the class we inherited from
		CBaseEntity::KeyValue( pkvd );
	}
}

void CTargetTimer::Use( CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value )
{
	if ( IsEnabled() )
	{
		SetThink( nullptr );
	}
	else
	{
		SetThink( &CTargetTimer::TimerThink );
	}

	pev->nextthink = gpGlobals->time + m_flTimerDelay;
}

void CTargetTimer::TimerThink()
{
	SUB_UseTargets( m_pActivator, USE_TOGGLE, 0.0f );

	pev->nextthink = gpGlobals->time + m_flTimerDelay + RANDOM_FLOAT( -m_flRandomMax, m_flRandomMax );
}

bool CTargetTimer::IsEnabled() const
{
	// We could've used a BOOL variable to keep track of 
	// the entity's status, but there's a smarter way
	return m_pfnThink != nullptr;
}

