#ifndef BRAIN_H
#define BRAIN_H

#include "robot.h"

enum Estado { IDLE, MEASURING };

class Brain {
private:
    Estado estadoActual;
    int angulosActuales[7];
    Robot& robot;

public:
    Brain(Robot& r);
    void procesarEntrada();
    void actualizar();
};

#endif