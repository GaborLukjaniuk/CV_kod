#pragma once

#include "Simulation.h"

Simulation::Simulation(Map* simMap, double randEventProb,  int minSafeSpace, bool shuffleIfCompetition) : simMap(simMap), randEventProb(randEventProb), minSafeSpace(minSafeSpace), shuffleIfCompetition(shuffleIfCompetition), randomEngine(std::default_random_engine(0)) {
	if(randEventProb < 0.0 || randEventProb > 1.0)
		throw std::invalid_argument("Random event probability must be in range between 0 and 1");
}

Map* Simulation::getSimulationMap() {
	return simMap;
}

void Simulation::initiateSimulation() {
	cellsWithVehs = simMap->getCellsWithVehs();
	simMap->updateMap(&cellsWithVehs);
}

void Simulation::transitionFunc() {
	std::vector<Cell*> newCellsWithVehs;
	simMap->updateMap(&newCellsWithVehs);
	std::vector<MoveData> vehsMovesData;
	for (Cell* vehCell : cellsWithVehs) {
		vehsMovesData.push_back(evalVehMove(vehCell));
		evalRandomEvent(vehCell , &vehsMovesData.back());
	}
	std::vector<Cell*> tempNewCellsWithVehs = moveVehs(cellsWithVehs, vehsMovesData);
	newCellsWithVehs.insert(newCellsWithVehs.end(), tempNewCellsWithVehs.begin(), tempNewCellsWithVehs.end());
	cellsWithVehs = newCellsWithVehs;
}

MoveData Simulation::evalVehMove(Cell* vehCell) {
	std::vector<MoveData> moves;
	bool sawObstacle = false;
	int curVehSpeed = vehCell->getVehicle()->getSpeed();
	SpeedData speedDataFront = evalNewVehSpeed(vehCell, curVehSpeed, &sawObstacle);
	if (sawObstacle == true) {
		moves.push_back(MoveData{ speedDataFront, 0 });
	}
	evalChangeLane(vehCell, &moves);
	if (sawObstacle == false) {
		moves.push_back(MoveData{ speedDataFront, 0 });
	}
	std::stable_sort(moves.begin(), moves.end(), cmpMoveData);
	return moves.back();
}

SpeedData Simulation::evalNewVehSpeed(Cell* startCell, int curVehSpeed, bool* sawObstacle) {
	Vehicle* startCellVeh = startCell->getVehicle();
	if (startCellVeh != nullptr) {
		if (startCellVeh->getIsObstacle() == true) {
			return SpeedData{ 0, startCell };
		}
	}
	int newVehSpeed = std::min(curVehSpeed + 1, startCell->getMaxSpeed());
	Cell* tempCell = startCell;
	int tempCellMaxSpeed = NULL;
	TrafficLights* vehCellLights = startCell->getTrafficLight();
	if (vehCellLights != nullptr && vehCellLights->getColor() != LightColor::green) 	{
		return SpeedData{ 0, startCell };
	}
	for (int i = 1; i <= newVehSpeed; i++) {
		tempCell = tempCell->getNextCell();
		if (tempCell == nullptr) {
			break;
		}
		if (sawObstacle != nullptr && *sawObstacle == false && tempCell->getObstacleAhead() == true) {
			*sawObstacle = true;
		}
		tempCellMaxSpeed = tempCell->getMaxSpeed();
		if (newVehSpeed > tempCellMaxSpeed) {
			newVehSpeed = std::min(newVehSpeed, std::max(tempCellMaxSpeed, i));
		}
		Vehicle* tempCellVeh = tempCell->getVehicle();
		if (tempCellVeh != nullptr) {
			newVehSpeed = i - 1;
			if (sawObstacle != nullptr && tempCellVeh->getIsObstacle() == true) {
				*sawObstacle = true;
			}
			tempCell = tempCell->getPreviousCell();
			break;
		}
		if (tempCell->getTrafficLight() != nullptr && tempCell->getTrafficLight()->getColor() != LightColor::green) {
			newVehSpeed = i;
			break;
		}
	}
	return SpeedData{ newVehSpeed, tempCell };
}

