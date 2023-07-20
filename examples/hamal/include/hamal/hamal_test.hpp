/**
 * @file hamal_test.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef HAMAL_TEST_HPP_
#define HAMAL_TEST_HPP_

#include "ethercat_interface/controller.hpp"

#include <math.h>
#include <signal.h>

constexpr double angularToRPM_Constant = ((180 / M_PI) / 6) * 24.635; 

class HamalController : public ethercat_interface::controller::Controller
{
    public:

    HamalController(const std::string& config_file_path);

    ~HamalController();

    void startTask() override;
    
    private:

    void cyclicTask() override;

    

};

#endif // HAMAL_TEST_HPP_