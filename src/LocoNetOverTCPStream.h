#pragma once

#include <LocoNetStream.h>
#include <WiFiClient.h>

class LocoNetOverTCPStream : public LocoNetStream {
public:
    LocoNetOverTCPStream(LocoNetBus *bus, WiFiClient* client, bool overTcpProtocol) : LocoNetStream(bus) {
        _client = client;
    	_overTcpProtocol = overTcpProtocol;
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
	bool _overTcpProtocol;
};