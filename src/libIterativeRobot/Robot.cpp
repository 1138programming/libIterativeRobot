Robot* Robot::instance = 0;

Robot::Robot() {
}

void Robot::robotInit() {
}

void Robot::autonInit() {
}

void Robot::autonPeriodic() {
}

void Robot::teleopInit() {
}

void Robot::teleopPeriodic() {
}

void Robot::disabledInit() {
}

void Robot::disabledPeriodic() {
}

Robot* Robot::getInstance() {
    if (instance == NULL) {
        instance = new Robot();
    }
    return instance;
}
