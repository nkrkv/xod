{{!-- Template for GENERATED_CODE token inside each patch implementation --}}
{{!-- Accepts the Node context --}}

struct Storage {
    State state;
  {{#each outputs}}
    {{ cppType type }} output_{{ pinKey }};
  {{/each}}

  {{#each outputs}}
    bool isOutputDirty_{{ pinKey }} : 1;
  {{/each}}
};

enum Pins {
  {{#each inputs}}
    input_{{ pinKey }},
  {{/each}}
  {{#each outputs}}
    input_{{ pinKey }},
  {{/each}}
    PIN_COUNT {{!-- A dummy to forget trailing comma problems --}}
};

struct Context {
    Storage* storage;

  {{#each inputs}}
    {{ cppType type }}* _input_{{ pinKey }};
  {{/each}}

  {{#each inputs}}
    bool _isInputDirty_{{ pinKey }};
  {{/each}}
};

{{#each inputs}}
template<> input_{{ pinKey }}::T getValue<input_{{ pinKey }}>(Context ctx) {
    return *ctx._input_{{pinKey }};
}
{{/each}}
{{#each outputs}}
template<> output_{{ pinKey }}::T getValue<output_{{ pinKey }}>(Context ctx) {
    return ctx.storage->otput_{{ pinKey }};
}
{{/each}}

{{#each outputs}}
template<> emitValue<output_{{ pinKey }}>(Context ctx, output_{{ pinKey }}::T val) {
    ctx.storage->otput_{{ pinKey }} = val;
    ctx.storage->isOutputDirty_{{ pinKey }} = true;
}
{{/each}}

State* getState(Context ctx) {
    return &ctx.storage->state;
}
