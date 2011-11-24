#include "SiegeTankAgent.h"
#include "AgentManager.h"
#include "PFManager.h"
#include "TargetingAgent.h"

SiegeTankAgent::SiegeTankAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "SiegeTankAgent";
	//Broodwar->printf("SiegeTankAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void SiegeTankAgent::computeActions()
{
	bool defensive = false;
	if(this->enemyGroundUnitsWithinRange(400) > 3)
	{
		if(!unit->isSieged())
		{
			unit->siege();
			//Broodwar->printf("Siege'ing");
		}
	}
	else if(this->enemyGroundUnitsWithinRange(500) == 0)
	{
		if(unit->isSieged())
		{
			unit->unsiege();
			//Broodwar->printf("UnSiege'ing");
		}
	}
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
