// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <cstdlib>
#include <random>
#include <string>

// clang-format off
#ifdef ${BACKEND_NAME}_FLOAT_TOLERANCE_BITS
#define DEFAULT_FLOAT_TOLERANCE_BITS ${BACKEND_NAME}_FLOAT_TOLERANCE_BITS
#endif

#ifdef ${BACKEND_NAME}_DOUBLE_TOLERANCE_BITS
#define DEFAULT_DOUBLE_TOLERANCE_BITS ${BACKEND_NAME}_DOUBLE_TOLERANCE_BITS
#endif
// clang-format on

#include "gtest/gtest.h"
#include "ngraph/ngraph.hpp"
#include "util/engine/test_engines.hpp"
#include "util/test_case.hpp"
#include "util/test_control.hpp"

using namespace std;
using namespace ngraph;

static string s_manifest = "${MANIFEST}";
using TestEngine = test::ENGINE_CLASS_NAME(${BACKEND_NAME});

NGRAPH_TEST(${BACKEND_NAME}, sinh) {
    Shape shape{8};
    auto A = make_shared<op::Parameter>(element::f32, shape);
    auto f = make_shared<Function>(make_shared<op::Sinh>(A), ParameterVector{A});

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<float>({-4, -3, -2, -1, 0, 1, 2, 3});
    test_case.add_expected_output<float>(
        shape,
        {sinhf(-4), sinhf(-3), sinhf(-2), sinhf(-1), sinhf(0), sinhf(1), sinhf(2), sinhf(3)});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, sinh_negative) {
    Shape shape{5};
    auto A = make_shared<op::Parameter>(element::f32, shape);
    auto f = make_shared<Function>(make_shared<op::Sinh>(A), ParameterVector{A});

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<float>({-4, -3, -2, -1, -5});
    test_case.add_expected_output<float>(shape, {sinhf(-4), sinhf(-3), sinhf(-2), sinhf(-1), sinhf(-5)});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, sinh_scalar) {
    Shape shape{};
    auto A = make_shared<op::Parameter>(element::f32, shape);
    auto f = make_shared<Function>(make_shared<op::Sinh>(A), ParameterVector{A});

    const vector<float> a{13};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<float>({a});
    test_case.add_expected_output<float>(shape, {sinhf(13)});
    test_case.run(DEFAULT_FLOAT_TOLERANCE_BITS + 2);
}

NGRAPH_TEST(${BACKEND_NAME}, sinh_in_place) {
    Shape shape{2};
    auto A = make_shared<op::Parameter>(element::f32, shape);
    auto T = make_shared<op::Sinh>(A);
    auto T2 = make_shared<op::Sinh>(T);

    auto f = make_shared<Function>(T2, ParameterVector{A});

    const vector<float> a{1, 3};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<float>({a});
    test_case.add_expected_output<float>(shape, {sinhf(sinhf(1)), sinhf(sinhf(3))});
    test_case.run(DEFAULT_FLOAT_TOLERANCE_BITS + 2);
}

NGRAPH_TEST(${BACKEND_NAME}, sinh_i32) {
    Shape shape{5};
    auto A = make_shared<op::Parameter>(element::i32, shape);
    ;
    auto f = make_shared<Function>(make_shared<op::Sinh>(A), ParameterVector{A});

    const vector<int32_t> input{2, 1, 0, -1, -2};
    const vector<int32_t> expected{4, 1, 0, -1, -4};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<int32_t>({input});
    test_case.add_expected_output<int32_t>(shape, {expected});
    test_case.run();
}
