// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "ngraph/op/constant.hpp"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <ngraph/validation_util.hpp>
#include <sstream>

#include "itt.hpp"
#include "ngraph/log.hpp"
#include "ngraph/op/util/attr_types.hpp"
#include "ngraph/util.hpp"

using namespace ngraph;
using namespace std;

template <typename T>
static inline string to_cpp_string(T value) {
    string rc;
    if (std::isnan(value)) {
        rc = "NAN";
    } else if (std::isinf(value)) {
        rc = (value > 0 ? "INFINITY" : "-INFINITY");
    } else {
        stringstream ss;
        ss << value;
        rc = ss.str();
    }
    return rc;
}

NGRAPH_RTTI_DEFINITION(op::Constant, "Constant", 0);

op::Constant::Constant(const shared_ptr<runtime::Tensor>& tensor) {
    m_element_type = tensor->get_element_type();
    m_shape = tensor->get_shape();
    // Share data from HostTensor if we work with it
    // And copy data in other cas
    if (auto hostTensor = std::dynamic_pointer_cast<runtime::HostTensor>(tensor)) {
        m_data = make_shared<runtime::SharedBuffer<std::shared_ptr<runtime::Tensor>>>(
            static_cast<char*>(hostTensor->get_data_ptr()),
            tensor->get_size_in_bytes(),
            tensor);
    } else {
        constructor_validate_and_infer_types();
        allocate_buffer();
        tensor->read(get_data_ptr_nc(), tensor->get_size_in_bytes());
        m_all_elements_bitwise_identical = are_all_data_elements_bitwise_identical();
    }
    constructor_validate_and_infer_types();
}

