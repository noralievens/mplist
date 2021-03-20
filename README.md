# mplist

mpd playlist utility - add tracks to a playlist

## usage

`mplist [options] <playlist> <track> <track>  ...`

`<playlist>` is mandatory
`<track>` is optional and will fallback to current track

## options

`--host,    -h`
`--port,    -p`
`--quiet,   -q`

## examples
```
$ mplist -h music.local -p 6600 "mysillyplaylist" someband/tehalbum/stupidsong.flac
$ mplist "goodsongs" $(mpc queued -f %file%)
$ mplist "verygoodsongs"
```
