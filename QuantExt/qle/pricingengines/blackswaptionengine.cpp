/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/* This file is supposed to be part of the QuantLib library eventually,
   in the meantime we provide is as part of the QuantExt library. */

/*
 Copyright (C) 2007, 2008 Ferdinando Ametrano
 Copyright (C) 2006 Cristina Duminuco
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb
 Copyright (C) 2006, 2007 StatPro Italia srl
 Copyright (C) 2015 Peter Caspers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <qle/pricingengines/blackswaptionengine.hpp>

namespace QuantExt {

    BlackSwaptionEngine::BlackSwaptionEngine(const Handle<YieldTermStructure> &discountCurve,
                        Volatility vol, const DayCounter &dc,
                        Real displacement)
        : BlackStyleSwaptionEngine<detail::Black76Spec>(discountCurve, vol, dc,
                                                displacement) {}
    BlackSwaptionEngine::BlackSwaptionEngine(const Handle<YieldTermStructure> &discountCurve,
                        const Handle<Quote> &vol,
                        const DayCounter &dc,
                        Real displacement)
        : BlackStyleSwaptionEngine<detail::Black76Spec>(discountCurve, vol, dc,
                                                displacement) {}
    BlackSwaptionEngine::BlackSwaptionEngine(const Handle<YieldTermStructure> &discountCurve,
                        const Handle<SwaptionVolatilityStructure> &vol,
                        Real displacement)
        : BlackStyleSwaptionEngine<detail::Black76Spec>(discountCurve, vol,
                                                displacement) {}

    BachelierBlackSwaptionEngine::BachelierBlackSwaptionEngine(
        const Handle<YieldTermStructure> &discountCurve, Volatility vol,
        const DayCounter &dc)
        : BlackStyleSwaptionEngine<detail::BachelierSpec>(discountCurve, vol,
                                                          dc, 0.0) {}
    BachelierBlackSwaptionEngine::BachelierBlackSwaptionEngine(
        const Handle<YieldTermStructure> &discountCurve,
        const Handle<Quote> &vol, const DayCounter &dc)
        : BlackStyleSwaptionEngine<detail::BachelierSpec>(discountCurve, vol,
                                                          dc, 0.0) {}
    BachelierBlackSwaptionEngine::BachelierBlackSwaptionEngine(
        const Handle<YieldTermStructure> &discountCurve,
        const Handle<SwaptionVolatilityStructure> &vol)
        : BlackStyleSwaptionEngine<detail::BachelierSpec>(discountCurve, vol,
                                                          0.0) {}
} // namespace QuantExt
