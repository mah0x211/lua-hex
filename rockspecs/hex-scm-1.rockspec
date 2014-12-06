package = "hex"
version = "scm-1"
source = {
    url = "git://github.com/mah0x211/lua-hex.git"
}
description = {
    summary = "hexadecimal encode/decode module",
    homepage = "https://github.com/mah0x211/lua-hex", 
    license = "MIT/X11",
    maintainer = "Masatoshi Teruya"
}
dependencies = {
    "lua >= 5.1"
}
build = {
    type = "builtin",
    modules = {
        hex = {
            sources = { 
                "src/hex.c",
            }
        }
    }
}

