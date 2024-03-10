# Mango Pi baremetal blink test with xfel

Referenced from [https://github.com/dwelch67/lichee-rv-samples](https://github.com/dwelch67/lichee-rv-samples).

## compile

- make

## install

- sudo /opt/rv/xfel/xfel write 0x45000 main.bin
- sudo /opt/rv/xfel/xfel exec 0x45000
