
/*=============================================================================
 *
 *
 * Main loop components
 *
 *
 =============================================================================*/

namespace xod {

// Define/allocate persistent storages (state, timeout, output data) for all nodes
{{#each nodes}}
{{~ mergePins }}
{{#each outputs }}
{{ decltype type value }} node_{{ ../id }}_output_{{ pinKey }} = {{ cppValue type value }};
{{/each}}
{{ ns patch }}::Node node_{{ id }} = {
    { }, // state
    0, // timeoutAt
  {{#each outputs}}
    node_{{ ../id }}_output_{{ pinKey }},
  {{/each}}
  {{#each outputs}}
    true, // {{ pinKey }} dirty
  {{/each}}
    true // node itself dirty
};
{{/each}}

void idle() {
    TimeMs now = millis();
    {{#each nodes}}
    detail::checkTriggerTimeout(&node_{{ id }}, now);
    {{/each}}
}

void runTransaction() {
    g_transactionTime = millis();

    XOD_TRACE_F("Transaction started, t=");
    XOD_TRACE_LN(g_transactionTime);

    // defer-* nodes are always at the very bottom of the graph,
    // so no one will recieve values emitted by them.
    // We must evaluate them before everybody else
    // to give them a chance to emit values.
    {{#eachDeferNode }}
    {
        Storage& node = node_{{ id }};
        if (detail::isTimedOut(&node)) {
            node.isOutputDirty_OUT = true;

            // mark downstream nodes dirty
          {{#each outputs }}
            {{#each to}}
            node_{{ this }}.isNodeDirty = true;
            {{/each}}
          {{/each}}

            // Clear node dirty flag (do not clear output dirieness), so it
            // will evaluate on the regular pass only if it pushed a new value
            // again.
            //
            // We must save dirty output flags, or `isInputDirty` will not work
            // correctly in downstream nodes.
            node.isNodeDirty = false;

            detail::clearTimeout(&node);
        }
    }
    {{/eachDeferNode }}

    // Evaluate all dirty nodes
  {{#each nodes}}
    { // {{ ns patch }} #{{ id }}
        if (node_{{ id }}.isNodeDirty) {
            {{ns patch }}::ContextObject ctxObj;
            ctxObj._node = &node_{{ id }};

          {{#each inputs }}
            // {{ json this }}
            ctxObj._input_{{ pinKey }} = node_{{ nodeId }}.output_{{ fromPinKey }};
            ctxObj._isInputDirty_{{ pinKey }} = node_{{ nodeId }}.isOutputDirty_{{ fromPinKey }};
          {{/each}}

            {{ ns patch }}::evaluate(&ctxObj);

            // mark downstream nodes dirty
          {{#each outputs }}
            {{#each to}}
            node_{{ this }}.isNodeDirty |= node_{{ ../../id }}.isOutputDirty_{{ ../pinKey }};
            {{/each}}
          {{/each}}
        }
    }
  {{/each}}

    // Clear dirtieness and timeouts for all nodes and pins
  {{#each nodes }}
    detail::clearDirtieness(&node_{{ id }});
  {{/each }}

    XOD_TRACE_F("Transaction completed, t=");
    XOD_TRACE_LN(millis());
}

} // namespace xod


// TODO: move to own file
//----------------------------------------------------------------------------
// Entry point
//----------------------------------------------------------------------------
void setup() {
    // FIXME: looks like there is a rounding bug. Waiting for 100ms fights it
    delay(100);
#ifdef XOD_DEBUG
    DEBUG_SERIAL.begin(115200);
#endif
    XOD_TRACE_FLN("\n\nProgram started");
}

void loop() {
    xod::idle();
    xod::runTransaction();
}