op::Constant::Constant(const element::Type& type, const Shape& shape, const std::vector<std::string>& values)
    : Constant(type, shape) {
    NODE_VALIDATION_CHECK(this,
                          values.size() == shape_size(m_shape) || values.size() == 1,
                          "Did not get the expected number of literals for a constant of shape ",
                          m_shape,
                          " (got ",
                          values.size(),
                          ", expected ",
                          shape_size(m_shape),
                          ".");

    using Type_t = element::Type_t;

    if (values.size() == 1 && shape_size(m_shape) != 1) {
        // broadcast single value
        switch (m_element_type) {
        case Type_t::boolean:
            fill_data<Type_t::boolean>(stoi(values[0]));
            break;
        case Type_t::bf16:
            fill_data<Type_t::bf16>(parse_string<float>(values[0]));
            break;
        case Type_t::f16:
            fill_data<Type_t::f16>(parse_string<float>(values[0]));
            break;
        case Type_t::f32:
            fill_data<Type_t::f32>(parse_string<float>(values[0]));
            break;
        case Type_t::f64:
            fill_data<Type_t::f64>(parse_string<double>(values[0]));
            break;
        case Type_t::i4:
            fill_data<Type_t::i4>(parse_string<int64_t>(values[0]));
            break;
        case Type_t::i8:
            fill_data<Type_t::i8>(parse_string<int64_t>(values[0]));
            break;
        case Type_t::i16:
            fill_data<Type_t::i16>(parse_string<int64_t>(values[0]));
            break;
        case Type_t::i32:
            fill_data<Type_t::i32>(parse_string<int64_t>(values[0]));
            break;
        case Type_t::i64:
            fill_data<Type_t::i64>(parse_string<int64_t>(values[0]));
            break;
        case Type_t::u1:
            fill_data<Type_t::u1>(stoi(values[0]));
            break;
        case Type_t::u4:
            fill_data<Type_t::u4>(parse_string<uint64_t>(values[0]));
            break;
        case Type_t::u8:
            fill_data<Type_t::u8>(parse_string<uint64_t>(values[0]));
            break;
        case Type_t::u16:
            fill_data<Type_t::u16>(parse_string<uint64_t>(values[0]));
            break;
        case Type_t::u32:
            fill_data<Type_t::u32>(parse_string<uint64_t>(values[0]));
            break;
        case Type_t::u64:
            fill_data<Type_t::u64>(parse_string<uint64_t>(values[0]));
            break;
        case Type_t::undefined:
            throw std::runtime_error("deserialize unsupported type undefined");
        case Type_t::dynamic:
            throw std::runtime_error("deserialize unsupported type dynamic");
        }
        m_all_elements_bitwise_identical = true;
    } else {
        switch (m_element_type) {
        case Type_t::boolean:
            write_buffer<Type_t::boolean>(parse_string<uint8_t>(values));
            break;
        case Type_t::bf16:
            write_buffer<Type_t::bf16>(parse_string<float>(values));
            break;
        case Type_t::f16:
            write_buffer<Type_t::f16>(parse_string<float>(values));
            break;
        case Type_t::f32:
            write_buffer<Type_t::f32>(parse_string<float>(values));
            break;
        case Type_t::f64:
            write_buffer<Type_t::f64>(parse_string<double>(values));
            break;
        case Type_t::i4:
            write_buffer<Type_t::i4>(parse_string<int8_t>(values));
            break;
        case Type_t::i8:
            write_buffer<Type_t::i8>(parse_string<int8_t>(values));
            break;
        case Type_t::i16:
            write_buffer<Type_t::i16>(parse_string<int16_t>(values));
            break;
        case Type_t::i32:
            write_buffer<Type_t::i32>(parse_string<int32_t>(values));
            break;
        case Type_t::i64:
            write_buffer<Type_t::i64>(parse_string<int64_t>(values));
            break;
        case Type_t::u1:
            write_buffer<Type_t::u1>(parse_string<uint8_t>(values));
            break;
        case Type_t::u4:
            write_buffer<Type_t::u4>(parse_string<uint8_t>(values));
            break;
        case Type_t::u8:
            write_buffer<Type_t::u8>(parse_string<uint8_t>(values));
            break;
        case Type_t::u16:
            write_buffer<Type_t::u16>(parse_string<uint16_t>(values));
            break;
        case Type_t::u32:
            write_buffer<Type_t::u32>(parse_string<uint32_t>(values));
            break;
        case Type_t::u64:
            write_buffer<Type_t::u64>(parse_string<uint64_t>(values));
            break;
        case Type_t::undefined:
            throw std::runtime_error("deserialize unsupported type undefined");
        case Type_t::dynamic:
            throw std::runtime_error("deserialize unsupported type dynamic");
        }
        m_all_elements_bitwise_identical = are_all_data_elements_bitwise_identical();
    }
}

op::Constant::Constant(const element::Type& type, const Shape& shape) : m_element_type(type), m_shape(shape) {
    allocate_buffer();
    constructor_validate_and_infer_types();
}

void op::Constant::allocate_buffer() {
    m_data = make_shared<runtime::AlignedBuffer>(mem_size(), host_alignment());
    std::memset(m_data->get_ptr(), 0, m_data->size());
}

op::Constant::Constant(const element::Type& type, const Shape& shape, const void* data) : Constant(type, shape) {
    size_t size = ceil(shape_size(m_shape) * m_element_type.bitwidth() / 8.f);
    std::memcpy(get_data_ptr_nc(), data, size);
    m_all_elements_bitwise_identical = are_all_data_elements_bitwise_identical();
}

op::Constant::Constant(const Constant& other) {
    m_element_type = other.m_element_type;
    m_shape = other.m_shape;
    m_data = other.m_data;
    m_all_elements_bitwise_identical = other.m_all_elements_bitwise_identical;
    constructor_validate_and_infer_types();
}

op::Constant::Constant(const Constant& other, const Shape& new_shape) {
    NGRAPH_CHECK(shape_size(other.m_shape) == shape_size(new_shape),
                 "Shape size " + std::to_string(shape_size(new_shape)) + " is not equal to " +
                     std::to_string(shape_size(other.m_shape)));
    m_element_type = other.m_element_type;
    m_shape = new_shape;
    m_data = other.m_data;
    m_all_elements_bitwise_identical = other.m_all_elements_bitwise_identical;
    constructor_validate_and_infer_types();
}

