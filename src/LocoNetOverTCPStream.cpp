#include <LocoNetOverTCPStream.h>

void LocoNetOverTCPStream::begin(Stream *serialPort) {}


void LocoNetOverTCPStream::start() {
    begin(nullptr);
}

void LocoNetOverTCPStream::end() {
    _client->stop();
    bus->removeConsumer(this);
}

void LocoNetOverTCPStream::process() {
    if(_client->available())
    {
        DEBUG("LocoNetOverTCPStream: process: Process LocoNet Bytes");
        while(_client->available())
        {
            uint8_t inByte = _client->read();
            DEBUG("LocoNetOverTCPStream: process: Byte: %02x", inByte);
            consume(inByte);
        }
    }
}

LN_STATUS LocoNetOverTCPStream::sendLocoNetPacketTry(uint8_t *packetData, uint8_t packetLen, unsigned char ucPrioDelay) {
    if( !isBusy() )
    {
        beforeSend();

        DEBUG("sendLocoNetPacketTry: Start to send data");
        while(packetLen--)
        {
            _client->write(packetData, 1);
            packetData++;
        }
        txStats.txPackets++;

        afterSend();

        return LN_IDLE;
    }

    return LN_NETWORK_BUSY;
}

bool LocoNetOverTCPStream::isBusy() {
    return !isConnected();
}

bool LocoNetOverTCPStream::isConnected() const {
    return _client != nullptr && _client->connected();
}

void LocoNetOverTCPStream::sendBreak() {}
void LocoNetOverTCPStream::beforeSend() {}
void LocoNetOverTCPStream::afterSend() {}
