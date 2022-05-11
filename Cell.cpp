#pragma once

#include "Cell.h"

Cell::Cell() : rightCell(nullptr), leftCell(nullptr), previousCell(nullptr), carHolder(nullptr), maxSpeed(0), isObstacleAhead(false), trafficLight(nullptr) {}

Cell::Cell(int maxSpeed) : rightCell(nullptr), leftCell(nullptr), previousCell(nullptr), carHolder(nullptr), maxSpeed(maxSpeed), isObstacleAhead(false), trafficLight(nullptr) {
    if(maxSpeed > 6 || maxSpeed < 1)
        throw std::invalid_argument("Max speed must be in range between 1 and 6");
}

Cell::~Cell() {
    delete carHolder;
}

int Cell::getMaxSpeed() {
    return maxSpeed;
}

void Cell::setMaxSpeed(int maxSpeed) {
    this->maxSpeed = maxSpeed;
}

void Cell::setCarHolder(CarHolder* carHolder) {
    this->carHolder = carHolder;
}

Vehicle* Cell::getVehicle() {
    return carHolder->getVehicle();
}

void Cell::setVehicle(Vehicle* vehicle) {
    this->carHolder->setVehicle(vehicle);
}

Cell* Cell::getRightCell() {
    return rightCell;
}

void Cell::setRightCell(Cell* rightCell) {
    this->rightCell = rightCell;
}

Cell* Cell::getLeftCell() {
    return leftCell;
}

void Cell::setLeftCell(Cell* leftCell) {
    this->leftCell = leftCell;
}

Cell* Cell::getPreviousCell() {
    return previousCell;
}

void Cell::setPreviousCell(Cell* previousCell) {
    this->previousCell = previousCell;
}

TrafficLights* Cell::getTrafficLight() {
    return trafficLight;
}

void Cell::setTrafficLight(TrafficLights* newLight) {
    trafficLight = newLight;
}

bool Cell::getObstacleAhead() {
    return isObstacleAhead;
}

void Cell::setObstacleAhead(bool isObstacleAhead) {
    this->isObstacleAhead = isObstacleAhead;
}