op::Constant::~Constant() {}

string op::Constant::convert_value_to_string(size_t index) const {
    string rc;
#if defined(__GNUC__) && !(__GNUC__ == 4 && __GNUC_MINOR__ == 8)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic error "-Wswitch"
#    pragma GCC diagnostic error "-Wswitch-enum"
#endif
    using Type_t = element::Type_t;
    switch (get_element_type()) {
    case Type_t::boolean:
        rc = to_string(get_element_value<Type_t::boolean>(index));
        break;
    case Type_t::bf16:
        rc = to_cpp_string(static_cast<float>(get_element_value<Type_t::bf16>(index)));
        break;
    case Type_t::f16:
        rc = to_cpp_string(static_cast<float>(get_element_value<Type_t::f16>(index)));
        break;
    case Type_t::f32:
        rc = to_cpp_string(get_element_value<Type_t::f32>(index));
        break;
    case Type_t::f64:
        rc = to_cpp_string(get_element_value<Type_t::f64>(index));
        break;
    case Type_t::i4:
        rc = to_string(get_element_value<Type_t::i4>(index));
        break;
    case Type_t::i8:
        rc = to_string(get_element_value<Type_t::i8>(index));
        break;
    case Type_t::i16:
        rc = to_string(get_element_value<Type_t::i16>(index));
        break;
    case Type_t::i32:
        rc = to_string(get_element_value<Type_t::i32>(index));
        break;
    case Type_t::i64:
        rc = to_string(get_element_value<Type_t::i64>(index));
        break;
    case Type_t::u1:
        rc = to_string(get_element_value<Type_t::u1>(index));
        break;
    case Type_t::u4:
        rc = to_string(get_element_value<Type_t::u4>(index));
        break;
    case Type_t::u8:
        rc = to_string(get_element_value<Type_t::u8>(index));
        break;
    case Type_t::u16:
        rc = to_string(get_element_value<Type_t::u16>(index));
        break;
    case Type_t::u32:
        rc = to_string(get_element_value<Type_t::u32>(index));
        break;
    case Type_t::u64:
        rc = to_string(get_element_value<Type_t::u64>(index));
        break;
    case Type_t::undefined:
        throw runtime_error("unsupported type");
    case Type_t::dynamic:
        throw runtime_error("unsupported type");
    }
#if defined(__GNUC__) && !(__GNUC__ == 4 && __GNUC_MINOR__ == 8)
#    pragma GCC diagnostic pop
#endif
    return rc;
}

vector<string> op::Constant::get_value_strings() const {
    vector<string> rc;

#if defined(__GNUC__) && !(__GNUC__ == 4 && __GNUC_MINOR__ == 8)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic error "-Wswitch"
#    pragma GCC diagnostic error "-Wswitch-enum"
#endif
    switch (get_element_type()) {
    case element::Type_t::boolean:
        for (int value : get_vector<char>()) {
            rc.push_back(to_string(value));
        }
        break;
    case element::Type_t::bf16:
        for (bfloat16 value : get_vector<bfloat16>()) {
            rc.push_back(to_cpp_string(static_cast<float>(value)));
        }
        break;
    case element::Type_t::f16:
        for (float16 value : get_vector<float16>()) {
            rc.push_back(to_cpp_string(static_cast<float>(value)));
        }
        break;
    case element::Type_t::f32:
        for (float value : get_vector<float>()) {
            rc.push_back(to_cpp_string(value));
        }
        break;
    case element::Type_t::f64:
        for (double value : get_vector<double>()) {
            rc.push_back(to_cpp_string(value));
        }
        break;
    case element::Type_t::i4:
        for (auto value : cast_vector<int8_t>()) {
            rc.push_back(to_string(value));
        }
        break;
    case element::Type_t::i8:
        for (int value : get_vector<int8_t>()) {
            rc.push_back(to_string(value));
        }
        break;
    case element::Type_t::i16:
        for (int value : get_vector<int16_t>()) {
            rc.push_back(to_string(value));
        }
        break;
    case element::Type_t::i32:
        for (int32_t value : get_vector<int32_t>()) {
            rc.push_back(to_string(value));
        }
        break;
    case element::Type_t::i64:
        for (int64_t value : get_vector<int64_t>()) {
            rc.push_back(to_string(value));
        }
        break;
    case element::Type_t::u1:
    case element::Type_t::u4:
        for (auto value : cast_vector<uint8_t>()) {
            rc.push_back(to_string(value));
        }
        break;
    case element::Type_t::u8:
        for (uint32_t value : get_vector<uint8_t>()) {
            rc.push_back(to_string(value));
        }
        break;
    case element::Type_t::u16:
        for (uint32_t value : get_vector<uint16_t>()) {
            rc.push_back(to_string(value));
        }
        break;
    case element::Type_t::u32:
        for (uint32_t value : get_vector<uint32_t>()) {
            rc.push_back(to_string(value));
        }
        break;
    case element::Type_t::u64:
        for (uint64_t value : get_vector<uint64_t>()) {
            rc.push_back(to_string(value));
        }
        break;
    case element::Type_t::undefined:
    case element::Type_t::dynamic:
        throw runtime_error("unsupported type");
    }
#if defined(__GNUC__) && !(__GNUC__ == 4 && __GNUC_MINOR__ == 8)
#    pragma GCC diagnostic pop
#endif

    return rc;
}

