# cbuf

Simple C Buffer for Lua

# Install

```bash
luarocks install cbuf
```

# Reference

## new

```Lua
new(size, uvcount)
```

Creates a c buffer.

##### Parameters

- size - size of the allocating buffer
- uvcount - count of user values associated with the buffer, default is 0

##### Returns

- a userdata refers to the c buffer, or nil if failed.

## delete

```Lua
buf:delete()
```

Delete the buffer.

##### Parameters

- none

##### Returns

- none


## uvcount

```Lua
buf:uvcount()
```

Gets the user value count associated with the buffer

##### Parameters

- none

##### Returns

- the user value count associated with the buffer

## getuv

```Lua
buf:getuv(index)
```

Gets the buffer's user value at index

##### Parameters

- index - index of user value

##### Returns

- the user value at index

## setuv

```Lua
buf:setuv(index, value)
```

Sets the buffer's user value at index to value

##### Parameters

- index - index of user value
- value - value to set

##### Returns

- none

## raw

```Lua
buf:raw()
```

Gets the raw buffer pointer, == buf[0]

##### Parameters

- none

##### Returns

- the raw buffer pointer

## shift

```Lua
buf:shift(index, length, move)
```

Shift the segment inside the buffer.

##### Parameters

- index - index to start with
- length - length of the segment to shift with
- move - move to shift, negative for left shift and positive for right shift

##### Returns

- none

## zero

```Lua
buf:zero(index, length)
```

Fill zeros into the buffer.

##### Parameters

- index - index to start with, default is 0
- length - length of the segment to fill into, default is #buf - index

##### Returns

- none

## copy

```Lua
buf:copy(index, src, length)
```

Copy the segment from src to buffer.

##### Parameters

- index - buffer index to start with
- src - pointer of a raw buffer
- length - length of the segment to copy with

##### Returns

- none

## copystring

```Lua
buf:copystring(index, src, length)
```

Copy the segment from string to buffer.

##### Parameters

- index - buffer index to start with
- src - string that reading from
- length - length of the segment to copy with, default is #src

##### Returns

- none

## substring

```Lua
buf:substring(index)
```

Gets a null-terminated string from the buffer.

##### Parameters

- index - index to start with, default is 0

##### Returns

- a string from the buffer, start from index, terminate while reaching '\0'

## sublstring

```Lua
buf:tolstring(index, length)
```

Gets a fixed length string from the buffer.

- index - index to start with, default is 0
- length - length of the string, default is #buf - index

##### Returns

- a string from the buffer, in range [index, index + length)

## index and newindex

```Lua
buf[index]
```

Gets the raw buffer at index

```Lua
buf[index] = 'string to set'
```

Sets the content start from index, same as `buf:copystring(index, src)`

## size

```Lua
#buf
```

Gets the buffer size
