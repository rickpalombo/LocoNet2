
#include <LocoNetOverTCPStream.h>

const char digitMap[] = "0123456789abcdef";

void LocoNetOverTCPStream::start() {
    begin(nullptr);
}

void LocoNetOverTCPStream::begin(Stream * serialPort) {
    _busy = false;

    DEBUG("Waiting for WiFi...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);

    while (WiFi.status() != WL_CONNECTED) {
        DEBUG(".");
        delay(1000);
    }

    DEBUG("");
    DEBUG("WiFi connected");
    DEBUG("IP address: ");
    DEBUG(WiFi.localIP().toString().c_str());
    delay(1000);

    DEBUG("Connecting to IP: '");
    DEBUG(_ipAddress);
    DEBUG("' and port ");
    DEBUG(String(_port).c_str());
    DEBUG(" ...");
    
    _client.connect(_ipAddress, _port);
    while (!_client.connected()) {
        DEBUG(".");
        delay(1000);
    }
    DEBUG("");
    DEBUG("TCP Connection Established");

    LnMsg SendPacket;
    SendPacket.data[0] = OPC_RQ_SL_DATA;
    SendPacket.data[1] = 121;
    SendPacket.data[2] = 1;
    writeChecksum(SendPacket);

    // Links the Throttle with the command station
    //const char * tcpMessage = "SEND BB 79 01 3C";
    String tcpMessage = toTcpMessage(SendPacket.data, 4);
    DEBUG(tcpMessage.c_str());

    _client.println(tcpMessage);
    _client.flush();

    delay(500);
    String line = emptyString;
    while ((line = _client.readStringUntil('\n')).length() > 0) {
        DEBUG(line.c_str());

        if (line.startsWith("VERSION ")) {
            _lnOverTcpVersion = line.substring(8);
        }
    }
}

void LocoNetOverTCPStream::end() {
    _client.stop();
    WiFi.disconnect();
}

void LocoNetOverTCPStream::process() {
    // Read the message from the TCP connection
    if (!isBusy()) {
        if (_client.connected()) {
            int delimCount = 0;
            uint8_t loconetByte = 0;
            uint8_t offset = 0;
            boolean highNibble = true;
            lnMsg ReceivedPacket;
            int packetLen = 0;

            while (_client.available() > 0) {
                char RxByte = _client.read();
                if (RxByte == ' ' || RxByte == '\n' || RxByte == '\r') {
                    if (delimCount > 0) {
                        ReceivedPacket.data[delimCount - 1] = loconetByte;
                        packetLen++;
                    }
                    loconetByte = 0;
                    offset = 0;
                    highNibble = true;
                    delimCount++;
                } else {
                    if (isDigit(RxByte)) {
                        offset = 0x30;
                    } else {
                        offset = 0x37;
                    }

                    if (highNibble) {
                        loconetByte = (RxByte - offset) << 4;
                        highNibble = false;
                    } else {
                        loconetByte |= (RxByte - offset);
                        highNibble = true;
                    }
                }
            }

            if (packetLen > 0) {
                bus->broadcast(ReceivedPacket, this);
            }
        } else {
            DEBUG("Reconnecting...");
            _client.connect(_ipAddress, _port);
            DEBUG("Reconnected");
        }
    } else {
        DEBUG("Stream is busy");
    }
}

LN_STATUS LocoNetOverTCPStream::sendLocoNetPacketTry(uint8_t *packetData, uint8_t packetLen, unsigned char ucPrioDelay) {
    if (!isBusy()) {
        if (_client.connected()) {
            _busy = true;

            String tcpMessage = toTcpMessage(packetData, packetLen);

            _client.println(tcpMessage);
            _client.flush();

            // Sinchronous ECHO and SENT messages receive
            String line = emptyString;
            int delay = 0;
            for (int i = 0; i < 2; i++) {
                while ((line = _client.readStringUntil('\n')).length() == 0) {
                    delay++;

                    if (delay > 1000) {
                        break;
                    }
                }
        
                DEBUG(line.c_str());
                line = emptyString;
            }

            _busy = false;
            return LN_IDLE;
        }
        return LN_UNKNOWN_ERROR;
    }
    return LN_NETWORK_BUSY;
}

String LocoNetOverTCPStream::toTcpMessage(uint8_t *packetData, uint8_t packetLen) {
    String strMsg = "SEND ";

    for (int i = 0; i < packetLen; i++) {
        strMsg += (digitMap[(packetData[i] >> 4) & 0xF]);
        strMsg += (digitMap[(packetData[i] & 0xF)]);
        if (i < packetLen - 1) {
            strMsg += ' ';
        }
    }

    strMsg.toUpperCase();
    return strMsg;
}

String LocoNetOverTCPStream::getVersion() {
    return _lnOverTcpVersion;
}

bool LocoNetOverTCPStream::isBusy(void) {
    return _busy;
}
void LocoNetOverTCPStream::sendBreak(void) {}
void LocoNetOverTCPStream::beforeSend(void) {}
void LocoNetOverTCPStream::afterSend(void) {}