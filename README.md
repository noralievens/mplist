# mplist

mpd playlist utility - add tracks to a playlist  
useful in scripts or keybindings  

## usage

`mplist [options] <playlist> <track> <track>  ...`

`<playlist>` is mandatory  
`<track>` is optional and will fallback to "current"

## options

`--host,    -h` mpd host address  
`--port,    -p` mpd port, usually 6600  
`--quiet,   -q` silence stdout  

host and port read from environment variables `$MPD_HOST` and `$MPD_PORT` if not provided

## examples
```
$ mplist -h music.local -p 6600 "mysillyplaylist" someband/tehalbum/stupidsong.flac
$ mplist "goodsongs" "$(mpc  -f %file%)"
$ mplist "verygoodsongs"
```

## dependencies

`libmpdclient`

## install

run `make clean install`

