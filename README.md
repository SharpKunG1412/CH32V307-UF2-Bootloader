# CH32V307 UF2 Bootloader

A UF2-compatible bootloader for the **CH32V307** RISC-V microcontroller.

This project enables firmware updates using the UF2 format, making flashing simple and user-friendly.

---

## Features

- UF2 firmware update support
- Simple and reliable bootloader entry methods
- Designed for CH32V307 internal flash
- Lightweight and fast execution

---

## Memory Layout

| Component    | Address        |
|--------------|----------------|
| Bootloader   | `0x08000000`   |
| Application  | `0x08005000`   |

> Make sure your application firmware is linked to start **after the bootloader region**

---

## Entering Bootloader Mode

You can jump from your application to the bootloader using one of the following methods:

### 1. GPIO Trigger
Pull **GPIO C7** LOW during reset or runtime.

### 2. Magic Value Trigger
Write a magic value to a reserved flash address and soft reset mcu.
```
Address: 0x08078000-4
Value: 0x0000FFFF (MAGIC_IAP)
```

---

## Build Instructions

### Prerequisites

- RISC-V GCC Toolchain
- CMake

### Setup

Edit `CMakeLists.txt` and set your toolchain path:

```CMakeLists.txt
set(TOOLPATH "path/to/riscv-gcc")
```

## Build (Windows)

```
build.bat
```


## UF2 Conversion

Convert your compiled .bin file into UF2 format using the provided Python script:

```
python .\UF2\uf2conv.py .\demo.bin -o demo.uf2 -f 0x0A424B53 -b 0x08005000
```

Example file `Example.uf2` have been include in UF2 folder   