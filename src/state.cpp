#include "state.hpp"
#include <algorithm>
namespace ethercat_interface
{
    namespace slave
    {
        
        StatusHelper::UnimportantBitIndexes StatusHelper::NotReadyToSwitchOn = std::vector<int>{4, 7};
        StatusHelper::UnimportantBitIndexes StatusHelper::SwitchOnDisabled = std::vector<int>{4, 5, 7};
        StatusHelper::UnimportantBitIndexes StatusHelper::ReadyToSwitchOn = std::vector<int>{4, 7};
        StatusHelper::UnimportantBitIndexes StatusHelper::SwitchedOn = std::vector<int>{4, 7};
        StatusHelper::UnimportantBitIndexes StatusHelper::OperationEnabled = std::vector<int>{4, 7};
        StatusHelper::UnimportantBitIndexes StatusHelper::QuickStopActive = std::vector<int>{4, 5, 7};
        StatusHelper::UnimportantBitIndexes StatusHelper::FaultResponseActive = std::vector<int>{4, 5, 7};
        StatusHelper::UnimportantBitIndexes StatusHelper::Fault = std::vector<int>{4, 5, 7};

        bool isStatusCorrect(SlaveStatus current_status, StatusType target_status)
        {
            std::string currentStatusStr = std::bitset<8>((uint8_t)(current_status & 0x00FF)).to_string();
            std::string targetStatusStr = std::bitset<8>((uint8_t)(getStatusValue(target_status) & 0x00FF)).to_string();

            if(currentStatusStr.size() != targetStatusStr.size())
            {   
                // LOG
                return false;
            }

            std::reverse(currentStatusStr.begin(), currentStatusStr.end());
            std::reverse(currentStatusStr.begin(), currentStatusStr.end());

            auto unimportantBitIndexes = StatusHelper::getNotImportantBits(target_status);

            for(int i = 0; i < currentStatusStr.size(); i++)
            {
                if(std::find(unimportantBitIndexes.begin(), unimportantBitIndexes.end(), i) == unimportantBitIndexes.end())
                {

                    if(currentStatusStr[i] != targetStatusStr[i])
                    {
                        return false;
                    }
                }
            }

            return true;
        }
    }
}