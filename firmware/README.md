####固件开发环境
* Arduino 1.6
* github.com/Wiznet/WIZ_Ethernet_Library
* github.com/Seeed-Studio/PN532

Library安装方法(Arduino对库的支持实在太hack了)：

1. 参照WIZ_Ethernet_Library-master中的说明，替换Arduino自带的Ethernet库，以支持W5500
2. Select device部分选择`W5500_ETHERNET_SHIELD`
3. 注释掉 `#define WIZ550io_WITH_MACADDRESS`
4. 参照PN532-master中的说明将`PN532*`文件夹复制到Arduino的库目录中