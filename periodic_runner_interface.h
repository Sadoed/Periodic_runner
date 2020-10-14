#pragma once

/**
    \brief Интерфейс Periodic_runner_interface.

    Разрабатывался, чтобы спратять функцию потока за реализиацию.
    Позволяет шаблонному классу не указывать "<N>" при объявлении std::thread

 */
class Periodic_runner_interface {
public:
    /**
        \brief Конструктор объекта Periodic_runner_interface

     */
    Periodic_runner_interface() = default;

    /**
        \brief Деструктор объекта Periodic_runner_interface

     */
    virtual ~Periodic_runner_interface() = default;

    /**
        \brief Поток, для вызова метода thread_process();

        \param periodic_runner_ptr - Указатель на класс
                                    Periodic_runner_interface
     */
    static void _thread_observe_and_run(void* periodic_runner_ptr) {
        Periodic_runner_interface* periodic_runner =
            static_cast<Periodic_runner_interface*>(periodic_runner_ptr);

        periodic_runner->thread_process();
    }

private:
    /**
        \brief Метод, который будет вызываться в функции потока

     */
    virtual void thread_process() = 0;
}; 
