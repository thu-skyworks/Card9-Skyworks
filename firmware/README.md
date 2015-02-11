####固件开发环境
* Arduino 1.0.5
* WIZ_Ethernet_Library

Library安装方法(Arduino对库的支持实在太hack了)：

1. 参照WIZ_Ethernet_Library-master中的说明，替换Arduino自带的Ethernet库，以支持W5500
2. Select device部分选择`W5500_ETHERNET_SHIELD`
3. 注释掉 `#define WIZ550io_WITH_MACADDRESS`