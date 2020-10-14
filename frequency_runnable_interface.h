#pragma once

#include "runnable_interface.h"

/**
    \brief Интерфейс, который должен гарантировать выполнение произвольного
           алгоритма действий в формате метода void run() с определенной
           частотой, заданной пользователем

 */
class Frequency_runnable_interface : public Runnable_interface {
public:
    /**
        \brief Конструктор объекта Frequency_runnable_interface

     */
    Frequency_runnable_interface() = default;

    virtual ~Frequency_runnable_interface() = default;

    /**
        \brief Метод должен возвращать частоту, с которой необходимо выполнять
       метод run()

        \return float - частота
     */
    virtual float get_run_freq_hz() const = 0;
};