Shape op::Constant::get_shape_val() const {
    NGRAPH_CHECK(m_element_type.is_integral_number());
    std::vector<int64_t> out_shape = cast_vector<int64_t>();
    Shape output_shape(shape_size(m_shape));
    std::transform(out_shape.begin(), out_shape.end(), output_shape.begin(), [&](const int64_t& v) {
        return (v > 0) ? v : 0;
    });
    return output_shape;
}

Strides op::Constant::get_strides_val() const {
    NGRAPH_CHECK(m_element_type == element::i64);
    std::vector<int64_t> out_strides = cast_vector<int64_t>();
    Strides output_strides(shape_size(m_shape));
    std::transform(out_strides.begin(), out_strides.end(), output_strides.begin(), [&](const int64_t& v) {
        return (v > 0) ? v : 0;
    });
    return output_strides;
}

Coordinate op::Constant::get_coordinate_val() const {
    NGRAPH_CHECK(m_element_type == element::i64);
    std::vector<int64_t> out_coordinate = cast_vector<int64_t>();
    Coordinate output_coordinate(shape_size(m_shape));
    std::transform(out_coordinate.begin(), out_coordinate.end(), output_coordinate.begin(), [&](const int64_t& v) {
        return (v > 0) ? v : 0;
    });
    return output_coordinate;
}

CoordinateDiff op::Constant::get_coordinate_diff_val() const {
    NGRAPH_CHECK(m_element_type == element::i64);
    std::vector<int64_t> out_coordinate_diff = cast_vector<int64_t>();
    CoordinateDiff output_coordinate_diff(shape_size(m_shape));
    std::transform(out_coordinate_diff.begin(),
                   out_coordinate_diff.end(),
                   output_coordinate_diff.begin(),
                   [&](const int64_t& v) {
                       return (v > 0) ? v : 0;
                   });
    return output_coordinate_diff;
}

AxisVector op::Constant::get_axis_vector_val() const {
    NGRAPH_CHECK(m_element_type.is_integral_number());
    std::vector<int64_t> out_axis_vector = cast_vector<int64_t>();
    AxisVector output_axis_vector(shape_size(m_shape));
    std::transform(out_axis_vector.begin(), out_axis_vector.end(), output_axis_vector.begin(), [&](const int64_t& v) {
        return (v > 0) ? v : 0;
    });
    return output_axis_vector;
}

AxisSet op::Constant::get_axis_set_val() const {
    NGRAPH_CHECK(m_element_type.is_integral_number());
    std::vector<int64_t> out_axis_set = cast_vector<int64_t>();
    AxisSet output_axis_set;
    for (auto& axis : out_axis_set) {
        output_axis_set.insert(axis > 0 ? axis : 0);
    }
    return output_axis_set;
}

