# -*- coding: utf-8 -*-
import sys
import struct
import crcmod

class SerialParser:

    def __init__(self, header='S'):
        self.__header = header
        self.__crc_func = crcmod.predefined.mkPredefinedCrcFun("crc-16")

    @property
    def HEADER(self):
        return self.__header

    def _crc_func(self, data):
        return self.__crc_func(data)

    def _bytes(self, the_str):
        if sys.version_info.major < 3:
            return bytes(the_str)
        elif sys.version_info.major == 3:
            return bytes(the_str, 'ascii')
        else:
            raise Exception("Python Version Error")

    def encode(self, payload):
        msg = b""
        msg += self._bytes(self.HEADER)
        msg += struct.pack("B", len(payload) + 2)
        msg += self._bytes(payload)
        msg += struct.pack("<H", self._crc_func(msg))
        return msg

    def decode(self, msg):
        # return payload
        raise NotImplementedError

    def find(self, raw, max_msglen=0):
        # return payload
        raise NotImplementedError

    def __msg_check(self, msg, max_msglen=0):
        pass

