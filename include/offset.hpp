#ifndef OFFSET_HPP
#define OFFSET_HPP

#include <vector>
#include <unordered_map>
#include <optional>

namespace ethercat_interface
{

    class DataOffset
    {
        typedef std::pair<std::string, uint> Data;

        public:

        DataOffset();

        DataOffset(const std::vector<std::string>& pdo_names);
        
        ~DataOffset();

        std::optional<unsigned int*> getDataOffset(const std::string& offset_name);
        
        private:

        std::unordered_map<std::string, uint*> m_DataOffsetAdressMap;

        std::vector<Data> m_DataOffsets;

    };

        /**
         * @brief Base class to derive from when creating offsets for the Slave PDOs.
         * 
         */
    class Offset
    {
        public:
        Offset(void){};
        virtual ~Offset(){};
        
        /**
         * @brief Set the Data object
         * 
         * @param data 
         */
        virtual void setData(void* data) = 0;
        /**
         * @brief Get the Data object
         * 
         * @param offset_name 
         * @return unsigned* 
         */
        virtual unsigned int* getData(const std::string& offset_name)
        {
            auto found = m_OffsetNameIndexMap.find(offset_name);
	        if(found == m_OffsetNameIndexMap.end())
	        {
	        	return nullptr;
	        }
        
	        return found->second;
        };

        /**
         * @brief When implementing the init_OffsetMap function, use the member function of the STL Vector "emplace_back"
         * @brief to insert offset names to the vector. Then use this container to fetch names from the map during the creation of ec_pdo_entry_reg_t.
         * 
         */
        std::vector<std::string> m_OffsetNameIndexes;

        inline std::size_t getNumberOfOffsets() const
        {
            return m_NumOffsets;
        }

        protected:
        /**
         * @brief 
         * 
         */
        std::unordered_map<std::string, uint*> m_OffsetNameIndexMap;
        
        /**
         * @brief 
         * 
         */
        virtual void init_OffsetMap() = 0;

        std::size_t m_NumOffsets;

        private:

        
    };

}

#endif // OFFSET_HPP