void Simulation::evalChangeLane(Cell* vehCell, std::vector<MoveData>* moves) {
	int curVehSpeed = vehCell->getVehicle()->getSpeed();
	Cell* tempCell;
	int newLane;
	bool sawObstacle = false;
	int laneRandomizer = std::rand() % 2;
	for (int random_const = 1; random_const <= 2; random_const++) {
		if ((random_const + laneRandomizer) % 2 == 0) {
			tempCell = vehCell->getLeftCell();
			newLane = -1;
		}
		else {
			tempCell = vehCell->getRightCell();
			newLane = 1;
		}
		if (tempCell != nullptr && tempCell->getVehicle() == nullptr) {
			SpeedData newVehSpeed = evalNewVehSpeed(tempCell, curVehSpeed, &sawObstacle);
			if (sawObstacle == true) {
				continue;
			}
			int prevVehDist = -6;
			for (int i = -1; i >= -6; i--) {
				tempCell = tempCell->getPreviousCell();
				if (tempCell == nullptr) {
					break;
				}
				Vehicle* tempVeh = tempCell->getVehicle();
				if (tempVeh != nullptr) {
					prevVehDist = i + evalNewVehSpeed(tempCell, tempVeh->getSpeed(), nullptr).newVehSpeed;
					break;
				}
			}
			if (newVehSpeed.newVehSpeed - prevVehDist - 1 > minSafeSpace) {
				moves->push_back(MoveData{ newVehSpeed, newLane });
			}
		}
	}
}

void Simulation::evalRandomEvent(Cell* vehCell, MoveData* vehMoveData) {
	int newVehSpeed = vehMoveData->speedData.newVehSpeed;
	if (1.0 * std::rand() / RAND_MAX <= randEventProb && newVehSpeed != 0 &&vehCell->getVehicle()->getSpeed() <= newVehSpeed && vehMoveData->newVehLane == 0 && vehMoveData->speedData.destinationCell != nullptr) {
		vehMoveData->speedData.newVehSpeed -= 1;
		vehMoveData->speedData.destinationCell = vehMoveData->speedData.destinationCell->getPreviousCell();
	}
}

std::vector<Cell*> Simulation::moveVehs(std::vector<Cell*> cellsWithVehs, std::vector<MoveData> vehMovesData) {
	std::vector<Cell*> newCellsWithVehs;
	bool competition = false;
	for (unsigned int i = 0; i < cellsWithVehs.size(); i++) {
		Vehicle* curVeh = cellsWithVehs[i]->getVehicle();
		int newVehSpeed = vehMovesData[i].speedData.newVehSpeed;
		Cell* destinationCell = vehMovesData[i].speedData.destinationCell;
		if (newVehSpeed == 0) {
			newCellsWithVehs.push_back(cellsWithVehs[i]);
			curVeh->setSpeed(0);
			continue;
		}
		if (destinationCell == nullptr) {
			delete curVeh;
			cellsWithVehs[i]->setVehicle(nullptr);
			continue;
		}
		if (destinationCell->getVehicle() == nullptr) {
			curVeh->setSpeed(newVehSpeed);
			cellsWithVehs[i]->setVehicle(nullptr);
			destinationCell->setVehicle(curVeh);
			newCellsWithVehs.push_back(destinationCell);
		}
		else {
			SpeedData speedData = evalNewVehSpeed(cellsWithVehs[i], curVeh->getSpeed(), nullptr);
			curVeh->setSpeed(speedData.newVehSpeed);
			cellsWithVehs[i]->setVehicle(nullptr);
			speedData.destinationCell->setVehicle(curVeh);
			newCellsWithVehs.push_back(speedData.destinationCell);
			if (competition == false) {
				competition = true;
			}
		}
	}
	if (competition == true && shuffleIfCompetition == true) {
		std::shuffle(newCellsWithVehs.begin(), newCellsWithVehs.end(), randomEngine);
	}
	return newCellsWithVehs;
}

std::string Simulation::toString() {
	return simMap->toString();
}



bool cmpMoveData(MoveData lhs, MoveData rhs) {
	return lhs.speedData.newVehSpeed < rhs.speedData.newVehSpeed;
}