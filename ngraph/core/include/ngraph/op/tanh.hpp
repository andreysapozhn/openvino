// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "ngraph/op/util/unary_elementwise_arithmetic.hpp"

namespace ngraph {
namespace op {
namespace v0 {
/// \brief Elementwise hyperbolic tangent operation.
class NGRAPH_API Tanh : public util::UnaryElementwiseArithmetic {
public:
    NGRAPH_RTTI_DECLARATION;

    /// \brief Constructs a hyperbolic tangent operation.
    ///
    /// \param arg Node that produces the input tensor.
    Tanh(const Output<Node>& arg);
    Tanh() = default;

    bool visit_attributes(AttributeVisitor& visitor) override;
    virtual std::shared_ptr<Node> clone_with_new_inputs(const OutputVector& new_args) const override;
    bool evaluate(const HostTensorVector& outputs, const HostTensorVector& inputs) const override;
    bool has_evaluate() const override;
};
}  // namespace v0
using v0::Tanh;
}  // namespace op
}  // namespace ngraph
