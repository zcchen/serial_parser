# -*- coding: utf-8 -*-
import pytest
from ..serial_parser import SerialParser

@pytest.fixture(scope="module")
def serial_parser():
    return SerialParser("S")

def test_encoder(serial_parser):
    assert b'S\x05123\x80\x7a' == serial_parser.encode("123")
