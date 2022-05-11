#pragma once

#include "Map.h"

Map::Map(std::string name) : name(name) {
	if (name == "") {
		throw std::invalid_argument("Map must have a name");
	}
}

Map::~Map() {
	for (Road* road : roads) {
		delete road;
	}
	for (Generator* generator : generators) {
		delete generator;
	}
	for (Crossing* crossing : crossings) {
		delete crossing;
	}
	for (LaneEndsMerge* laneEndsMerge : laneEndsMerges) {
		delete laneEndsMerge;
	}
}

std::string Map::getName() {
	return name;
}

std::vector<Road*> Map::getRoads() {
	return roads;
}

std::vector<Generator*> Map::getGenerators() {
	return generators;
}

std::vector<Crossing*> Map::getCrossings() {
	return crossings;
}

std::vector<LaneEndsMerge*> Map::getLaneEndsMerges() {
	return laneEndsMerges;
}

int Map::getMapPassableCellsCnt() {
	int passableCellsCnt = 0;
	for (Road* road : roads) {
		passableCellsCnt += road->getPassableCellsCnt();
	}
	for (Generator* generator : generators) {
		Vehicle* cellVeh = generator->getVehicle();
		if (cellVeh != nullptr && cellVeh->getIsObstacle() == true) {
			continue;
		}
		passableCellsCnt++;
	}
	for (LaneEndsMerge* laneEndsMerge : laneEndsMerges) {
		passableCellsCnt += laneEndsMerge->getPassableCellsCnt();
	}
	return passableCellsCnt;
}

std::vector<Cell*> Map::getCellsWithVehs() {
	std::vector<Cell*> cellsWithVehs;
	for (Generator* generator : generators) {
		if (generator->getVehicle() != nullptr) {
			cellsWithVehs.push_back(generator);
		}
	}
	for (Road* road : roads) {
		std::vector<Cell*> roadCellsWithVehs = road->getCellsWithVehs();
		cellsWithVehs.insert(cellsWithVehs.end(), roadCellsWithVehs.begin(), roadCellsWithVehs.end());
	}
	for (LaneEndsMerge* laneEndsMerge : laneEndsMerges) {
		std::vector<Cell*> laneEndsMergeCellsWithVehs = laneEndsMerge->getCellsWithVehs();
		cellsWithVehs.insert(cellsWithVehs.end(), laneEndsMergeCellsWithVehs.begin(), laneEndsMergeCellsWithVehs.end());
	}
	return cellsWithVehs;
}

void Map::addRoad(Road* road) {
	roads.push_back(road);
}

void Map::addGenerator(Generator* generator) {
	generators.push_back(generator);
}

void Map::addCrossing(Crossing* crossing) {
	crossings.push_back(crossing);
}

void Map::addLaneEndsMerge(LaneEndsMerge* laneEndsMerge) {
	laneEndsMerges.push_back(laneEndsMerge);
}

void Map::fillWithVehs(double fillingDegree) {
	int vehsToGenerateCnt = std::min((int)(std::round(fillingDegree * generators.size())), (int)generators.size());
	int generatedVehsCnt = 0;
	for (Generator* generator : generators) {
		if (generator->getVehicle() == nullptr) {
			if (1.0 * std::rand() / RAND_MAX <= fillingDegree) {
				generator->setVehicle(new Vehicle(0));
				generatedVehsCnt++;
				if (generatedVehsCnt >= vehsToGenerateCnt) {
					return;
				}
			}
		}
	}
	for (Road* road : roads) {
		road->fillWithVehs(fillingDegree);
	}
	for (LaneEndsMerge* laneEndsMerge : laneEndsMerges) {
		laneEndsMerge->fillWithVehs(fillingDegree);
	}
}

void Map::updateMap(std::vector<Cell*>* cellsWithVehs) {
	for (Generator* generator : generators) {
		if (generator->createVeh() == true) {
			cellsWithVehs->push_back(generator);
		}
	}
	for (Road* road : roads) {
		std::vector<TrafficLights*> roadLights = road->getTrafficLights();
		for (TrafficLights* roadLight : roadLights) {
			roadLight->updateTrafficLights();
		}
	}
	for (Crossing* crossing : crossings) {
		crossing->updateCrossing();
		std::vector<TrafficLights*> crossingLights = crossing->getTrafficLights();
		for (TrafficLights* crossingLight : crossingLights) {
			crossingLight->updateTrafficLights();
		}
	}
}

std::string Map::toString() {
	std::string repStr = "";
	for (Generator* generator : generators) {
		repStr += generator->toString();
	}
	for (Road* road : roads) {
		repStr += road->toString();
	}
	for (Crossing* crossing : crossings) {
		repStr += crossing->toString();
	}
	for (LaneEndsMerge* laneEndsMerge : laneEndsMerges) {
		repStr += laneEndsMerge->toString();
	}
	return repStr += "\n";
}



void linkCells(Cell* previousCell, Cell* nextCell) {
	previousCell->setNextCell(nextCell);
	nextCell->setPreviousCell(previousCell);
}
