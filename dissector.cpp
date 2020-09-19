#include <iostream>

#include "config.h"
#include <epan/packet.h>
#include <glib-2.0/glib/gtypes.h>

#define PROTO_PORT 9001

static int proto_redescomp = -1;
static int hf_redescomp_pdu_size = -1;
static int hf_redescomp_pdu_data = -1;
static gint ett_redescomp = -1;

int plugin_version = 0x010001;
int plugin_want_major = 3;
int plugin_want_minor = 2;

void proto_register_redescomp(void) {
    static hf_register_info hf[] = {
        {&hf_redescomp_pdu_size,
         {"Length", "redescomp01.length",
          FT_UINT32, BASE_DEC,
          NULL, 0x0,
          NULL, HFILL}},
        {&hf_redescomp_pdu_data,
         {"Message", "redescomp01.message",
          FT_STRING, BASE_NONE,
          NULL, 0x0,
          NULL, HFILL}}};

    /* Setup protocol subtree array */
    static gint *ett[] = {
        &ett_redescomp};

    proto_redescomp = proto_register_protocol(
        "RedesComp01 Sample Protocol", /* name        */
        "RedesComp01",                 /* short_name  */
        "redescomp"                    /* filter_name */
    );

    proto_register_field_array(proto_redescomp, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));
}

static int dissect_redescomp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree _U_, void *data _U_) {
    gint offset = 0;

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "RedesComp01");

    /* Clear the info column */
    col_clear(pinfo->cinfo, COL_INFO);

    proto_item *ti = proto_tree_add_item(tree, proto_redescomp, tvb, 0, -1, ENC_NA);
    proto_tree *redescomp_tree = proto_item_add_subtree(ti, ett_redescomp);
    proto_tree_add_item(redescomp_tree, hf_redescomp_pdu_size, tvb, offset, 4, ENC_BIG_ENDIAN);

    offset += 4;
    proto_tree_add_item(redescomp_tree, hf_redescomp_pdu_data, tvb, offset, tvb_ensure_captured_length_remaining(tvb, offset), ENC_NA);

    return tvb_captured_length(tvb);
}

void proto_reg_handoff_redescomp(void) {
    static dissector_handle_t redescomp_handle;

    redescomp_handle = create_dissector_handle(dissect_redescomp, proto_redescomp);
    dissector_add_uint("tcp.port", PROTO_PORT, redescomp_handle);
}

extern "C" void plugin_register(void) {
    static proto_plugin plugin_foo;

    plugin_foo.register_protoinfo = proto_register_redescomp;
    plugin_foo.register_handoff = proto_reg_handoff_redescomp;
    proto_register_plugin(&plugin_foo);
}
