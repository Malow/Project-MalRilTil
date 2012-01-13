#include "MedicAgent.h"
#include "PFManager.h"
#include "AgentManager.h"

MedicAgent::MedicAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "MedicAgent";
	//Broodwar->printf("MedicAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void MedicAgent::computeActions()
{
	//check if restoration has been researched
	if(Broodwar->self()->hasResearched(TechTypes::Restoration))
	{
		//check if medic has enough mana
		if(unit->getEnergy() >= TechTypes::Restoration.energyUsed())
		{
			//get units in range of restoration
			int radius = 6 * 32; //in pixels (6 = range of spell, 32 = pixels/radiusunit)
			for(set<Unit*>::iterator i = unit->getUnitsInRadius(radius).begin(); i != unit->getUnitsInRadius(radius).end(); i++)
			{
				if((*i)->exists())
				{
					//check if it's one of our units
					if((*i)->getPlayer() == Broodwar->self())
					{
						//check if it's an important unit(battlecruiser/science vessel/siege tank(both modes))
						if((*i)->getType() == UnitTypes::Terran_Battlecruiser || (*i)->getType() == UnitTypes::Terran_Science_Vessel || (*i)->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || (*i)->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode)
						{
							//check if it is plagued/irridiated or locked down
							if((*i)->isPlagued() || (*i)->isIrradiated() || (*i)->isLockedDown())
							{
								unit->useTech(TechTypes::Restoration, (*i));
							}
						}
					}
				}
			}
		}
	}


	bool defensive = true;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}

