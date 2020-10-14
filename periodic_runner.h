#pragma once

/**
    \file Код ниже выставлен в качестве примера функционала. Не стоит ему
   относиться как к продакшн-реди коду. Код состоит из множества допущений,
   которым никогда не следует попадать в продакшн.

    Код просто показывает пример моего оформления и мысли.
 */

#include <stdint.h>
#include <algorithm>  // std::max
#include <array>
#include <chrono>

#include "frequency_runnable_interface.h"
#include "lcm.h"
#include "periodic_runner_interface.h"

// Сделаем исключение в виде существования класса, который умеет работать с
// потоком и заставить его заснуть на определенное время.
class Thread_timer {
public:
    std::chrono::milliseconds hz_to_us(uint32_t hz) {}

    void sleep(std::chrono::milliseconds) {}
};

/**
    \class По определенной частоте (заданной в классе пользователя)
   выполняет алгоритм действий, заданный пользователем (run)

    \warning Для корректной работы необходимо задавать частоту в
   Frequency_runnable_interface::run_freq_hz в целых числах или float ==
   0.1, 0.001 ... или 0.5, 0.05. В других случаях все отработает без ошибок,
   но отправка будет происходить с погрешностью по частоте.


    \tparam runners_count - Количество "запускаемых" объектов
 */
template <size_t runners_count>
class Periodic_runner final : public Periodic_runner_interface {
public:
    /**
       \brief Конструктор объекта Periodic_runner

       \tparam Runners Классы, реализующие Frequency_runnable_interface
       \param runners Ссылки на объекты реализующие Frequency_runnable_interface
     */
    template <class... Runners>
    Periodic_runner(Runners&... runners);

    /**
        \brief Деструктор объекта Periodic_runner
     */
    ~Periodic_runner() override = default;

private:
    /// Массив указателей на Frequency_runnable_interface
    std::array<Frequency_runnable_interface*, runners_count> runners_{};

    /// Частота обновления основного потока. В конструкторе объекта она
    /// изменится на максимальную частоту из объектов
    /// Frequency_runnable_interface::get_run_freq_hz()
    float update_freq_hz_ = 1.0f;

    /// Массив для хранения тиков, по которым будет происходить вызов run для
    /// каждого Frequency_runnable_interface объекта
    std::array<uint32_t, runners_count> update_tics{};

    /// Счетчик тиков основного потока
    uint32_t tic_counter_ = 0;

    /// Максимальное значение тика, по достижению которого tic_counter_
    /// обнулится
    uint32_t max_tic_value_ = 0;

    /**
        \brief Метод, который будет бесконечно вызываться в функции потока.
               tic_counter_ увеличивается каждую итерацию while(1) в потоке

     */
    void thread_process() override;

    /**
        \brief Метод, в котором происходит проверка тика и соответствующий по
       тику вызов run у Frequency_runnable_interface объектов
     */
    void observe_and_run();
};

// Реализация дедуцирования шаблона через конструктор
template <class... Runners>
Periodic_runner(Runners&...) -> Periodic_runner<sizeof...(Runners)>;

template <size_t runners_count>
template <class... Runners>
Periodic_runner<runners_count>::Periodic_runner(Runners&... runners)
        : runners_{&runners...} {
    /// Если передан 1 runner, то принимаем частоту нашего потока в соответствии
    /// с частотой  в runner
    if (runners_count == 1) {
        update_freq_hz_ = runners_[0]->get_run_freq_hz();
    } else {
        /// Если передано множество runner, то принимаем частоту нашего потока в
        /// соответствии с максимальной частотой их всех runner
        for (uint32_t i = 0; i < runners_count; i++)
            update_freq_hz_ =
                std::max(update_freq_hz_, runners_[i]->get_run_freq_hz());

        static_assert(runners_count == sizeof...(runners),
                      "Размер, передаваемый в шаблон, должен соответствовать "
                      "числу переданных аргументов шаблона");
    }

    /// Высчитываем тики относительно которых будем вызывать run у каждого
    /// runnera.
    for (uint32_t i = 0; i < runners_count; i++) {
        float run_freq_hz = runners_[i]->get_run_freq_hz();
        if (run_freq_hz <= 0) {
            // LOG_MESSAGE("Periodic_runner : Частота для "
            //             "Frequency_runnable_interface::run() у "
            //             "одного из объектов == 0");
        }
        update_tics[i] = static_cast<uint32_t>(update_freq_hz_ / run_freq_hz);
    }

    /// Для корректного обнуления tic_counter_, ищем НОК от всех тиков. Если
    /// надеяться на uint32_t переполнение, то выполнение run по тику не
    /// кратному 2 будет со временем уплывать. При обнулении по НОК, уплывания
    /// не будет
    max_tic_value_ = pt_math::lcm(update_tics);
}

template <size_t runners_count>
void Periodic_runner<runners_count>::thread_process() {
    /// Контролируем частоту потока
    Thread_timer timer;
    const auto&  ms = timer.hz_to_ms(update_freq_hz_);

    while (1) {
        timer.sleep(ms);
        observe_and_run();
    }
}

template <size_t runners_count>
void Periodic_runner<runners_count>::observe_and_run() {
    // Сначала увеличиваем счетчик, чтобы не учитывать значение == 0 после
    // обнуления - если учитывать 0, то мы будет вызывать run на 1 тик позже
    // желаемого
    tic_counter_++;
    for (uint32_t i = 0; i < runners_count; i++) {
        // Проверяем кратен ли текущий тик, для тика вызова run у конкретного
        // runner
        if ((tic_counter_ % update_tics[i]) == 0) {
            runners_[i]->run();
        }
    }
    /// Обнуление счетчика по cycle counter
    tic_counter_ = tic_counter_ % (max_tic_value_);
}
