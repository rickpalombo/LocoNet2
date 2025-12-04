#include <LocoNetOverTCPStream.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>

std::vector<std::string> tokenizeBySpace(const std::string& command) {
    std::vector<std::string> tokens;
    std::istringstream iss(command);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

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
        if (_overTcpProtocol) {
            if (_client->available()) {
                const char* command = _client->readStringUntil('\n').c_str();
                const std::vector<std::string> tokens = tokenizeBySpace(command);
                std::vector<int> byteArray;
                for (int i = 1; i < tokens.size(); i++) {
                    int value = std::stoi(tokens[i], nullptr, 16);
                    byteArray.push_back(value);
                }

                for (auto const byte : byteArray) {
                    consume(byte);
                }

                const char* sentOk = "SENT OK";
                _client->println(sentOk);
            }
        } else {
            while(_client->available())
            {
                uint8_t inByte = _client->read();
                DEBUG("LocoNetOverTCPStream: process: Byte: %02x", inByte);
                consume(inByte);
            }
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