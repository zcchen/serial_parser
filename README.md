Serail Parser
=====================================

This is a project to decode the parse the common serial commands in C / Python / ...


Protocol
-------------------------------------
| name          |   byte    |   comments                                |
|---------------|-----------|-------------------------------------------|
| header        |   0       | 0x01 - 0xFF                               |
| length        |   1       | The length from next byte to the end      |
| payload       |   N       | user defined.                             |
| crc           |   2       | the crc for above bits, including header  |

NOTE:
1. All data are in little endian.
2. CRC, 0x8005

What Problems Will Be Solved
------------------------------------
1.  decode the payload from the above protocol.

2.  encode the payload from the above protocol.

3.  handle the message correctly in following cases:
    +   real header is missing.
        -   drop the data till a new header (including fake header) is found.
    +   identify the header is real or faking.
        -   if real, continue as normal.
        -   if faked, go back to this header, and search for the next header.
    +   if message is uncompleted, return current state.
        -   let the user codes to append the mixing ones.
    +   split messages from raw data.
        -   serial raw messages usually contains some message snippets,
            thus they need to be split correctly.

Usage
------------------------------------
TBD

How to Contribute
------------------------------------
TBD

License
------------------------------------
MIT License

