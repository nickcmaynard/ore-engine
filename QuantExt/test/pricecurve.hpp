/*
 Copyright (C) 2018 Quaternion Risk Management Ltd
 All rights reserved.

 This file is part of ORE, a free-software/open-source library
 for transparent pricing and risk analysis - http://opensourcerisk.org

 ORE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.
 The license is also available online at <http://opensourcerisk.org>

 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

/*! \file test/pricecurve.hpp
    \brief PriceCurve tests
    \ingroup tests
*/

#ifndef quantext_test_pricecurve_hpp
#define quantext_test_pricecurve_hpp

#include <boost/test/unit_test.hpp>

namespace testsuite {

//! PriceCurve tests
/*! \ingroup tests
*/
class PriceCurveTest {
public:
    static void testTimesAndPricesCurve();
    static void testTimesAndQuotesCurve();
    static void testDatesAndPricesCurve();
    static void testDatesAndQuotesCurve();
    static void testNoTimeZeroWorks();
    static void testNegativeTimeRequestThrows();
    static void testNegativePriceThrows();

    static boost::unit_test_framework::test_suite* suite();
};
}

#endif
