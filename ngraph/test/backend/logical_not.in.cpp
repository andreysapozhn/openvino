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

NGRAPH_SUPPRESS_DEPRECATED_START

using namespace std;
using namespace ngraph;

static string s_manifest = "${MANIFEST}";
using TestEngine = test::ENGINE_CLASS_NAME(${BACKEND_NAME});

NGRAPH_TEST(${BACKEND_NAME}, not) {
    Shape shape{2, 2};
    auto A = make_shared<op::Parameter>(element::boolean, shape);
    auto f = make_shared<Function>(make_shared<op::v1::LogicalNot>(A), ParameterVector{A});

    std::vector<char> a{1, 0, 1, 0};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<char>(shape, a);
    test_case.add_expected_output<char>(shape, {0, 1, 0, 1});
    test_case.run();
}

NGRAPH_TEST(${BACKEND_NAME}, not_i32) {
    Shape shape{2, 2};
    auto A = make_shared<op::Parameter>(element::i32, shape);
    auto f = make_shared<Function>(make_shared<op::v1::LogicalNot>(A), ParameterVector{A});

    std::vector<int32_t> a{1, 0, 2, 0};

    auto test_case = test::TestCase<TestEngine>(f);
    test_case.add_input<int32_t>(shape, a);
    test_case.add_expected_output<int32_t>(shape, {0, 1, 0, 1});
}
