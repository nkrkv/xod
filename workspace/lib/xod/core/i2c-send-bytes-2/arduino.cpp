
{{#global}}
#include <Wire.h>
{{/global}}

struct State {
};

{{ GENERATED_CODE }}

void evaluate(Context ctx) {
    if (!isInputDirty<input_UPD>(ctx))
        return;

    Wire.begin();
    Wire.beginTransmission((uint8_t)getValue<input_ADDR>(ctx));
    Wire.write((uint8_t)getValue<input_B0>(ctx));
    Wire.write((uint8_t)getValue<input_B1>(ctx));
    Wire.endTransmission();

    emitValue<output_DONE>(ctx, 1);
}
