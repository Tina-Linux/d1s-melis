# d1s-melis
Allwinner melis RTOS for D1s

# Toolchain

Download and place it to `prebuilt`

Github: [YuzukiHD/d1s-melis/releases](https://github.com/YuzukiHD/d1s-melis/releases)

# How to build

Register the environment variables of the SDK with the compilation server host;
```shell
source melis-env.sh
```

Enter lunch, then enter 2, and select the d1s scheme "d1s-evb-board";
```shell
lunch
```

Build projcet
```shell
make -j65535
```

Pack the target image
```shell
pack
```

# How to download
Using the `PhoenixSuit` tool to burns the `melis_d1s-evb-board_uart0_8Mnor.img` firmware in the `out` directory.

