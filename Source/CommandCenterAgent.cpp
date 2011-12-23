#include "CommandCenterAgent.h"
#include "AgentManager.h"
#include "WorkerAgent.h"
#include "PFManager.h"
#include "BuildPlanner.h"
#include "ExplorationManager.h"
#include "ResourceManager.h"

CommandCenterAgent::CommandCenterAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	//Broodwar->printf("New base created at (%d,%d)", unit->getTilePosition().x(), unit->getTilePosition().y());

	hasSentWorkers = false;
	if (AgentManager::getInstance()->countNoUnits(UnitTypes::Terran_Command_Center) == 0)
	{
		//We dont do this for the first Command Center.
		hasSentWorkers = true;
	}

	agentType = "CommandCenterAgent";
	BuildPlanner::getInstance()->commandCenterBuilt();
}

void CommandCenterAgent::computeActions()
{
	//check if command center is being attacked	***NOT TESTED***
	if(this->isUnderAttack())
	{
		Broodwar->printf("Command center under attack, attacking with nearby units...");
		int radius = 10;
		for(set<Unit*>::const_iterator i = this->getUnit()->getUnitsInRadius(radius).begin(); i != this->getUnit()->getUnitsInRadius(radius).end(); i++)
		{
			if((*i)->exists())
			{
				if((*i)->getPlayer() == Broodwar->enemy()) //find enemy
				{
					if(!(*i)->getType().isFlyer())
					{
						if((*i)->isCloaked() || (*i)->isBurrowed() || !(*i)->isVisible())
						{
							if(!(*i)->isDetected())
							{
								Broodwar->printf("(DBG) enemy hitting CC is invisible AND not detected, scanning...");
								AgentManager::getInstance()->getAgent(0)->doScannerSweep((*i)->getTilePosition());
							}
						}
					}
				}
				else //order our units to attack
				{
					(*i)->attack(Position(this->getUnit()->getTilePosition()));
				}

			}
		}
	}

	if (!hasSentWorkers)
	{
		if (!unit->isBeingConstructed())
		{
			sendWorkers();
			hasSentWorkers = true;

			BuildPlanner::getInstance()->addRefinery();

			if (AgentManager::getInstance()->countNoUnits(UnitTypes::Terran_Barracks) > 0)
			{
				BuildPlanner::getInstance()->addBuildingFirst(UnitTypes::Terran_Bunker);
			}
			if (AgentManager::getInstance()->countNoUnits(UnitTypes::Terran_Engineering_Bay) > 0)
			{
				BuildPlanner::getInstance()->addBuildingFirst(UnitTypes::Terran_Missile_Turret);
			}
		}
	}

	if (!unit->isIdle())
	{
		//Already doing something
		return;
	}

	//Build comsat addon
	if (unit->getAddon() == NULL)
	{
		if (Broodwar->canMake(unit, UnitTypes::Terran_Comsat_Station))
		{
			unit->buildAddon(UnitTypes::Terran_Comsat_Station);
			return;
		}
	}

	if (ResourceManager::getInstance()->needWorker())
	{
		UnitType worker = Broodwar->self()->getRace().getWorker();
		if (canBuild(worker))
		{
			unit->train(worker);
		}
	}
}
