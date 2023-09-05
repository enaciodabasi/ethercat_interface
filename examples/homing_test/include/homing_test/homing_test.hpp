/**
 * @file homing_test.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef HOMING_TEST_HPP_
#define HOMING_TEST_HPP_

#include "ethercat_interface/controller.hpp"

using namespace ethercat_interface;

class HomingTest : public controller::Controller
{

    public:

    HomingTest(const std::string& config_file_path);

    ~HomingTest()
    {
        joinThread();
    }

    void startTask() override;

    private:

    void cyclicTask() override;

};

#endif HOMING_TEST_HPP_