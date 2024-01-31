# LoRa Audio Transmission System

## Overview

The LoRa Audio Transmission System is designed for wireless audio transmission using Heltec devices and LoRa modules. This README provides instructions for setting up the receiving and sending sides of the system.

## Receiving Side Setup

### 1. Heltec Configuration

- Attach the Heltec Wireless Stick V3 to a Windows machine.
- Load the code into Heltec using the code located in the `receivingSide/heltecCode/heltecCode.ino` directory.

### 2. Python Script

- Run the Python script located in the `sendingSide/pythonScript/main.py` directory.
- The Python script will receive data from the Heltec device and create an audio WAV file.

## Sending Side Setup

### 1. Heltec Wireless Stick V3 Connection

- Connect the Heltec Wireless Stick V3 with a PDM microphone according to the following connections:

  | MEM ES PDM MIC | Heltec Wireless Stick (V3) |
  |----------------|-----------------------------|
  | CLK            | GPl0 (34)                   |
  | DAT            | GPl0 (33)                   |
  | SEL            | Not Connected               |
  | VCC            | +3.3V                       |
  | GND            | GND                         |

### 2. PDM Microphone Connection

- Connect the PDM microphone using the instructions provided in the microphone's documentation.

### 3. Heltec Configuration

- Load the code into Heltec using the code located in the `receivingSide/heltecCode/heltecCode.ino` directory.
- Enter 'R' in the serial monitor of Heltec.

### 4. Recording Process

- Wait for recording; the process will last for 20 seconds and then send the audio data over the LoRa module to the receiving side.

## Hardware Links

- [Heltec Wireless Stick V3](https://heltec.org/project/wireless-stick-v3/)
- [PDM Microphone](https://www.aliexpress.com/item/1005002110339161.html?spm=a2g0o.productlist.main.11.27426d97mgGNL1&algo_pvid=16326bb1-ca12-441e-81d3-f1efbe43a8f7&algo_exp_id=16326bb1-ca12-441e-81d3-f1efbe43a8f7-5&pdp_npi=4%40dis%21PKR%211758.96%211055.38%21%21%215.95%21%21%402101f4a016951429684842087ef983%2112000018771149036%21sea%21PK%213589218214%21S&curPageLogUid=4loBUASSln7x)

