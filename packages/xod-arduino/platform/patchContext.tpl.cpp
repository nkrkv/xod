{{!-- Template for GENERATED_CODE token inside each patch implementation --}}
{{!-- Accepts the Node context --}}

struct Node {
    State state;
    TimeMs timeoutAt;
  {{#each outputs}}
    {{ cppType type }} output_{{ pinKey }};
  {{/each}}

  {{#each outputs}}
    bool isOutputDirty_{{ pinKey }} : 1;
  {{/each}}
    bool isNodeDirty : 1;
};

{{#each inputs}}
struct input_{{ pinKey }} {
    using T = {{ cppType type }};
};
{{/each}}
{{#each outputs}}
struct output_{{ pinKey }} {
    using T = {{ cppType type }};
};
{{/each}}

struct ContextObject {
    Node* _node;

  {{#each inputs}}
    {{ cppType type }} _input_{{ pinKey }};
  {{/each}}

  {{#each inputs}}
    bool _isInputDirty_{{ pinKey }};
  {{/each}}
};

using Context = ContextObject*;

template<typename PinT> typename PinT::T getValue(Context ctx) {
    // TODO: throw compile error
}

{{#each inputs}}
template<> input_{{ pinKey }}::T getValue<input_{{ pinKey }}>(Context ctx) {
    return ctx->_input_{{pinKey }};
}
{{/each}}
{{#each outputs}}
template<> output_{{ pinKey }}::T getValue<output_{{ pinKey }}>(Context ctx) {
    return ctx->_node->output_{{ pinKey }};
}
{{/each}}

template<typename InputT> bool isInputDirty(Context ctx) {
    // TODO: throw compile error
}

{{#each inputs}}
template<> bool isInputDirty<input_{{ pinKey }}>(Context ctx) {
    return ctx->_isInputDirty_{{ pinKey }};
}
{{/each}}

template<typename OutputT> void emitValue(Context ctx, typename OutputT::T val) {
    // TODO: throw compile error
}

{{#each outputs}}
template<> void emitValue<output_{{ pinKey }}>(Context ctx, {{ cppType type }} val) {
    ctx->_node->output_{{ pinKey }} = val;
    ctx->_node->isOutputDirty_{{ pinKey }} = true;
}
{{/each}}

State* getState(Context ctx) {
    return &ctx->_node->state;
}
