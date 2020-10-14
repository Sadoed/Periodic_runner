#pragma once

/**
    \brief Интерфейс, который должен гарантировать выполнение произвольного
           алгоритма действий в формате метода void run() 

 */
class Runnable_interface {
public:
    /**
        \brief Конструктор объекта Runnable_interface

     */
    Runnable_interface() = default;

    virtual ~Runnable_interface() = default;

    /**
        \brief Метод, в котором подразумевается какой-либо алгоритм действий

     */
    virtual void run() = 0;
};
