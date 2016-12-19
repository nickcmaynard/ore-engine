/*
 Copyright (C) 2016 Quaternion Risk Management Ltd
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

/*! \file engine/sensitivityanalysis.hpp
    \brief Perfrom sensitivity analysis for a given portfolio
    \ingroup simulation
*/

#pragma once

#include <ored/portfolio/portfolio.hpp>
#include <ored/marketdata/market.hpp>
#include <orea/cube/npvcube.hpp>
#include <orea/scenario/scenariosimmarketparameters.hpp>
#include <orea/scenario/scenariosimmarket.hpp>
#include <orea/scenario/sensitivityscenariodata.hpp>
#include <orea/scenario/sensitivityscenariogenerator.hpp>

#include <map>
#include <set>
#include <tuple>

namespace ore {
namespace analytics {

//! Sensitivity Analysis
/*!
  This class wraps functionality to perform a sensitivity analysis for a given portfolio.
  It comprises
  - building the "simulation" market to which sensitivity scenarios are applied
  - building the portfolio linked to this simulation market
  - generating sensitivity scenarios
  - running the scenario "engine" to apply these and compute the NPV impacts of all required shifts
  - compile first and second order sensitivities for all factors and all trades
  - convert zero rate / caplet vol sensitivities into "par" rate / cap vol sensitivities
  - fill result structures that can be queried
  - write sensitivity report to a file

  \ingroup simulation
*/
class SensitivityAnalysis {
public:
    //! Constructor
    SensitivityAnalysis(boost::shared_ptr<ore::data::Portfolio>& portfolio,
                        boost::shared_ptr<ore::data::Market>& market, const string& marketConfiguration,
                        const boost::shared_ptr<ore::data::EngineData>& engineData,
                        boost::shared_ptr<ScenarioSimMarketParameters>& simMarketData,
                        const boost::shared_ptr<SensitivityScenarioData>& sensitivityData,
                        const Conventions& conventions);

    //! Return set of trades analysed
    const std::set<std::string>& trades() { return trades_; }

    //! Return unique set of factors shifted
    const std::set<std::string>& factors() { return factors_; }

    //! Return base NPV by trade, before shift
    const std::map<std::string, Real>& baseNPV() { return baseNPV_; }

    //! Return delta/vega (first order sensitivity times shift) by trade/factor pair
    const std::map<std::pair<std::string, std::string>, Real>& delta() { return delta_; }

    //! Return par delta (first order sensitivity times shift) by trade/factor pair
    const std::map<std::pair<std::string, std::string>, Real>& parDelta() { return parDelta_; }

    //! Return gamma (second order sensitivity times shift^2) by trade/factor pair
    const std::map<std::pair<std::string, std::string>, Real>& gamma() { return gamma_; }

    //! Return cross gamma (mixed second order sensitivity times shift^2) by trade/factor1/factor2
    const std::map<std::tuple<std::string, std::string, std::string>, Real>& crossGamma() { return crossGamma_; }

    //! Write "raw" NPV by trade/scenario to a file (contains base, up and down shift scenarios)
    void writeScenarioReport(string fileName, Real outputThreshold = 0.0);

    //! Write deltas and gammas by trade/factor pair to a file
    void writeSensitivityReport(string fileName, Real outputThreshold = 0.0);

    //! Write cross gammas by trade/factor1/factor2 to a file
    void writeCrossGammaReport(string fileName, Real outputThreshold = 0.0);

    //! Write sensitivity of par rates w.r.t. zero rates to a file
    void writeParRateSensitivityReport(string fileName);

private:
    //! Create Deposit for implying par rate sensitivity from zero rate sensitivity
    boost::shared_ptr<Instrument> makeDeposit(string ccy, string indexName, Period term,
                                              const boost::shared_ptr<ore::data::Market>& market,
                                              const boost::shared_ptr<Convention>& conventions, bool singleCurve);
    //! Create FRA for implying par rate sensitivity from zero rate sensitivity
    boost::shared_ptr<Instrument> makeFRA(string ccy, string indexName, Period term,
                                          const boost::shared_ptr<ore::data::Market>& market,
                                          const boost::shared_ptr<Convention>& conventions, bool singleCurve);
    //! Create Swap for implying par rate sensitivity from zero rate sensitivity
    boost::shared_ptr<Instrument> makeSwap(string ccy, string indexName, Period term,
                                           const boost::shared_ptr<ore::data::Market>& market,
                                           const boost::shared_ptr<Convention>& conventions, bool singleCurve);
    //! Create OIS Swap for implying par rate sensitivity from zero rate sensitivity
    boost::shared_ptr<Instrument> makeOIS(string ccy, string indexName, Period term,
                                          const boost::shared_ptr<ore::data::Market>& market,
                                          const boost::shared_ptr<Convention>& conventions, bool singleCurve);
    //! Create Cap/Floor isntrument for implying flat vol sensitivity from optionlet vol sensitivity
    boost::shared_ptr<CapFloor> makeCapFloor(string ccy, string indexName, Period term, Real strike,
                                             const boost::shared_ptr<ore::data::Market>& market);

