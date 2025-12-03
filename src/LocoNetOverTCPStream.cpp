
#include <LocoNetOverTCPStream.h>

const char digitMap[] = "0123456789abcdef";

void LocoNetOverTCPStream::begin(Stream *serialPort) {}


void LocoNetOverTCPStream::start() {
    begin(nullptr);
}

void LocoNetOverTCPStream::end() {
    _client->stop();
}

void LocoNetOverTCPStream::process() {
    if(_client->available())
    {
        DEBUG("process: Process LocoNet Bytes");
        while(_client->available())
        {
            uint8_t inByte = _client->read();
            DEBUG("process: Byte: %02x", inByte);
            consume(inByte);
        }
    }
}

LN_STATUS LocoNetOverTCPStream::sendLocoNetPacketTry(uint8_t *packetData, uint8_t packetLen, unsigned char ucPrioDelay) {
    DEBUG("sendLocoNetPacketTry: begin state: %s", getStatusStr(LN_IDLE));

    // Do a process() just in case another byte has arrived after we last checked
    process();

    if( !isBusy() )
    {
        uint32_t tempRxThreshold;
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
    return false;
}

void LocoNetOverTCPStream::sendBreak() {}
void LocoNetOverTCPStream::beforeSend() {}
void LocoNetOverTCPStream::afterSend() {}
