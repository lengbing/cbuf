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

- a lightuserdata refers to the c buffer, or nil if failed.

## delete

```Lua
delete(buffer)
```

Delete the buffer.

##### Parameters

- buffer - cbuf object

##### Returns

- none

## size

```Lua
size(buffer)
```

Gets the size of the buffer.

##### Parameters

- buffer - cbuf object

##### Returns

- the size of the buffer

## uvcount

```Lua
uvcount(buffer)
```

Gets the user value count associated with the buffer

##### Parameters

- buffer - cbuf object

##### Returns

- the user value count associated with the buffer

## getuv

```Lua
getuv(buffer, index)
```

Gets the buffer's user value at index

##### Parameters

- buffer - cbuf object
- index - index of user value

##### Returns

- the user value at index

## setuv

```Lua
setuv(buffer, index, value)
```

Sets the buffer's user value at index to value

##### Parameters

- buffer - cbuf object
- index - index of user value
- value - value to set, only integer could be set

##### Returns

- none

## slice

```Lua
slice(buffer, offset)
```

Gets a slice of the buffer.

##### Parameters

- buffer - cbuf object
- offset - offset to start with

##### Returns

- the slice of the buffer in range [offset, buffersize)

##### Notes

- Be careful with the slice. A slice is just a c pointer without range checking.

## shift

```Lua
shift(buffer, offset, length, move)
```

Shift the segment inside the buffer.

##### Parameters

- buffer - cbuf object
- offset - offset to start with
- length - length of the segment to shift with
- move - move to shift, negative for left shift and positive for right shift

##### Returns

- none

## zero

```Lua
zero(buffer, offset, length)
```

Fill zeros into the buffer.

##### Parameters

- buffer - cbuf object
- offset - offset to start with, default is 0
- length - length of the segment to fill into, default is buffersize - offset

##### Returns

- none

## copy

```Lua
copy(destbuf, destoffset, srcbuf, srcoffset, length)
```

Copy the segment from srcbuf to destbuf.

##### Parameters

- destbuf - cbuf object that writing into
- destoffset - index to start writing info
- srcbuf - cbbuf object that reading from
- srcoffset - index to start reading from
- length - length of the segment to copy with

##### Returns

- none

## copystring

```Lua
copystring(destbuf, destoffset, srcstr, srcidx, length)
```

Copy the segment from srcstr to destbuf.

##### Parameters

- destbuf - cbuf object that writing into
- destidx - index to start writing info
- srcstr - string that reading from
- srcidx - index to start reading from, string index start from 1
- length - length of the segment to copy with

##### Returns

- none

## tostring

```Lua
tostring(buffer, offset)
```

Gets a null-terminated string from the buffer.

##### Parameters

- buffer - cbuf object
- offset - offset to start with, default is 0

##### Returns

- a string from the buffer, start from offset, terminate while reaching '\0'

## tolstring

```Lua
tolstring(buffer, offset, length)
```

Gets a fixed length string from the buffer.

- buffer - cbuf object
- offset - index to start with
- length - length of the string

##### Returns

- a string from the buffer, in range [offset, offset + length)
