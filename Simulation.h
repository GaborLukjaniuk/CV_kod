#pragma once

#include <random>
#include <cstdlib>

#include "Map.h"

struct SpeedData {
	int newVehSpeed;
	Cell* destinationCell;
};

struct MoveData {
	SpeedData speedData;
	int newVehLane;
};

bool cmpMoveData(MoveData lhs, MoveData rhs);



class Simulation {
protected:
	std::vector<Cell*> cellsWithVehs;
	double randEventProb;
	Map* simMap;
	std::default_random_engine randomEngine;
	int minSafeSpace;
	bool shuffleIfCompetition;

public:
	Simulation(Map* simMap, double randEventProb, int minSafeSpace, bool shuffleIfCompetition = true);
	Map* getSimulationMap();
	void initiateSimulation();
	void transitionFunc();
	std::string toString();

protected:
	MoveData evalVehMove(Cell* vehCell);
	SpeedData evalNewVehSpeed(Cell* startCell, int curVehSpeed, bool* sawObstacle);
	void evalChangeLane(Cell* vehCell, std::vector<MoveData>* moves);
	void evalRandomEvent(Cell* vehCell, MoveData* vehMoveData);
	std::vector<Cell*> moveVehs(std::vector<Cell*> cellsWithVehs, std::vector<MoveData> vehMovesData);
};

