#pragma once

#include <LocoNetStream.h>
#include <WiFi.h>
#include <WiFiClient.h>

class LocoNetOverTCPStream : public LocoNetStream {
    public:
        LocoNetOverTCPStream(LocoNetBus *bus,
        char * ssid,
        char * password,
        char * ipAddress,
        uint16_t port) : LocoNetStream(bus) {
            _ssid = ssid;
            _password = password;
            _ipAddress = ipAddress;
            _port = port;
            _client = WiFiClient();
        };
		void begin(Stream * serialPort);
		void end();
		void process();

        void start();

        String getVersion();
    protected:
		LN_STATUS sendLocoNetPacketTry(uint8_t *packetData, uint8_t packetLen, unsigned char ucPrioDelay);

		virtual bool isBusy(void);
		virtual void sendBreak(void);
		virtual void beforeSend(void);
		virtual void afterSend(void);
    private:
        WiFiClient _client;
        char * _ssid;
        char * _password;
        char * _ipAddress;
        uint16_t _port;
        bool _busy;
        String _lnOverTcpVersion;
        int _keepAlive = 1000;
        int _keepIdle = 5;

        String toTcpMessage(uint8_t *packetData, uint8_t packetLen);
};