#pragma once

#include <LocoNetStream.h>
#include <WiFi.h>
#include <WiFiClient.h>

class LocoNetOverTCPStream : public LocoNetStream {
public:
    LocoNetOverTCPStream(LocoNetBus *bus, WiFiClient* client) : LocoNetStream(bus) {
        _client = client;
    };
	void begin(Stream * serialPort);
	void end();
	void process();
    void start();
	bool isConnected() const;
protected:
	LN_STATUS sendLocoNetPacketTry(uint8_t *packetData, uint8_t packetLen, unsigned char ucPrioDelay) override;

	bool isBusy() override;
	void sendBreak() override;
	void beforeSend() override;
	void afterSend() override;
private:
    WiFiClient* _client;
};