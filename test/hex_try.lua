local hex = require('hex');
local src = [[ !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~]];
local enc, dec;

enc = ifNil( hex.encode( src ) );
dec = ifNil( hex.decode( enc ) );
ifNotEqual( dec, src );
