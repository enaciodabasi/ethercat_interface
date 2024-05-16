/**
 * @file slave.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef SLAVE_HPP_
#define SLAVE_HPP_

#include <expected>
#include <unordered_map>
#include <functional>

#include "ethercat_interface/common_defs.hpp"

namespace ethercat_interface
{
  namespace slave
  {
    namespace
    {

      /*
      /
      / Function wrappers around the EC_WRITE(D, X) macros
      /
      /
      */

      void writeInt8(uint8_t* data_addr, double value)
      {
        EC_WRITE_S8(data_addr, static_cast<int8_t>(value));
      }
      void writeUInt8(uint8_t* data_addr, double value)
      {
        EC_WRITE_U8(data_addr, static_cast<uint8_t>(value));
      }

      void writeInt16(uint8_t* data_addr, double value)
      {
        EC_WRITE_S16(data_addr, static_cast<int16_t>(value));
      }
      void writeUInt16(uint8_t* data_addr, double value)
      {
        EC_WRITE_U16(data_addr, static_cast<uint16_t>(value));
      }

      void writeInt32(uint8_t* data_addr, double value)
      {
        EC_WRITE_S32(data_addr, static_cast<int32_t>(value));
      }
      void writeUInt32(uint8_t* data_addr, double value)
      {
        EC_WRITE_U32(data_addr, static_cast<uint32_t>(value));
      }
      
      void writeInt64(uint8_t* data_addr, double value)
      {
        EC_WRITE_S64(data_addr, static_cast<int64_t>(value));
      }
      void writeUInt64(uint8_t* data_addr, double value)
      {
        EC_WRITE_U64(data_addr, static_cast<uint64_t>(value));
      }

      /*
      /
      / Function wrappers around the EC_READ(D, X) macros
      /
      /
      */

      double readInt8(uint8_t* data_addr)
      {
        return static_cast<double>(EC_READ_S8(data_addr));
      }

      double readUInt8(uint8_t* data_addr)
      {
        return static_cast<double>(EC_READ_U8(data_addr));
      }

      double readInt16(uint8_t* data_addr)
      {
        return static_cast<double>(EC_READ_S16(data_addr));
      }

      double readUInt16(uint8_t* data_addr)
      {
        return static_cast<double>(EC_READ_U16(data_addr));
      }

      double readInt32(uint8_t* data_addr)
      {
        return static_cast<double>(EC_READ_S32(data_addr));
      }

      double readUInt32(uint8_t* data_addr)
      {
        return static_cast<double>(EC_READ_U32(data_addr));
      }

      double readInt64(uint8_t* data_addr)
      {
        return static_cast<double>(EC_READ_S64(data_addr));
      }

      double readUInt64(uint8_t* data_addr)
      {
        return static_cast<double>(EC_READ_U64(data_addr));
      }

    }
    
    struct Entry_RW_Functions
    {
      std::function<double(uint8_t*)> readFunction;
      std::function<void(uint8_t* /*data_addr*/, double/*value*/)> writeFunction;
    };

    class Slave
    {
      public:

      Slave();

      Slave(const Slave& other);

      Slave operator=(const Slave& other);

      Slave(Slave&& other);

      Slave& operator=(Slave&& other);

      virtual ~Slave();

      virtual ReturnType init(
        const SlaveInfo& info,
        ec_master_t* master_ptr,
        ec_domain_t* domain_ptr
      );

      const SlaveInfo getSlaveInformation() const
      {
        return m_SlaveInformation;
      }

      uint* getOffsetOfEntry(const std::string& entry_name)
      {
        if(auto offsetFound = m_Offsets.find(entry_name); offsetFound != m_Offsets.end())
        { 
          // return address of found entry offset;
          return &offsetFound->second;
        }

        return nullptr;
      }

      void setDomainDataPointer(uint8_t* domain_data_ptr)
      {
        m_DomainDataPtr = domain_data_ptr;
      }

      protected:

      SlaveInfo m_SlaveInformation;

      ec_slave_config_t* m_SlaveConfigPtr;

      uint8_t* m_DomainDataPtr;

      std::unordered_map<std::string, uint> m_Offsets;

      std::unordered_map<std::string, Entry_RW_Functions> m_EntryRWFunctionMap;

      virtual ReturnType configureSlave(
        ec_master_t* master_ptr,
        ec_domain_t* domain_ptr
      );

      virtual std::optional<ec_pdo_info_t*> configureProcessDataObjects();

      virtual std::optional<ec_sync_info_t*> configureSyncManager(ec_pdo_info_t* pdos);

      bool m_DcEnabled;

      private:


    };
  } // End of namespace slave
  
} // End of namespace ethercat_interface

#endif // SLAVE_HPP_
