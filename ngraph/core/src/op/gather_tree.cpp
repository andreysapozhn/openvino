// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "ngraph/op/gather_tree.hpp"

#include "itt.hpp"
#include "ngraph/shape.hpp"

using namespace std;
using namespace ngraph;

NGRAPH_RTTI_DEFINITION(op::v1::GatherTree, "GatherTree", 1);

op::v1::GatherTree::GatherTree(const Output<Node>& step_ids,
                               const Output<Node>& parent_idx,
                               const Output<Node>& max_seq_len,
                               const Output<Node>& end_token)
    : Op({step_ids, parent_idx, max_seq_len, end_token}) {
    constructor_validate_and_infer_types();
}

shared_ptr<Node> op::v1::GatherTree::clone_with_new_inputs(const OutputVector& new_args) const {
    NGRAPH_OP_SCOPE(v1_GatherTree_clone_with_new_inputs);
    check_new_args_count(this, new_args);
    return make_shared<v1::GatherTree>(new_args.at(0), new_args.at(1), new_args.at(2), new_args.at(3));
}

bool ngraph::op::v1::GatherTree::visit_attributes(AttributeVisitor& visitor) {
    NGRAPH_OP_SCOPE(v1_GatherTree_visit_attributes);
    return true;
}

void op::v1::GatherTree::validate_and_infer_types() {
    NGRAPH_OP_SCOPE(v1_GatherTree_validate_and_infer_types);
    const auto& step_ids_rank = get_input_partial_shape(0);
    const auto& parent_idx_rank = get_input_partial_shape(1);
    const auto& max_seq_len_rank = get_input_partial_shape(2);
    const auto& end_token_rank = get_input_partial_shape(3);

    NODE_VALIDATION_CHECK(this,
                          step_ids_rank.rank().is_dynamic() || step_ids_rank.rank().get_length() == 3,
                          "step_ids input rank must equal to 3 (step_ids rank: ",
                          step_ids_rank.rank().get_length(),
                          ")");

    NODE_VALIDATION_CHECK(this,
                          parent_idx_rank.rank().is_dynamic() || parent_idx_rank.rank().get_length() == 3,
                          "parent_idx input rank must equal to 3 (parent_idx rank: ",
                          parent_idx_rank.rank().get_length(),
                          ")");

    NODE_VALIDATION_CHECK(this,
                          max_seq_len_rank.rank().is_dynamic() || max_seq_len_rank.rank().get_length() == 1,
                          "max_seq_len input rank must equal to 1 (max_seq_len rank: ",
                          max_seq_len_rank.rank().get_length(),
                          ")");

    NODE_VALIDATION_CHECK(this,
                          end_token_rank.rank().is_dynamic() || end_token_rank.rank().get_length() == 0,
                          "end_token input rank must be scalar (end_token rank: ",
                          end_token_rank.rank().get_length(),
                          ")");

    const auto& step_ids_et = get_input_element_type(0);
    set_output_type(0, step_ids_et, step_ids_rank);
}
