# d1s-melis
Allwinner melis RTOS for D1s

## Toolchain

Before compiling, please download the compilation toolchain and place it to `prebuilt` directory.

Github: [Tina-Linux/d1s-melis/releases](https://github.com/Tina-Linux/d1s-melis/releases)

## Command
```
== before all ==
- lunch:        lunch <product_name>-<build_variant>

== build project ==
- Cleaning targets:
-   clean	  - Remove most generated files but keep the config and
-                     enough build support to build external modules
-   mrproper	  - Remove all generated files + config + various backup files
-   distclean	  - mrproper + remove editor backup and patch files
-
- Configuration targets:
-   make menuconfig to to do the customilize configuration.
-
- Other generic targets:
-   all	  - Build all targets marked with [*]
- * melis	  - Build the bare kernel
- * modules	  - Build all modules
-   gtags        - Generate GNU GLOBAL index
-
- Static analysers:
-   checkstack      - Generate a list of stack hogs
-   namespacecheck  - Name space analysis on compiled kernel
-   versioncheck    - Sanity check on version.h usage
-   includecheck    - Check for duplicate included header files
-   export_report   - List the usages of all exported symbols
-   headers_check   - Sanity check on exported headers
-   headerdep       - Detect inclusion cycles in headers
-   coccicheck      - Check with Coccinelle
-
- Kernel selftest:
-                     running kselftest on it
-   kselftest-clean - Remove all generated kselftest files
-   kselftest-merge - Merge all the config dependencies of kselftest to existing

- jump directory:
- croot:    Jump to the top of the tree.
- cboot:    Jump to uboot.
- cboot0:   Jump to boot0.
- cdts:     Jump to device tree.
- cbin:     Jump to uboot/boot0 bin directory.
- ckernel:  Jump to kernel.
- cdevice:  Jump to target.
- ccommon:  Jump to platform common.
- cconfigs: Jump to configs of target.
- cout:     Jump to out directory of target.
- ctarget:  Jump to target of compile directory.
- crootfs:  Jump to rootfs of compile directory.
- ctoolchain: Jump to toolchain directory.
- callwinnerpk: Jump to package allwinner directory.
- ctinatest:  Jump to tinateset directory.
- godir:    Go to the directory containing a file.

== grep file ==
- cgrep:    Greps on all local C/C++ files.
```

## How to build

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
pack          # pack spi nor flash
pack -a nand  # pack spi nand flash \ sd card
```

## How to download
Using the `PhoenixSuit` tool to burns the `melis_d1s-evb-board_uart0_8Mnor.img` firmware in the `out` directory.

