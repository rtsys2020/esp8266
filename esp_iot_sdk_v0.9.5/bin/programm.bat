@echo off
echo %cd%
cd C:\AITHINKER-ESP8266-SDK\project\esp_iot_sdk_v0.9.5\bin
echo %cd%
esptool.py --port com4 write_flash 0x0 eagle.flash.bin 0x40000 eagle.irom0text.bin
pause