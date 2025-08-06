# TISemu
## Overview
TISemu is a lightweight emulator for the [TIS-100](https://en.wikipedia.org/wiki/TIS-100) assembly-like programming environment

## Instruction

Instruction             Description<br>
`mov`            - copy a value from one location to another<br>
`sav`            - save the current value of acc to backup register bak<br>
`swp`            - swap the contents of acc and bak<br>
`inpt`           - read from input stream (similar to UP, DOWN, LEFT, RIGHT in TIS-100)<br>
`out`            - output to console (simulates output stream)<br>
`nil`            - represents zero or null (used for resetting acc)<br>
`nop`            - set acc to zero<br>
`add`            - add a value to acc<br>
`sub`            - subtract a value from acc<br>
`neg`            - negate the value in acc<br>
`jmp`            - unconditional jump to a label<br>
`jez`            - jump if acc is zero<br>
`jnz`            - jump if acc is not zero<br>
`jgz`            - jump if acc is greater than zero<br>
`jlz`            - jump if acc is less than zero<br>
`jro`            - relative jump by value<br>
`#`              - comments start with #<br>

## Registers & Streams
acc    - Accumulator: main working register<br>
bak    - Backup register: stores temporary values<br>
inpt   - Input stream<br>
out    - Output stream (prints to console)<br>
nil    - Represents a null or zero value<br>

## Syntax
- Comments begin with # and are ignored by the interpreter
- Labels are defined with a colon suffix, e.g. loop:
- Instructions can be indented for readability
- Labels are used as jump targets for control flow

## Examples
```
mov 10 acc    # acc = 10
sav           # bak = 10
add 5         # acc = 15
sub 5         # acc = 10
mov nil acc   # acc = 0
jmp label1
```

```
label1:
  add 1            # acc += 1 / acc = 1
  jez zero         # if acc == 0, jump to zero
  jnz nonzero      # if acc != 0, jump to nonzero
  jgz greater      # if acc > 0, jump to greater
  jlz lesser       # if acc < 0, jump to lesser

```

### ex2
```
mov 102 acc # acc = 102
mov acc out # print 102 in console
```
