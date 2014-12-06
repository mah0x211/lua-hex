lua-hex
=========

hexadecimal encode/decode module

---

## Installation

```sh
luarocks install hex --from=http://mah0x211.github.io/rocks/
```

## API

### Encode

#### str, err = hex.encode( src:string )

```lua
local hex = require('hex');
local src = [[ !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~]];
local enc, err = hex.encode( src );
print( enc ); -- 202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e
```

### Decode

#### str, err = hex.decode( src:string )

```lua
local hex = require('hex');
local src = '202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e';
local dec, err = hex.decode( src );
print( dec ); --  !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
```

