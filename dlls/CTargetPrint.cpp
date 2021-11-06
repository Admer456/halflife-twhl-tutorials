#include "extdll.h"
#include "util.h"
#include "cbase.h"

class CTargetPrint : public CBaseEntity
{
	void Use( CBaseEntity* pActivator, CBaseEntity* pOther, USE_TYPE useType, float value ) override;
};

LINK_ENTITY_TO_CLASS( target_print, CTargetPrint );

void CTargetPrint::Use( CBaseEntity* pActivator, CBaseEntity* pOther, USE_TYPE useType, float value )
{
	ALERT( at_console, "%s\n", STRING( pev->netname ) );
}
