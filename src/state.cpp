#include "state.hpp"

namespace ethercat_interface
{
    namespace slave
    {
        void stateMachine(const StatusType& target_status, std::function<void(void)> function)
        {
            switch (target_status)
            {
            case StatusType::OperationEnabled:
                function();
                break;
            
            default:
                break;
            }
        }
    }
}