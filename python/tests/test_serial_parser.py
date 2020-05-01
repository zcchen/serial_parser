# -*- coding: utf-8 -*-
import pytest
from ..serial_parser import SerialParser

@pytest.fixture(scope="module")
def serial_parser():
    return SerialParser("S")

payload_msg_testdata = [ # payload, msg
        (b"123", b'S\x05123\x80\x7a')
    ]

@pytest.mark.parametrize("payload, msg", payload_msg_testdata)
def test_encoder(serial_parser, payload, msg):
    assert msg == serial_parser.encode(payload)

@pytest.mark.parametrize("payload, msg", payload_msg_testdata)
def test_decoder(serial_parser, payload, msg):
    assert payload == serial_parser.decode(msg)

