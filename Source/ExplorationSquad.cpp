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
	goal = Broodwar->self()->getStartLocation();
	goalSetFrame = 0;
	currentState = STATE_NOT_SET;
	this->startLocations = Broodwar->getStartLocations();
	this->enemyBaseExplored = false;
	this->timesSenseExploredBase = 0;
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
		///////////////////////////////////////////////
		if((int)agents.size() < 1)
			{
				this->startLocations.clear();
				this->clearGoal();
				this->timesSenseExploredBase = cFrame;
				TilePosition nGoal = ExplorationManager::getInstance()->getNextToExplore(this);
				if (nGoal.x() >= 0)
				{
					this->goal = nGoal;
					setMemberGoals(this->goal);
				}
				//Broodwar->printf("My current goal are: %d, %d", this->goal.x(), this->goal.y());
			}
		////////////////////////////////////////////////////////////////////


		active = false;
		return;
	}
	
	if (active)
	{
		TilePosition nGoal;
		if(cFrame - this->timesSenseExploredBase > 5000)
		{
			//Broodwar->printf("Lets check out that base again");
			this->startLocations = Broodwar->getStartLocations();
			this->timesSenseExploredBase = cFrame;
		}
		if (activePriority != priority)
		{
			priority = activePriority;
		}

		if(startLocations.size() == 0)
		{
			nGoal = ExplorationManager::getInstance()->getNextToExplore(this);
		}
		else
		{
			set<TilePosition>::iterator go;
			go=startLocations.begin();
			startLocations.erase(go);
			if(*go != Broodwar->self()->getStartLocation())
			{
				MalRilTilData::enemyBasePosition = *go;
				nGoal = *go;
			}
			else
			{
				nGoal = ExplorationManager::getInstance()->getNextToExplore(this);
			}
		}
		if (nGoal.x() >= 0)
		{
			//Broodwar->printf("New Goal: %d, %d", this->goal.x(), this->goal.y());
			this->goal = nGoal;
			setMemberGoals(goal);
		}
	}
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
