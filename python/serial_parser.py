# -*- coding: utf-8 -*-
import sys
import struct
import crcmod

class SerialParserError(Exception): pass
class SerialParserErrorHeaderUnmatch(SerialParserError): pass
class SerialParserErrorCRC(SerialParserError): pass
class SerialParserErrorHeaderFaked(SerialParserError): pass
class SerialParserErrorNotCompleted(SerialParserError): pass

class SerialParser:

    def __init__(self, header=b'S'):
        if type(header) is not bytes:
            raise TypeError("header should be byte type, but not <{}>".format(type(header)))
        self.__header = header
        self.__crc_func = crcmod.predefined.mkPredefinedCrcFun("crc-16")

    @property
    def HEADER(self):
        return self._bytes(self.__header)

    def _crc_func(self, data):
        return self.__crc_func(data)

    def _bytes(self, the_str):
        if type(the_str) is bytes:
            return the_str
        if type(the_str) is int:
            the_str = chr(the_str)
        if sys.version_info.major < 3:
            return bytes(the_str)
        elif sys.version_info.major == 3:
            return bytes(the_str, 'latin')
        else:
            raise Exception("Python Version Error")

    def _byte2int(self, byte):
        if sys.version_info.major < 3:
            if type(byte) is not bytes:
                raise TypeError("byte <{the_byte}> is expected as <bytes>, but not {t}.".format(\
                                the_byte=byte, t=type(byte)))
            return ord(byte)
        elif sys.version_info.major == 3:
            return byte
        else:
            raise Exception("Python Version Error")


    def encode(self, payload):
        msg = b""
        msg += self.HEADER
        msg += struct.pack("B", len(payload) + 2)
        msg += self._bytes(payload)
        msg += struct.pack("<H", self._crc_func(msg))
        return msg

    def decode(self, msg, max_msglen=0):
        # return payload
        ret = self.__msg_check(msg, max_msglen=max_msglen)
        if ret >= 0:
            return msg[2:-2]

    def find(self, raw, max_msglen=0):
        # return (msg_this, msg_rest)
        if type(raw) is not bytes:
            raise TypeError("msg <{msg}> should be <bytes>, but not <{t}>".format(
                            msg=raw, t=type(msg)))
        first_header_index = 0
        found_msglen = 0
        while first_header_index < len(raw):
            try:
                found_msglen = self.__msg_check(raw[first_header_index:], max_msglen=max_msglen)
                if found_msglen:
                    first_msg = raw[first_header_index: first_header_index + found_msglen]
                else:
                    first_msg = b""
                second_msg = raw[first_header_index + found_msglen:]
                if not second_msg:
                    second_msg = b""
                return (first_msg, second_msg)
            except SerialParserErrorHeaderUnmatch: pass
            except SerialParserErrorCRC: pass
            except SerialParserErrorHeaderFaked: pass
            except SerialParserErrorNotCompleted:
                return (raw[first_header_index:], b"")
            except Exception as e:
                raise e
            first_header_index += 1
        return (b"", b"")

    def __msg_check(self, msg, max_msglen=0):
        # return the length of the correct msg from the first byte
        if type(msg) is not bytes:
            raise TypeError("msg <{msg}> should be <bytes>, but not <{t}>".format(
                            msg=msg, t=type(msg)))
        if self._bytes(msg[0]) != self.HEADER:
            raise SerialParserErrorHeaderUnmatch(\
                    "msg[0] is <{header_msg}>, but not <{header_def}>".format(
                        header_msg=self._bytes(msg[0]), header_def=self.HEADER))
        msglen = self._byte2int(msg[1])    # should be >= 2
        if max_msglen:
            if msglen + 2 > max_msglen:
                raise SerialParserErrorHeaderFaked
        else:
            if msglen + 2 > len(msg):
                raise SerialParserErrorNotCompleted
        msg_crc_raw = struct.unpack("<H", msg[0 + 1 + msglen - 1: 0 + 1 + msglen + 1])[0]
        msg_crc_recalc = self._crc_func(msg[0: msglen])
        if msg_crc_raw != msg_crc_recalc:
            raise SerialParserErrorCRC("raw crc is {crc_raw}, calc crc is {crc_calc}".format(\
                    crc_raw=hex(msg_crc_raw), crc_calc=hex(msg_crc_recalc)))
        return msglen + 2

