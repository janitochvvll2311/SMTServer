/**
 * @file Storage.hpp
 * @author Juan Jesus Chavez Villa (janitochvvll2311@gmail.com)
 * @brief Inline module for Persistent Storage for variable settings
 * @version 0.1.0
 * @date 2022-08-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef STORAGE_H
#define STORAGE_H

#include "Arduino.h" // Main Arduino library
#include <EEPROM.h>  // EEPROM library

const size_t EEPROOM_LENGTH = 1024 * 1024; // Requested EEPROM size
int eeprom_index = 0;                      // EEPROM storage index
bool storage_initialized = false;          // Init module flag

/**
 * @brief Reserver EEPROM bytes
 */
int eeprom_reserve(size_t size)
{
    auto index = eeprom_index;
    eeprom_index += size;
    return index;
}

/**
 * @brief Write EEPROM bytes
 */
void eeprom_store(int address, const uint8_t *const data, size_t length)
{
    for (size_t i = 0; i < length; i++)
        EEPROM.write(address + i, data[i]);
}

/**
 * @brief Read EEPROM bytes
 */
void eeprom_load(uint8_t *const data, int address, size_t length)
{
    for (size_t i = 0; i < length; i++)
        data[i] = EEPROM.read(address + i);
}

/**
 * @brief Reserve sizeof(T) bytes
 */
template <typename T>
int reserve()
{
    return eeprom_reserve(sizeof(T));
}

/**
 * @brief Write a T type on EEPRROM
 */
template <typename T>
void store(int address, const T &value)
{
    eeprom_store(address, (uint8_t *)&value, sizeof(T));
}

/**
 * @brief Read a T type from EEPROM
 */
template <typename T>
void load(T &value, int address)
{
    eeprom_load((uint8_t *)&value, address, sizeof(T));
}

/**
 * @brief Manage a T type persistent var
 */
template <typename T>
class EEPROMVar
{

private:
    T m_cache;         // Last readed var value
    char m_id = 'V';   // EEPROM header. Status flag.
    int m_address = 0; // EEPROM location address

public:
    /**
     * @brief Sets the value and saves it on EEPROM
     */
    void set(const T &value)
    {
        store(m_address, m_id);
        store(m_address + 1, value);
        EEPROM.commit();
    }
    /**
     * @brief Checks there are stored a valid value on EEPROM
     */
    bool has() const
    {
        char id;
        load(id, m_address);
        return id == m_id;
    }
    /**
     * @brief Get the stored value from EEPROM
     */
    const T &get() const
    {
        load(m_cache, m_address + 1);
        return m_cache;
    }
    /**
     * @brief Get the stored value from EEPROM if is valid.
     * Return a default value otherwise.
     */
    const T &get(const T &defvalue) const
    {
        if (!has())
            return defvalue;
        return get();
    }
    /**
     * @brief Invalidate the stored value on EEPROM
     */
    void erase()
    {
        store(m_address, '\0');
        EEPROM.commit();
    }
    /**
     * @brief Construct a new EEPROMVar object and reserve EEPROM space
     */
    EEPROMVar()
    {
        m_address = eeprom_reserve(sizeof(T) + 1);
    }
};

template <typename T>
class EEPROMArray;

/**
 * @brief Manage a T type array persistent var
 */
template <typename T, int L>
class EEPROMArray<T[L]>
{

private:
    T m_cache[L];      // Last readed array values
    char m_id = 'A';   // EEPROM header. Status flag.
    int m_address = 0; // EEPROM location address

public:
    /**
     * @brief Sets the values and saves it on EEPROM
     */
    void set(const T *const value, size_t count)
    {
        if (count > L)
            count = L;
        store(m_address, m_id);
        eeprom_store(m_address + 1, (uint8_t *)value, count);
        EEPROM.commit();
    }
    /**
     * @brief Checks there are stored a valid value on EEPROM
     */
    bool has() const
    {
        char id;
        load(id, m_address);
        return id == m_id;
    }
    /**
     * @brief Get the stored values from EEPROM
     */
    const T *const get() const
    {
        eeprom_load((uint8_t *)m_cache, m_address + 1, L);
        return m_cache;
    }
    /**
     * @brief Get the stored values from EEPROM if is valid.
     * Return a default values otherwise.
     */
    const T *const get(const T *const defvalue) const
    {
        if (!has())
            return defvalue;
        return get();
    }
    /**
     * @brief Invalidate the stored values on EEPROM
     */
    void erase()
    {
        store(m_address, '\0');
        EEPROM.commit();
    }
    /**
     * @brief Construct a new EEPROMArray object and reserve EEPROM space
     */
    EEPROMArray()
    {
        m_address = eeprom_reserve(sizeof(T) * L + 1);
    }
};

/**
 * @brief Initialize the Storage module
 */
void initStorage()
{
    if (!storage_initialized)
    {
        storage_initialized = true;
        Serial.println("Initializing EEPRROM...");
        EEPROM.begin(EEPROOM_LENGTH); // Request EEPROM storage
        Serial.print("EEPROM length: ");
        Serial.print(EEPROM.length()); // Show available EEPROM storage
        Serial.println(" bytes");
    }
}

#endif