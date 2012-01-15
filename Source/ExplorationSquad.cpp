#include "ExplorationSquad.h"
#include "UnitAgent.h"
#include "ExplorationManager.h"
#include "MalRilTil.h"

ExplorationSquad::ExplorationSquad(int mId, string mName, int mPriority)
{
	this->id = mId;
	this->type = EXPLORER;
	this->moveType = GROUND;
	this->name = mName;
	this->priority = mPriority;
	activePriority = priority;
	active = false;
	required = false;
	goalSetFrame = 0;
	currentState = STATE_NOT_SET;
	startLocationsTemp = Broodwar->getStartLocations();
	copy(startLocationsTemp.begin(), startLocationsTemp.end(), inserter(startLocations, startLocations.begin()));
	explorationState = SCOUT_BASE;
	startLocationsExplored.push_back(Broodwar->self()->getStartLocation());
	MalRilTilData::enemyBasePosition = TilePosition(0,0);
}

bool ExplorationSquad::isActive()
{
	return active;
}

void ExplorationSquad::defend(TilePosition mGoal)
{

}

void ExplorationSquad::attack(TilePosition mGoal)
{

}

void ExplorationSquad::assist(TilePosition mGoal)
{

}

void ExplorationSquad::computeActions()
{
int cFrame = Broodwar->getFrameCount();
	if (!active)
	{
		if (isFull())
		{
			active = true;
		}
		return;
	}

	//First, remove dead agents
	for(int i = 0; i < (int)agents.size(); i++)
	{
		if(!agents.at(i)->isAlive())
		{
			this->clearGoal();
			agents.erase(agents.begin() + i);
			i--;
		}
	}

	//All units dead, go back to inactive
	if ((int)agents.size() == 0)
	{
		active = false;
		return;
	}
	
	if (active)
	{
		if(explorationState == SCOUT_BASE)
		{
			if(ExplorationManager::getInstance()->buildingsSpotted())
			{
				MalRilTilData::enemyBasePosition = agents.at(0)->getGoal();
				//Broodwar->printf("Going into expansion scout");
				//Broodwar->printf("Enemy base is at: %d, %d", MalRilTilData::enemyBasePosition.x(), MalRilTilData::enemyBasePosition.y());
				explorationState = SCOUT_EXPANSIONS;
				this->clearGoal();
			}
		}

		TilePosition nGoal;
		if(explorationState == SCOUT_BASE)
		{
			nGoal = this->getNextStartLocation();
		}
		if(explorationState == SCOUT_EXPANSIONS)
		{
			nGoal = ExplorationManager::getInstance()->getNextToExplore(this);
		}
		if (nGoal.x() >= 0)
		{
			//Broodwar->printf("New Goal: %d, %d", this->goal.x(), this->goal.y());
			this->goal = nGoal;
			setMemberGoals(goal);
		}
	}
}
bool ExplorationSquad::checkIfExplored(TilePosition nGoal)
{
	double dist = agents.at(0)->getUnit()->getTilePosition().getDistance(nGoal);
	if(dist <= 10)
	{
		startLocationsExplored.push_back(nGoal);
		Broodwar->printf("CloseEnuff");
		return true;
	}
	return false;
}
bool ExplorationSquad::checkExplored(TilePosition nGoal)
{
	for(int i = 0; i < startLocationsExplored.size(); i++)
	{
		if(nGoal == startLocationsExplored.at(i))
			return true;
	}
	return false;
}

bool ExplorationSquad::enemyBaseExplored()
{
	if(MalRilTilData::enemyBasePosition != TilePosition(0,0))
		return true;
	else
		return false;
}

void ExplorationSquad::printInfo()
{
	string f = "NotFull";
	if (isFull())
	{
		f = "Full";
	}
	string a = "Inactive";
	if (isActive())
	{
		a = "Active";
	}

	Broodwar->printf("[ExplorationSquad %d] (%s, %s) Goal: (%d,%d) prio: %d", id, f.c_str(), a.c_str(), goal.x(), goal.y(), priority);
}

void ExplorationSquad::clearGoal()
{
	
}

TilePosition ExplorationSquad::getGoal()
{
	return goal;
}

bool ExplorationSquad::hasGoal()
{
	if (goal.x() < 0 || goal.y() < 0)
	{
		return false;
	}
	return true;
}
