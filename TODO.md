# TODO

- tcp socket for mac and metadata
- udp socket for audio data

## Server

### client handling

- socket select()
- try single threaded first
- launch client_handler in thread at boot?
    - put client in queue to handler when connected

### timecode

- timecode generator/transmitter
    - different network socket or interleave
    - to either channel
    - other i2s interface
        - builtin?
            - 8-bit should be fine for LTC
    - update timecode library
        - better formatting
        - read LTC input

## Client

- receive timecode over network
    - LTC output

