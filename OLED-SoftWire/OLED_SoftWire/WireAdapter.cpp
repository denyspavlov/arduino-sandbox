#include "WireAdapter.h"
#include <SoftwareWire.h>

WireAdapter::WireAdapter(int sdaPin, int sclPin)
    : TwoWire()
{
    SoftwareWire tmpSoftwareWire(sdaPin, sclPin, false);
    softwareWire = tmpSoftwareWire;
}

void WireAdapter::begin()
{
    softwareWire.begin();
}

void WireAdapter::begin(uint8_t address)
{
    softwareWire.begin(address);
}

void WireAdapter::begin(int address)
{
    softwareWire.begin(address);
}

void WireAdapter::end()
{
    softwareWire.end();
}

void WireAdapter::setClock(uint32_t clockSpeed)
{
    softwareWire.setClock(clockSpeed);
}

void WireAdapter::setWireTimeout(uint32_t timeout, bool reset_with_timeout)
{
    softwareWire.setTimeout(timeout);
}

bool WireAdapter::getWireTimeoutFlag()
{
    return false;
}

void WireAdapter::clearWireTimeoutFlag()
{
    // nothing
}

void WireAdapter::beginTransmission(uint8_t address)
{
    softwareWire.beginTransmission(address);
}

void WireAdapter::beginTransmission(int address)
{
    softwareWire.beginTransmission(address);
}

uint8_t WireAdapter::endTransmission()
{
    return softwareWire.endTransmission();
}

uint8_t WireAdapter::endTransmission(uint8_t releaseBus)
{
    return softwareWire.endTransmission(releaseBus);
}

uint8_t WireAdapter::requestFrom(uint8_t address, uint8_t size)
{
    return softwareWire.requestFrom(address, size);
}

uint8_t WireAdapter::requestFrom(uint8_t address, uint8_t size, uint8_t sendStop)
{
    return softwareWire.requestFrom(address, size, sendStop);
}

uint8_t WireAdapter::requestFrom(uint8_t address, uint8_t size, uint32_t waitTime,
                                uint8_t sendStop, uint8_t retryCount)
{
    return softwareWire.requestFrom(address, size, sendStop);
}

uint8_t WireAdapter::requestFrom(int address, int size)
{
    return softwareWire.requestFrom(address, size);
}

uint8_t WireAdapter::requestFrom(int address, int size, int sendStop)
{
    return softwareWire.requestFrom(address, size, sendStop);
}

size_t WireAdapter::write(uint8_t data)
{
    return softwareWire.write(data);
}

size_t WireAdapter::write(const uint8_t *buffer, size_t size)
{
    return softwareWire.write(buffer, size);
}

int WireAdapter::available()
{
    return softwareWire.available();
}

int WireAdapter::read()
{
    return softwareWire.read();
}

int WireAdapter::peek()
{
    return softwareWire.peek();
}

void WireAdapter::flush()
{
    // nothing
}

void WireAdapter::onReceive(void (*function)(int))
{
    // nothing
}

void WireAdapter::onRequest(void (*function)(void))
{
    // nothing
}

// inline size_t WireAdapter::write(unsigned long n)
// {
//     return write((uint8_t)n);
// }

// inline size_t WireAdapter::write(long n)
// {
//     return write((uint8_t)n);
// }

// inline size_t WireAdapter::write(unsigned int n)
// {
//     return write((uint8_t)n);
// }

// inline size_t WireAdapter::write(int n)
// {
//     return write((uint8_t)n);
// }