void op::Constant::set_data_shape(const Shape& shape) {
    NGRAPH_CHECK(shape_size(shape) == shape_size(m_shape));
    m_shape = shape;
}

shared_ptr<Node> op::Constant::clone_with_new_inputs(const OutputVector& new_args) const {
    NGRAPH_OP_SCOPE(v0_Constant_clone_with_new_inputs);
    check_new_args_count(this, new_args);
    return make_shared<Constant>(*this);
}

template <typename T>
static bool test_bitwise_identical(const T* data, const size_t size) {
    bool data_is_constant = true;
    if (size > 0) {
        const T compare = data[0];
        for (size_t i = 1; i < size; i++) {
            if (data[i] != compare) {
                data_is_constant = false;
                break;
            }
        }
    }
    return data_is_constant;
}

bool op::Constant::are_all_data_elements_bitwise_identical() const {
    bool rc = false;
#if defined(__GNUC__) && !(__GNUC__ == 4 && __GNUC_MINOR__ == 8)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic error "-Wswitch"
#    pragma GCC diagnostic error "-Wswitch-enum"
#endif
    switch (m_element_type) {
    case element::Type_t::boolean:
    case element::Type_t::i8:
    case element::Type_t::u8: {
        rc = test_bitwise_identical<uint8_t>(get_data_ptr<uint8_t>(), shape_size(m_shape));
        break;
    }
    case element::Type_t::bf16:
    case element::Type_t::f16:
    case element::Type_t::i16:
    case element::Type_t::u16: {
        rc = test_bitwise_identical<uint16_t>(get_data_ptr<uint16_t>(), shape_size(m_shape));
        break;
    }
    case element::Type_t::f32:
    case element::Type_t::i32:
    case element::Type_t::u32: {
        rc = test_bitwise_identical<uint32_t>(get_data_ptr<uint32_t>(), shape_size(m_shape));
        break;
    }
    case element::Type_t::f64:
    case element::Type_t::i64:
    case element::Type_t::u64: {
        rc = test_bitwise_identical<uint64_t>(get_data_ptr<uint64_t>(), shape_size(m_shape));
        break;
    }
    case element::Type_t::i4:
    case element::Type_t::u1:
    case element::Type_t::u4:
    case element::Type_t::undefined:
    case element::Type_t::dynamic:
        break;
    }
#if defined(__GNUC__) && !(__GNUC__ == 4 && __GNUC_MINOR__ == 8)
#    pragma GCC diagnostic pop
#endif
    return rc;
}

bool op::v0::Constant::visit_attributes(AttributeVisitor& visitor) {
    NGRAPH_OP_SCOPE(v0_Constant_visit_attributes);
    Shape prev_shape = m_shape;
    element::Type prev_type = m_element_type;
    visitor.on_attribute("element_type", m_element_type);
    visitor.on_attribute("shape", m_shape);

    bool need_to_reallocate = (m_shape != prev_shape || prev_type != m_element_type);
    if (m_alloc_buffer_on_visit_attributes && need_to_reallocate) {
        // Filling in a fresh constant
        allocate_buffer();
    }
    visitor.on_attribute("value", m_data);
    m_all_elements_bitwise_identical = are_all_data_elements_bitwise_identical();
    return true;
}

bool op::v0::Constant::evaluate(const HostTensorVector& outputs, const HostTensorVector& inputs) const {
    NGRAPH_OP_SCOPE(v0_Constant_evaluate);
    auto output = outputs[0];
    output->write(get_data_ptr(), output->get_size_in_bytes());
    return true;
}

bool op::v0::Constant::has_evaluate() const {
    NGRAPH_OP_SCOPE(v0_Constant_has_evaluate);
    return true;
}

bool op::v0::Constant::evaluate_lower(const HostTensorVector& outputs) const {
    return evaluate(outputs, {});
}
bool op::v0::Constant::evaluate_upper(const HostTensorVector& outputs) const {
    return evaluate(outputs, {});
}
