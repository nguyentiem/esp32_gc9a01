nvs,data,nvs,0x9000,24K,
phy_init,data,phy,0xf000,4K,
factory,app,factory,0x10000,1M,
storage,data,spiffs,0x110000,960K,

CONFIG_PARTITION_TABLE_CUSTOM=y
CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partitions_example.csv"
CONFIG_PARTITION_TABLE_FILENAME="partitions_example.csv"