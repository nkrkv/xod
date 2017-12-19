
namespace xod {

void runTransaction() {
    g_transactionTime = millis();

    XOD_TRACE_F("Transaction started, t=");
    XOD_TRACE_LN(g_transactionTime);

    // defer-* nodes are always at the very bottom of the graph,
    // so no one will recieve values emitted by them.
    // We must evaluate them before everybody else
    // to give them a chance to emit values.
    for (NodeId nid = NODE_COUNT - DEFER_NODE_COUNT; nid < NODE_COUNT; ++nid) {
        if (isTimedOut(nid)) {
            evaluateNode(nid);
            // Clear node dirty flag, so it will evaluate
            // on "regular" pass only if it has a dirty input.
            // We must save dirty output flags,
            // or 'isInputDirty' will not work correctly in "downstream" nodes.
            g_dirtyFlags[nid] &= ~0x1;
            clearTimeout(nid);
        }
    }

  {{#each nodes}}
    static {{ ns patch }}::Storage storage_{{ id }};
  {{/each}}

  {{#each nodes}}
    {
        constexpr NodeId nid = {{ id }};
        {{ns patch }}::ContextObject ctxObj;

      {{#each inputs }}
        // {{ json this }}
        ctxObj._input_{{ pinKey }} = storage_{{ nodeId }}.output_{{ fromPinKey }};
        ctxObj._isInputDirty_{{ pinKey }} = storage_{{ nodeId }}.isOutputDirty_{{ fromPinKey }};
      {{/each}}

        if (isNodeDirty(nid)) {
            {{ ns patch }}::evaluate(&ctxObj);
            if (isTimedOut(nid)) // note [1]
                clearTimeout(nid);
        }
    }
  {{/each}}

    // [1] if the schedule is stale, clear timeout so that
    // the node would not be marked dirty again in idle

    // Clear dirtieness for all nodes and pins
    memset(g_dirtyFlags, 0, sizeof(g_dirtyFlags));

    XOD_TRACE_F("Transaction completed, t=");
    XOD_TRACE_LN(millis());
}

} // namespace xod


{{!-- Template for program graph --}}
{{!-- Accepts the context with list of Nodes --}}
/*=============================================================================
 *
 *
 * Program graph
 *
 *
 =============================================================================*/

namespace xod {

    //-------------------------------------------------------------------------
    // Dynamic data
    //-------------------------------------------------------------------------
    /*
  {{#each nodes}}
  {{mergePins }}
    // Storage of #{{ id }} {{ patch.owner }}/{{ patch.libName }}/{{ patch.patchName }}
    {{#each outputs }}
    {{ decltype type value }} node_{{ ../id }}_output_{{ pinKey }} = {{ cppValue type value }};
    {{~/each}}

    {{ns patch }}::Storage storage_{{ id }} = {
        { }, // state
      {{#each outputs }}
        node_{{ ../id }}_output_{{ pinKey }} {{#unless @last }},{{/unless }}
      {{/each}}
    };
  {{/each}}
    */

    DirtyFlags g_dirtyFlags[NODE_COUNT] = {
        {{#each nodes}}DirtyFlags({{ dirtyFlags }}){{#unless @last}},
        {{/unless}}{{/each}}
    };

    TimeMs g_schedule[NODE_COUNT] = { 0 };

    //-------------------------------------------------------------------------
    // Static (immutable) data
    //-------------------------------------------------------------------------
    /*
  {{#each nodes}}
  {{mergePins }}
    // Wiring of #{{ id }} {{ patch.owner }}/{{ patch.libName }}/{{ patch.patchName }}
  {{#each outputs }}
    const NodeId outLinks_{{ ../id }}_{{ pinKey }}[] PROGMEM = { {{#each to }}{{ this }}, {{/each}}NO_NODE };
  {{/each}}
    const {{ns patch }}::Wiring wiring_{{ id }} PROGMEM = {
        &{{ns patch }}::evaluate,
        // inputs (UpstreamPinRef’s initializers)
      {{#each inputs }}
        {{#exists nodeId }}
        { NodeId({{ nodeId }}),
            {{ns patch }}::output_{{ fromPinKey  }}::INDEX,
            {{ns patch }}::output_{{ fromPinKey  }}::STORAGE_OFFSET }, // input_{{ pinKey }}
        {{else }}
        { NO_NODE, 0, 0 }, // input_{{ pinKey }}
        {{/exists }}
      {{/each}}
        // outputs (NodeId list binding)
      {{#each outputs }}
        outLinks_{{ ../id }}_{{ pinKey }}{{#unless @last }},{{/unless }} // output_{{ pinKey }}
      {{/each}}
    };
  {{/each}}

    // PGM array with pointers to PGM wiring information structs
    const void* const g_wiring[NODE_COUNT] PROGMEM = {
      {{#each nodes}}
        &wiring_{{ id }}{{#unless @last }},{{/unless }}
      {{/each}}
    };

    // PGM array with pointers to RAM-located storages
    void* const g_storages[NODE_COUNT] PROGMEM = {
      {{#each nodes}}
        &storage_{{ id }}{{#unless @last }},{{/unless }}
      {{/each}}
    };*/
}

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
