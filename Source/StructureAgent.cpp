#include "StructureAgent.h"
#include "AgentManager.h"
#include "BuildPlanner.h"
#include "UpgradesPlanner.h"
#include "Commander.h"
#include "WorkerAgent.h"
#include "ResourceManager.h"
#include "ExplorationManager.h"

#include "MalRilTil.h"

StructureAgent::StructureAgent()
{

}

StructureAgent::StructureAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "StructureAgent";
	//Broodwar->printf("StructureAgent created (%s)", unit->getType().getName().c_str());
}

void StructureAgent::debug_showGoal()
{
	if (!isAlive()) return;

	int w = unit->getType().tileWidth() * 32 / 2;
	Broodwar->drawText(CoordinateType::Map, unit->getPosition().x() - w, unit->getPosition().y() - 10, unit->getType().getName().c_str());

	//Draw "is working" box
	int total = 0;
	int done = 0;
	string txt = "";
	Color cColor = Colors::Blue;
	int bar_h = 18;

	if (unit->isBeingConstructed())
	{
		total = unit->getType().buildTime();
		done = total - unit->getRemainingBuildTime();
		txt = "";
		bar_h = 8;
	}
	if (!unit->isBeingConstructed() && unit->getType().isResourceContainer())
	{
		total = unit->getInitialResources();
		done = unit->getResources();
		txt = "";
		cColor = Colors::Orange;
		bar_h = 8;
	}
	if (unit->isResearching())
	{
		total = unit->getTech().researchTime();
		done = total - unit->getRemainingResearchTime();
		txt = unit->getTech().getName();
	}
	if (unit->isUpgrading())
	{
		total = unit->getUpgrade().upgradeTime();
		done = total - unit->getRemainingUpgradeTime();
		txt = unit->getUpgrade().getName();
	}
	if (unit->isTraining())
	{
		if (unit->getTrainingQueue().size() > 0)
		{
			UnitType t = *(unit->getTrainingQueue().begin());
			total = t.buildTime();
			txt = t.getName();
			done = total - unit->getRemainingTrainTime();
		}
	}

	if (total > 0)
	{
		int w = unit->getType().tileWidth() * 32;
		int h = unit->getType().tileHeight() * 32;

		//Start 
		Position s = Position(unit->getPosition().x() - w/2, unit->getPosition().y() - 30);
		//End
		Position e = Position(s.x() + w, s.y() + bar_h);
		//Progress
		int prg = (int)((double)done / (double)total * w);
		Position p = Position(s.x() + prg, s.y() +  bar_h);

		Broodwar->drawBox(CoordinateType::Map,s.x(),s.y(),e.x(),e.y(),cColor,false);
		Broodwar->drawBox(CoordinateType::Map,s.x(),s.y(),p.x(),p.y(),cColor,true);

		Broodwar->drawText(CoordinateType::Map, s.x() + 5, s.y() + 2, txt.c_str());
	}

	if (!unit->isBeingConstructed() && unit->getType().getID() == UnitTypes::Terran_Bunker.getID())
	{
		int w = unit->getType().tileWidth() * 32;
		int h = unit->getType().tileHeight() * 32;

		Broodwar->drawText(CoordinateType::Map, unit->getPosition().x() - w / 2, unit->getPosition().y() - 10, unit->getType().getName().c_str());

		//Draw Loaded box
		Position a = Position(unit->getPosition().x() - w/2, unit->getPosition().y() - h/2);
		Position b = Position(a.x() + 94, a.y() + 6);
		Broodwar->drawBox(CoordinateType::Map,a.x(),a.y(),b.x(),b.y(),Colors::Green,false);

		if ((int)unit->getLoadedUnits().size() > 0)
		{
			Position a = Position(unit->getPosition().x() - w/2, unit->getPosition().y() - h/2);
			Position b = Position(a.x() + unit->getLoadedUnits().size() * 24, a.y() + 6);

			Broodwar->drawBox(CoordinateType::Map,a.x(),a.y(),b.x(),b.y(),Colors::Green,true);
		}
	}
}