    // base NPV by trade
    std::map<std::string, Real> baseNPV_;
    // NPV respectively sensitivity by trade and factor
    std::map<std::pair<string, string>, Real> upNPV_, downNPV_, delta_, gamma_, parDelta_;
    // cross gamma by trade, factor1, factor2
    std::map<std::tuple<string, string, string>, Real> crossGamma_;
    // unique set of factors
    std::set<std::string> factors_;
    // unique set of trades
    std::set<std::string> trades_;
    // fair par rate by factor shift
    std::map<std::string, Real> fairParRate_;
    // map of factors to curve names
    std::map<std::string, std::string> factorToCurve_;
    // map of sensitivity of par rates w.r.t. zero rates by curve name (ccy, indexName) and factor
    map<std::pair<std::string, std::string>, std::vector<Real> > parRatesSensi_;
    // map of sensitivity of flat cap vols w.r.t. optionlet vols by curve name (ccy), strike index and factor
    map<std::tuple<std::string, Size, std::string>, std::vector<Real> > flatCapVolSensi_;
};

//! ParSensitivityConverter class
/*!
  1) Build Jacobi matrix containing sensitivities of par rates (first index) w.r.t. zero shifts (second index)
  2) Convert zero rate sensitivities into par rate sensitivities

  Let: p_ij denote curve i's par instrument j (curve may be a discount or an index curve)
           z_kl denote curve k's zero shift l (curve may be a discount or an index curve)
           i,k run from 0 to I
           j,l run from 0 to J

      Organisation of the matrix:
             z_00 z_01 z_02 ... z_0J ... z_10 z_11 ... z_1J ... z_I0 z_I1 ... z_IJ
      p_00
      p_01
      p_02
      ...
      p_0J
      p_10
      p_11
      ...
      p_1J
      ...
      p_I0
      p_I1
      ...
      p_IJ

      The curve numbering starts with discount curves (by ccy), followed by index curves (by index name).
      The number J of par instruments respectively zero shifts can differ between discount and index curves.
      The number of zero shifts matches the number of par instruments.
      The Jacobi matrix is therefore quadratic by construction.
 */
class ParSensitivityConverter {
public:
    ParSensitivityConverter(
        const boost::shared_ptr<SensitivityScenarioData>& sensitivityData, 
        //! Delta by trade and factor
        const std::map<std::pair<string, string>, Real>& delta,
        //! Par rate sensitivity w.r.t. zero shifts by factor and curve name (discount:ccy, index:ccy-name-tenor)
        const map<pair<string, string>, vector<Real> >& parRateSensi,
        const map<std::tuple<std::string, Size, std::string>, std::vector<Real> >& flatCapVolSensi)
        : sensitivityData_(sensitivityData), delta_(delta), parRateSensi_(parRateSensi),
          flatCapVolSensi_(flatCapVolSensi) {
        buildJacobiMatrix();
        convertSensitivity();
    }

    //! Inspectors
    //@{
    //! List of factor names as defined by the sensitivity scenario generator
    const vector<string>& factors(string curveId) { return factors_; }
    //! Yield curve names identified in the factors (ccy for discount curves, index name for index curves)
    const vector<string>& curves() { return curves_; }
    //@}

    //! Resulting Jacobi matrix
    const Matrix& jacobi() { return jacobi_; }
    //! Inverse of the Jacobi matrix
    const Matrix& jacobiInverse() { return jacobiInverse_; }
    //! Resulting par delta/vega data
    const std::map<std::pair<string, string>, Real>& parDelta() { return parDelta_; }

private:
    void buildJacobiMatrix();
    void convertSensitivity();

    boost::shared_ptr<SensitivityScenarioData> sensitivityData_;
    std::vector<string> factors_;
    std::map<std::pair<string, string>, Real> delta_;
    std::map<std::pair<string, string>, Real> parDelta_;
    std::map<std::pair<string, string>, std::vector<Real> > parRateSensi_;
    std::map<std::tuple<std::string, Size, std::string>, std::vector<Real> > flatCapVolSensi_;
    std::vector<string> curves_;
    Matrix jacobi_, jacobiInverse_;
};

//! Helper class for implying the fair flat cap/floor volatility
/*! This class is copied from QuantLib's capfloor.cpp and generalised to cover both normal and lognormal volatilities */
class ImpliedCapFloorVolHelper {
public:
    ImpliedCapFloorVolHelper(VolatilityType type, const CapFloor&, const Handle<YieldTermStructure>& discountCurve,
                             Real targetValue, Real displacement);
    Real operator()(Volatility x) const;
    Real derivative(Volatility x) const;

private:
    boost::shared_ptr<PricingEngine> engine_;
    Handle<YieldTermStructure> discountCurve_;
    Real targetValue_;
    boost::shared_ptr<SimpleQuote> vol_;
    const Instrument::results* results_;
};

//! Utility for implying a flat volatility which reproduces the provided Cap/Floor price
Volatility impliedVolatility(const CapFloor& cap, Real targetValue, const Handle<YieldTermStructure>& d,
                             Volatility guess, VolatilityType type = Normal, Real displacement = 0.0,
                             Real accuracy = 1.0e-6, Natural maxEvaluations = 100, Volatility minVol = 1.0e-7,
                             Volatility maxVol = 4.0);
}
}
