
# #08. Constant Nodes

<div class="ui segment">
<span class="ui ribbon label">Note</span>
This is a web-version of a tutorial chapter embedded right into the XOD IDE.
To get a better learning experience we recommend to
<a href="../install/">install the IDE</a>, launch it, and you’ll see the
same tutorial there.
</div>

Previously, we specified the node values manually with the Inspector. However,
the data can be transferred from the output pin of one node to the input pin of
another node. This is the core idea of XOD programming.

![Patch](./patch.png)

## Test circuit

<div class="ui segment">
<span class="ui ribbon label">Note</span>
The circuit is the same as for the previous lesson.
</div>

![Circuit](./circuit.fz.png)

[↓ Download as Fritzing project](./circuit.fzz)

## Instructions

1. Click on the `constant-number` node, and set the value of 1 for the `VAL`
   output in the Inspector.
2. Connect the `VAL` pin with the `LUM` pin on the LED1 node. To do this,
   simply click on the `VAL` pin and then on the `LUM` pin. You will then see a
   green line connecting the two pins. This line is called a *link*.
3. Make another link between the `VAL` pin and `LUM` pin on the LED2 node.
4. Upload the patch to the Arduino.

![Screencast](./screencast.gif)

The both LEDs should be on. Now the values of the `LUM` pins of both `led`
nodes are taken form the `VAL` pin of the `constant-number` node.

The `constant-number` node transfers data to the `VAL` pin. You can specify the
data in the Inspector. Try to change values of the constant and upload again.
See how LEDs’ brightness changes.

When played enough follow to the [next lesson](../09-pot/).
