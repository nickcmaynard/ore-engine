/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/* This file is supposed to be part of the QuantLib library eventually,
   in the meantime we provide is as part of the QuantExt library. */

/*
 Copyright (C) 2008 Ferdinando Ametrano
 Copyright (C) 2004, 2005, 2007 StatPro Italia srl
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

/*
  Copyright (C) 2016 Quaternion Risk Management Ltd.
  All rights reserved.
*/

/*! \file constantoptionletvol.hpp
    \brief Constant caplet/floorlet volatility
*/

#ifndef quantext_caplet_constant_volatility_hpp
#define quantext_caplet_constant_volatility_hpp

#include <qle/termstructures/optionletvolatilitystructure.hpp>

namespace QuantLib {
    class Quote;
}

namespace QuantExt {

    //! Constant caplet volatility, no time-strike dependence
    class ConstantOptionletVolatility : public OptionletVolatilityStructure {
      public:
        //! floating reference date, floating market data
        ConstantOptionletVolatility(Natural settlementDays,
                                    const Calendar& cal,
                                    BusinessDayConvention bdc,
                                    const Handle<Quote>& volatility,
                                    const DayCounter& dc,
                                    VolatilityType type = ShiftedLognormal,
                                    Real displacement = 0.0);
        //! fixed reference date, floating market data
        ConstantOptionletVolatility(const Date& referenceDate,
                                    const Calendar& cal,
                                    BusinessDayConvention bdc,
                                    const Handle<Quote>& volatility,
                                    const DayCounter& dc,
                                    VolatilityType type = ShiftedLognormal,
                                    Real displacement = 0.0);
        //! floating reference date, fixed market data
        ConstantOptionletVolatility(Natural settlementDays,
                                    const Calendar& cal,
                                    BusinessDayConvention bdc,
                                    Volatility volatility,
                                    const DayCounter& dc,
                                    VolatilityType type = ShiftedLognormal,
                                    Real displacement = 0.0);
        //! fixed reference date, fixed market data
        ConstantOptionletVolatility(const Date& referenceDate,
                                    const Calendar& cal,
                                    BusinessDayConvention bdc,
                                    Volatility volatility,
                                    const DayCounter& dc,
                                    VolatilityType type = ShiftedLognormal,
                                    Real displacement = 0.0);
        //! \name TermStructure interface
        //@{
        Date maxDate() const;
        //@}
        //! \name VolatilityTermStructure interface
        //@{
        Real minStrike() const;
        Real maxStrike() const;
        //@}
        const VolatilityType volatilityType() const;
        const Real displacement() const;
      protected:
        boost::shared_ptr<SmileSection> smileSectionImpl(const Date& d) const;
        boost::shared_ptr<SmileSection> smileSectionImpl(Time) const;
        Volatility volatilityImpl(Time,
                                  Rate) const;
      private:
        Handle<Quote> volatility_;
        VolatilityType type_;
        Real displacement_;
    };


    // inline definitions

    inline Date ConstantOptionletVolatility::maxDate() const {
        return Date::maxDate();
    }

    inline Real ConstantOptionletVolatility::minStrike() const {
        return QL_MIN_REAL;
    }

    inline Real ConstantOptionletVolatility::maxStrike() const {
        return QL_MAX_REAL;
    }

    inline const VolatilityType ConstantOptionletVolatility::volatilityType() const {
        return type_;
    }

    inline const Real ConstantOptionletVolatility::displacement() const {
        return displacement_;
    }

}

#endif
