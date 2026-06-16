local testcase = require('testcase')
local assert = require('assert')
local hex = require('hex')

function testcase.encode()
    -- test that encode string to hex string
    local src = 'Hello, World!'
    local enc = assert(hex.encode(src))
    assert.equal(enc, '48656c6c6f2c20576f726c6421')
end

function testcase.decode()
    -- test that decode hex string to original string
    local src = '48656c6c6f2c20576f726c6421'
    local dec = assert(hex.decode(src))
    assert.equal(dec, 'Hello, World!')

    -- test that decode hex string with odd length returns error
    local err
    dec, err = hex.decode('48656c6c6f2c20576f726c642')
    assert.is_nil(dec)
    assert.re_match(err, 'invalid argument', 'i')
end

function testcase.encode_and_decode()
    -- test that encode and decode string is same as original string
    local src =
        [[ !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~]]
    local enc = assert(hex.encode(src))
    local dec = assert(hex.decode(enc))
    assert.equal(dec, src)
end
