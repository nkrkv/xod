
# #14. Adjusting Map Range

<div class="ui segment">
<span class="ui ribbon label">Note</span>
This is a web-version of a tutorial chapter embedded right into the XOD IDE.
To get a better learning experience we recommend to
<a href="../install/">install the IDE</a>, launch it, and you’ll see the
same tutorial there.
</div>

After the previous lesson, the `map-range` node linearly maps the 20–30°C range
to 0–90 degrees.

![Patch](./patch.png)

However, what will happen if the `X` pin receives data that is outside the input
range (10 or 42, for example)?

Easy. Anything that is out of range will be rounded to `Smin` or `Smax`. Thus,
if `X` receives a value of 10, the `map-range` node will accept it as 20, and
42 will be accepted as 30.

Now, try to make the servo rotate smoothly from 0–180 degrees when the
temperature changes from 20°C to 27°C.

## Test circuit

<div class="ui segment">
<span class="ui ribbon label">Note</span>
The circuit is the same as for the previous lesson.
</div>

![Circuit](./circuit.fz.png)

[↓ Download as Fritzing project](./circuit.fzz)

## Instructions

1. Set the input range (Smin to Smax) to 20–27.
2. Set the output range (Tmin to Tmax) to 0–1.
3. (Optional) If you prefer °F, you can easily translate the data from the
   thermometer with the `c-to-f` node in `xod/units`. The range will be
   68–77°F.
4. Upload.

![Screencast](./screencast.gif)

Now the servo is more sensitive to the changes of the temperature sensor.

You will use the `map-range` node quite often. It is located in `xod/core`.

Try to play with the map values and when done follow to the [next
lesson](../15-buttons/).
