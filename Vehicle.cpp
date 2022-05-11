#pragma once

#include "Vehicle.h"

Vehicle::Vehicle(std::string name, int speed) : name(name), speed(speed), isObstacle(false) {
	create();
}

Vehicle::Vehicle(int speed) : speed(speed), isObstacle(false) {
	create();
	name = std::to_string(ID);
}

void Vehicle::create() {
	if (speed < 0 || speed > 6)
		throw std::invalid_argument("Speed must be in range between 0 and 6");
	ID = IDcnt++;
}

std::string Vehicle::getName() {
	return name;
}

int Vehicle::getID() {
	return ID;
}

int Vehicle::getSpeed() {
	return speed;
}

void Vehicle::setSpeed(int newSpeed) {
	this->speed = newSpeed;
}

bool Vehicle::getIsObstacle() {
	return isObstacle;
}
