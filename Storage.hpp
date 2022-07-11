#ifndef STORAGE_H
#define STORAGE_H

#include "Arduino.h"
#include <EEPROM.h>

const size_t EEPROOM_LENGTH = 1024 * 1024;
int eeprom_index = 0;
bool storage_initialized = false;

int eeprom_reserve(size_t size)
{
    auto index = eeprom_index;
    eeprom_index += size;
    return index;
}

void eeprom_store(int address, const uint8_t *const data, size_t length)
{
    for (size_t i = 0; i < length; i++)
        EEPROM.write(address + i, data[i]);
}

void eeprom_load(uint8_t *const data, int address, size_t length)
{
    for (size_t i = 0; i < length; i++)
        data[i] = EEPROM.read(address + i);
}

template <typename T>
int reserve()
{
    return eeprom_reserve(sizeof(T));
}

template <typename T>
void store(int address, const T &value)
{
    eeprom_store(address, (uint8_t *)&value, sizeof(T));
}

template <typename T>
void load(T &value, int address)
{
    eeprom_load((uint8_t *)&value, address, sizeof(T));
}

template <typename T>
class EEPROMVar
{

private:
    T m_cache;
    char m_id = 'V';
    int m_address = 0;

public:
    void set(const T &value)
    {
        store(m_address, m_id);
        store(m_address + 1, value);
        EEPROM.commit();
    }
    bool has() const
    {
        char id;
        load(id, m_address);
        return id == m_id;
    }
    const T &get() const
    {
        load(m_cache, m_address + 1);
        return m_cache;
    }
    const T &get(const T &defvalue) const
    {
        if (!has())
            return defvalue;
        return get();
    }
    void erase()
    {
        store(m_address, '\0');
        EEPROM.commit();
    }
    EEPROMVar()
    {
        m_address = eeprom_reserve(sizeof(T) + 1);
    }
};

template <typename T>
class EEPROMArray;

template <typename T, int L>
class EEPROMArray<T[L]>
{

private:
    T m_cache[L];
    char m_id = 'A';
    int m_address = 0;

public:
    void set(const T *const value, size_t count)
    {
        if (count > L)
            count = L;
        store(m_address, m_id);
        eeprom_store(m_address + 1, (uint8_t *)value, count);
        EEPROM.commit();
    }
    bool has() const
    {
        char id;
        load(id, m_address);
        return id == m_id;
    }
    const T *const get() const
    {
        eeprom_load((uint8_t *)m_cache, m_address + 1, L);
        return m_cache;
    }
    const T *const get(const T *const defvalue) const
    {
        if (!has())
            return defvalue;
        return get();
    }
    void erase()
    {
        store(m_address, '\0');
        EEPROM.commit();
    }
    EEPROMArray()
    {
        m_address = eeprom_reserve(sizeof(T) * L + 1);
    }
};

void initStorage()
{
    if (!storage_initialized)
    {
        storage_initialized = true;
        Serial.println("Initializing EEPRROM...");
        EEPROM.begin(EEPROOM_LENGTH);
        Serial.print("EEPROM length: ");
        Serial.print(EEPROM.length());
        Serial.println(" bytes");
    }
}

#endif