# MiniNet Chapter 0: Bit Transmission Simulation

A low-level simulation of binary data transmission across a local pipeline using raw C bitwise operations.

## What This Project Does (A Quick Review for Future Me)
- **sender.c:** Extracts 8 raw bits using bit-shifts (>>) and masks (&), flashing them to stderr while using fwrite for stdout.
- **receiver.c:** Captures incoming raw bytes using fread via stdin and reconstructs the string.
- **The Core Trick:** Uses stderr for bit visualization so stdout stays clean for data payload transmission.

## How to Compile & Run
1. Compile sender: gcc chapter0/sender.c -o chapter0/sender.exe
2. Compile receiver: gcc chapter0/receiver.c -o chapter0/receiver.exe
3. Run pipeline: .\chapter0\sender.exe hello | .\chapter0\receiver.exe