void StructureAgent::computeActions()
{
	if (isAlive())
	{
		//check if we have buildings that need to lift
		if(!unit->isBeingConstructed())
		{	
			if(unit->canIssueCommand(UnitCommand::lift(unit)))
			{
				
				Broodwar->printf("(DBG) can lift");
				int threshold = (int)(unit->getType().maxHitPoints() * 0.25);
				if(unit->getHitPoints() < threshold)
				{
					
					Broodwar->printf("(DBG) less than half health");
					if(unit->getRemainingTrainTime() < (5 * 15)) //**15 = frames
					{
						Broodwar->printf("(DBG) remaining build time");
						unit->cancelTrain();
						unit->lift();
					}
				}
			}
		}
		//check if we have buildings that can land
		if(unit->isLifted())
		{
			int radius = 10 * 32; //in pixels
			bool canLand = true;
			for(set<Unit*>::iterator i = unit->getUnitsInRadius(radius).begin(); i != unit->getUnitsInRadius(radius).end(); i++)
			{
				if((*i)->getPlayer() == Broodwar->enemy())
				{
					canLand = false;
					break;
				}
			}
			if(canLand)
			{
				unit->land(unit->getTilePosition());
			}
			else
			{
				Broodwar->printf("(DBG) won't land: enemies nearby");
			}
		}

		if (!unit->isIdle())
		{
			return;
		}

		if (UpgradesPlanner::getInstance()->checkUpgrade(this))
		{
			return;
		}

		if (BuildPlanner::isTerran())
		{
			//Check addons here
			if (isOfType(UnitTypes::Terran_Science_Facility))
			{
				if (unit->getAddon() == NULL)
				{
					unit->buildAddon(UnitTypes::Terran_Physics_Lab);
				}
			}
			if (isOfType(UnitTypes::Terran_Starport))
			{
				if (unit->getAddon() == NULL)
				{
					unit->buildAddon(UnitTypes::Terran_Control_Tower);
				}
			}
			if (isOfType(UnitTypes::Terran_Starport))
			{
				if (unit->getAddon() == NULL)
				{
					unit->buildAddon(UnitTypes::Terran_Control_Tower);
				}
			}
			if (isOfType(UnitTypes::Terran_Factory))
			{
				if (unit->getAddon() == NULL)
				{
					unit->buildAddon(UnitTypes::Terran_Machine_Shop);
				}
			}

			
			//scanner sweep on potential enemy locations(expansion sites) when having 200 energy
			if(isOfType(UnitTypes::Terran_Comsat_Station))
			{
				if(unit->getEnergy() == 200)
				{
					Broodwar->printf("200 energy reached for a comsat station, scanning with it...");
					vector<TilePosition> expSites = MalRilTilData::expansionPositions; //get expansion locations
					static int expSiteNr = 0;
					TilePosition scanPos = expSites.at(expSiteNr-- % expSites.size()); //next expansion site to scan
					
					//check for every unit if its in range of scan
					vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
					int scanRadius = 12 * 32; //in pixels (tank range = 12)
					for(int j = 0; j < (int)agents.size(); j++)
					{
						if(agents.at(j)->getUnit()->exists())
						{
							int dist = agents.at(j)->getUnit()->getDistance(Position(scanPos)); //distance(in pixels) between unit and scan position
							
							if(dist < scanRadius) //unit is within scanning radius, check next site
							{
								Broodwar->printf("Owned unit within scanning radius, checking next site...");
								scanPos = expSites.at(expSiteNr-- % expSites.size()); //get next expansion site
							}
						}
					}
					unit->useTech(TechTypes::Scanner_Sweep, Position(scanPos));
					Broodwar->printf("Scan used at: %d, %d", scanPos.x(), scanPos.y());
				}
			}

			/*If Comsat Station, check if we need to scan for enemy bases
			if (isOfType(UnitTypes::Terran_Comsat_Station))
			{
				TilePosition p = getNextScanLocation();
				if (p.x() != -1)
				{
					if (unit->getEnergy() >= 50)
					{
						//Broodwar->printf("Scanning (%d,%d)", p.x(), p.y());
						if (unit->useTech(TechTypes::Scanner_Sweep, Position(p)))
						{
							hasScanned.push_back(p);
							return;
						}
					}
				}
			}*/


			
		}

		if (!unit->isBeingConstructed() && unit->isIdle() && getUnit()->getType().canProduce())
		{
			//Iterate through all unit types
			for(set<UnitType>::iterator i=UnitTypes::allUnitTypes().begin();i!=UnitTypes::allUnitTypes().end();i++)
			{
				//Check if we can (and need) to build the unit
				if (canBuildUnit(*i))
				{
					//Build it!
					unit->train(*i);
				}
			}
		}
	}
}

