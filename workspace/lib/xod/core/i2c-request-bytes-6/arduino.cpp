
{{#global}}
#include <Wire.h>
{{/global}}

struct State {
};

{{ GENERATED_CODE }}

uint8_t readByteSync() {
    while (!Wire.available())
        continue;

    return Wire.read();
}

void evaluate(Context ctx) {
    if (!isInputDirty<input_UPD>(ctx))
        return;

    Wire.begin();

    Wire.requestFrom((uint8_t)getValue<input_ADDR>(ctx), 6);

    emitValue<output_B0>(ctx, readByteSync());
    emitValue<output_B1>(ctx, readByteSync());
    emitValue<output_B2>(ctx, readByteSync());
    emitValue<output_B3>(ctx, readByteSync());
    emitValue<output_B4>(ctx, readByteSync());
    emitValue<output_B5>(ctx, readByteSync());

    emitValue<output_DONE>(ctx, 1);
}
