---
title: Logic Nodes
---

# #16. Logic Nodes

<div class="ui segment note">
<span class="ui ribbon label">Note</span>
This is a web-version of a tutorial chapter embedded right into the XOD IDE.
To get a better learning experience we recommend to
<a href="../install/">install the IDE</a>, launch it, and you’ll see the
same tutorial there.
</div>

Boolean values are cool. You can use them to construct complex logic systems.
In XOD, you will find a lot of nodes for this purpose. All of them are located
in `xod/core`.

![Patch](./patch.png)

* `and` returns `true` if both A *and* B are `true`
* `or` returns `true` if A, B, or both are `true`
* `xor` returns `true` if only one of A and B is `true`
* `not` inverts the input value
* `nand` is an inverted `and`
* `nor` is an inverted `or`

## Test circuit

![Circuit](./circuit.fz.png)

[↓ Download as a Fritzing project](./circuit.fzz)

In the example patch above the `and` node returns `false` until both buttons
are pressed. The `not` node inverts the value from the `and` node, so the `LUM`
pin receives its `true` value (1). Thus, the LED turns on.

Pressing one of the buttons changes nothing. The LED will turn off only when
the `LUM` pin receives false (0). This will happen only if the `not` node
receives the true value, which is possible only if both buttons are pressed at
the same time.

[Next lesson →](../17-ldr/)