bool StructureAgent::canBuildUnit(UnitType type)
{
	//1. Check if race matches
	if (type.getRace().getID() != Broodwar->self()->getRace().getID())
	{
		return false;
	}

	//2. Check if this unit can construct the unit
	pair<UnitType, int> builder = type.whatBuilds();
	if (builder.first.getID() != unit->getType().getID())
	{
		return false;
	}

	//3. Check if we need the unit in a squad
	if (!Commander::getInstance()->needUnit(type))
	{
		return false;
	}
	
	//4. Check canBuild
	if (!Broodwar->canMake(unit, type))
	{
		return false;
	}

	//5. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}

	//All clear. Build the unit.
	return true;
}

void StructureAgent::printInfo()
{
	Broodwar->printf("[%s-%d]", agentType.c_str(), unitID);
}

void StructureAgent::sendWorkers()
{
	//We have constructed a new base. Make some workers move here.
	int noWorkers = AgentManager::getInstance()->getNoWorkers();
	int toSend = noWorkers / AgentManager::getInstance()->countNoBases();
	int hasSent = 0;

	//Broodwar->printf("Sending %d/%d workers to new base", toSend, noWorkers);
	vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive() && agent->isFreeWorker())
		{
			Unit* worker = agent->getUnit();
			WorkerAgent* wa = (WorkerAgent*)agent;
			worker->rightClick(unit->getPosition());
			hasSent++;
		}

		if (hasSent >= toSend)
		{
			return;
		}
	}
}

bool StructureAgent::canMorphInto(UnitType type)
{
	//1. Check canBuild
	if (!Broodwar->canMake(unit, type))
	{
		return false;
	}
	
	//2. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}

	//3. Check if unit is already morphing
	if (unit->isMorphing())
	{
		return false;
	}

	//All clear. Build the unit.
	return true;
}

bool StructureAgent::canEvolveUnit(UnitType type)
{
	//1. Check if we need the unit in a squad
	if (!Commander::getInstance()->needUnit(type))
	{
		return false;
	}

	//2. Check canBuild
	if (!Broodwar->canMake(unit, type))
	{
		return false;
	}

	//3. Check if we have enough resources
	if (!ResourceManager::getInstance()->hasResources(type))
	{
		return false;
	}

	//All clear. Build the unit.
	return true;
}

TilePosition StructureAgent::getNextScanLocation()
{
	TilePosition ePos = ExplorationManager::getInstance()->getClosestSpottedBuilding(Broodwar->self()->getStartLocation());
	if (ePos.x() > -1)
	{
		//Already found enemy base
		return TilePosition(-1, -1);
	}

	for(set<BaseLocation*>::const_iterator i=getStartLocations().begin();i!=getStartLocations().end();i++)
	{
		TilePosition basePos = (*i)->getTilePosition();

		bool needScan = true;

		//1. Check previous scans
		for (int i = 0; i < (int)hasScanned.size(); i++)
		{
			if (hasScanned.at(i).x() == basePos.x() && hasScanned.at(i).y() == basePos.y())
			{
				needScan = false;
			}
		}

		//2. Check if we have this base
		vector<BaseAgent*> agents = AgentManager::getInstance()->getAgents();
		for (int i = 0; i < (int)agents.size(); i++)
		{
			if (agents.at(i)->isAlive())
			{
				double dist = basePos.getDistance(agents.at(i)->getUnit()->getTilePosition());
				if (dist <= 10)
				{
					needScan = false;
					break;
				}
			}
		}

		//3. Check if enemy units are near
		for(set<Unit*>::const_iterator j=Broodwar->enemy()->getUnits().begin();j!=Broodwar->enemy()->getUnits().end();j++)
		{
			if ((*j)->exists())
			{
				double dist = basePos.getDistance((*j)->getTilePosition());
				if (dist <= 10)
				{
					needScan = false;
					break;
				}
			}
		}

		if (needScan)
		{
			return basePos;
		}
	}
	return TilePosition(-1, -1);
}
