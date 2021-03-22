# cbuf

Simple C Buffer for Lua

# Install

```bash
luarocks install cbuf
```

# Reference

## new

```Lua
new(size)
```

Receives a size, returns a c buffer.

##### Parameters

- size - size of the allocating buffer

##### Returns

- a lightuserdata refers to the c buffer, or nil if failed.

## delete

```Lua
delete(buffer)
```

Receives a buffer and delete it.

##### Parameters

- buffer - cbuf object

##### Returns

- none

## size

```Lua
size(buffer)
```

Returns the size of the buffer.

##### Parameters

- buffer - cbuf object

##### Returns

- the size of the buffer

## slice

```Lua
slice(buffer, index)
```

Returns a slice of the buffer.

##### Parameters

- buffer - cbuf object
- index - index to start with

##### Returns

- the slice of the buffer in range [index, buffersize)

##### Notes

- Be careful with the slice. A slice is just a c pointer without range checking.

## shift

```Lua
shift(buffer, index, length, offset)
```

Shift the segment inside the buffer.

##### Parameters

- buffer - cbuf object
- index - index to start with
- length - length of the segment to shift with
- offset - offset to shift, negative for left shift and positive for right shift

##### Returns

- none

## zero

```Lua
zero(buffer, index, length)
```

Fill zeros into the buffer.

##### Parameters

- buffer - cbuf object
- index - index to start with, default is 0
- length - length of the segment to fill into, default is buffersize - index

##### Returns

- none

## copy

```Lua
copy(destbuf, destidx, srcbuf, srcidx, length)
```

Copy the segment from srcbuf to destbuf.

##### Parameters

- destbuf - cbuf object that writing into
- destidx - index to start writing info
- srcbuf - cbbuf object that reading from
- srcidx - index to start reading from
- length - length of the segment to copy with

##### Returns

- none

## copystring

```Lua
copystring(destbuf, destidx, srcstr, srcidx, length)
```

Copy the segment from srcstr to destbuf.

##### Parameters

- destbuf - cbuf object that writing into
- destidx - index to start writing info
- srcstr - string that reading from
- srcidx - index to start reading from
- length - length of the segment to copy with

##### Returns

- none

## tostring

```Lua
tostring(buffer, index)
```

Returns a null-terminated string from the buffer.

##### Parameters

- buffer - cbuf object
- index - index to start with, default is 0

##### Returns

- a string from the buffer, start from index, terminate while reaching '\0'

## tolstring

```Lua
tolstring(buffer, index, length)
```

Returns a fixed length string from the buffer.

- buffer - cbuf object
- index - index to start with
- length - length of the string

##### Returns

- a string from the buffer, in range [index, index + length)
