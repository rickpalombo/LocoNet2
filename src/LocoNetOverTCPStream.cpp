#include <LocoNetOverTCPStream.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>

void LocoNetOverTCPStream::begin(Stream *serialPort) {}


void LocoNetOverTCPStream::start() {
    begin(nullptr);
    _started = true;
}

void LocoNetOverTCPStream::end() {
    _client->stop();
    bus->removeConsumer(this);
    _started = false;
}

void LocoNetOverTCPStream::process() {
    if(_client->available())
    {
        DEBUG("LocoNetOverTCPStream: process: Process LocoNet Bytes");
        while(_client->available()) {
            uint8_t inByte = _client->read();
            consume(inByte);
        }
    }
}

LN_STATUS LocoNetOverTCPStream::sendLocoNetPacketTry(uint8_t *packetData, uint8_t packetLen, unsigned char ucPrioDelay) {
    if( !isBusy() )
    {
        beforeSend();

        DEBUG("sendLocoNetPacketTry: Start to send data");
        _client->write(packetData, packetLen);
        _client->flush();
        txStats.txPackets++;

        afterSend();

        return LN_IDLE;
    }

    return LN_NETWORK_BUSY;
}

bool LocoNetOverTCPStream::isBusy() {
    return false;
}

void LocoNetOverTCPStream::sendBreak() {}
void LocoNetOverTCPStream::beforeSend() {}
void LocoNetOverTCPStream::afterSend() {}