---
title: Buttons
---

# #15. Buttons

<div class="ui segment note">
<span class="ui ribbon label">Note</span>
This is a web-version of a tutorial chapter embedded right into the XOD IDE.
To get a better learning experience we recommend to
<a href="../install/">install the IDE</a>, launch it, and you’ll see the
same tutorial there.
</div>

So, buttons!

We have already added the `button` node from `xod/common` hardware.

![Patch](./patch.png)

The button node has a purple output pin called the `PRS`. This pin returns a new
type of data: *boolean*.

Boolean data can have either of two values: true or false.
In our case, the `button` node returns a value of `false` when idle and `true`
while the button is being pressed.

Good news, in XOD boolean and number data types are compatible. Here are two
rules of datacasting:

* Boolean-to-number: if you send a boolean false to a numeric (green) input, it
  will be interpreted as a numeric 0; if you send a boolean true, it will be
  interpreted as a numeric 1.
* Number-to-boolean: if you send any numeric value except 0 to a boolean
  (purple) input; it will be interpreted as true, and if you send 0, it will be
  interpreted as false.

## Test circuit

![Circuit](./circuit.fz.png)

[↓ Download as a Fritzing project](./circuit.fzz)

## Instructions

1. Assemble the circuit.
2. Link the `PRS` pin to the `LUM` pin.
3. Upload your patch.

![Screencast](./screencast.gif)

When you press the button the `button` node sets the `PRS` pin to `true`, the
`led` node (`LUM` pin) interprets it as 1, and the LED turns on at full
brightness.

Send a WOW morse-signal with the button and go to the [next lesson](../16-logic).
