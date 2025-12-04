#pragma once

#include <LocoNetStream.h>
#include <WiFi.h>

class LocoNetOverTCPStream : public LocoNetStream {
public:
    LocoNetOverTCPStream(LocoNetBus *bus, WiFiServer* server, WiFiClient* client, const bool isCommandStation) : LocoNetStream(bus) {
    	_server = server;
    	_client = client;
    	_isCommandStation = isCommandStation;
    };
	void begin(Stream * serialPort);
	void end();
	void process();
    void start();
	bool isConnected() const;
protected:
	LN_STATUS sendLocoNetPacketTry(uint8_t *packetData, uint8_t packetLen, unsigned char ucPrioDelay) override;

	bool isBusy(void) override;
	void sendBreak(void) override;
	void beforeSend(void) override;
	void afterSend(void) override;
private:
    WiFiClient* _client;
	WiFiServer* _server;
	bool _isCommandStation;
};