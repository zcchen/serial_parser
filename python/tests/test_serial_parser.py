# -*- coding: utf-8 -*-
import pytest
from ..serial_parser import SerialParser

@pytest.fixture(scope="module")
def serial_parser():
    return SerialParser(b"S")


payload_msg_testdata = [ # payload, msg
        (b"123", b'S\x05123\x80\x7a')
    ]

@pytest.mark.parametrize("payload, msg", payload_msg_testdata)
def test_encoder(serial_parser, payload, msg):
    assert msg == serial_parser.encode(payload)

@pytest.mark.parametrize("payload, msg", payload_msg_testdata)
def test_decoder(serial_parser, payload, msg):
    assert payload == serial_parser.decode(msg)


found_msg_testdata = [ # insert_msg, msg, append_msg, max_msglen=0
        (b"ab", b'S\x05123\x80\x7a', b"cd", 0),
    ]

found_msg_but_not_complete_testdata = [ # insert_msg, msg, append_msg, max_msglen=0
        (b"ab", b'S\x08123\x80\x7a', b"cd", 0),
    ]

found_msg_but_nothing_testdata = [ # insert_msg, msg, append_msg, max_msglen=0
        (b"ab", b'S\x03123\x80\x7a', b"cd", 0),
    ]

@pytest.mark.parametrize("insert_msg, msg, append_msg, max_msglen",
                         found_msg_testdata)
def test_find(serial_parser, insert_msg, msg, append_msg, max_msglen):
    found = serial_parser.find(insert_msg + msg + append_msg, max_msglen=max_msglen)
    assert msg == found[0]
    assert append_msg == found[1]

@pytest.mark.parametrize("insert_msg, msg, append_msg, max_msglen",
                         found_msg_but_not_complete_testdata)
def test_find_but_not_complete(serial_parser, insert_msg, msg, append_msg, max_msglen):
    found = serial_parser.find(insert_msg + msg + append_msg, max_msglen=max_msglen)
    assert msg + append_msg == found[0]
    assert b"" == found[1]

@pytest.mark.parametrize("insert_msg, msg, append_msg, max_msglen",
                         found_msg_but_nothing_testdata)
def test_find_but_nothing(serial_parser, insert_msg, msg, append_msg, max_msglen):
    found = serial_parser.find(insert_msg + msg + append_msg, max_msglen=max_msglen)
    assert b"" == found[0]
    assert b"" == found[1]

