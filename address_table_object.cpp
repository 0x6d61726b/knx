#include <cstring>

#include "address_table_object.h"
#include "bits.h"

using namespace std;


AddressTableObject::AddressTableObject(uint8_t* memoryReference): TableObject(memoryReference)
{

}

void AddressTableObject::readProperty(PropertyID id, uint32_t start, uint32_t count, uint8_t* data)
{
    switch (id)
    {
        case PID_OBJECT_TYPE:
            pushWord(OT_ADDR_TABLE, data);
            break;
        default:
            TableObject::readProperty(id, start, count, data);
    }
}

uint16_t AddressTableObject::entryCount()
{
    if (loadState() != LS_LOADED)
        return 0;

    return _groupAddresses[0];
}

uint16_t AddressTableObject::getGa(uint16_t tsap)
{
    if (loadState() != LS_LOADED || tsap > entryCount() )
        return 0;
    
    return _groupAddresses[tsap];
}

uint16_t AddressTableObject::getTsap(uint16_t addr)
{
    uint16_t size = entryCount();
    for (uint16_t i = 1; i <= size; i++)
        if (_groupAddresses[i] == addr)
            return i;
    return 1;
}

#pragma region SaveRestore

uint8_t* AddressTableObject::save(uint8_t* buffer)
{
    return TableObject::save(buffer);
}

uint8_t* AddressTableObject::restore(uint8_t* buffer)
{
    buffer = TableObject::restore(buffer);
    
    _groupAddresses = (uint16_t*)_data;
    
    return buffer;
}

#pragma endregion

bool AddressTableObject::contains(uint16_t addr)
{
    uint16_t size = entryCount();
    for (uint16_t i = 1; i <= size; i++)
        if (_groupAddresses[i] == addr)
            return true;

    return false;
}

void AddressTableObject::beforeStateChange(LoadState& newState)
{
    if (newState != LS_LOADED)
        return;

    _groupAddresses = (uint16_t*)_data;

    uint16_t count = reverseByteOrder(_groupAddresses[0]);
    // big endian -> little endian
    for (size_t i = 0; i <= count; i++)
        _groupAddresses[i] = reverseByteOrder(_groupAddresses[i]);
}
