/*
 Copyright (C) 2020 Quaternion Risk Management Ltd
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

#include <ored/portfolio/equityidentifier.hpp>

namespace ore {
namespace data {
    
void EquityIdentifier::setEquityName() {
    if (equityName_.empty()) {
        string name = identifierType_ + ":" + identifierName_;
        if (!currency_.empty())
            name = name + ":" + currency_;
        if (!exchange_.empty()) {
            if (currency_.empty())
                name = name + ":";
            name = name + ":" + exchange_;
        }
        equityName_ = name;
    }
}

void EquityIdentifier::fromXML(XMLNode* node) {
    // May get an explicit CreditCurveId node. If so, we use it. Otherwise, we must have ReferenceInformation node.
    XMLNode* tmp = XMLUtils::getChildNode(node, "Name");
    if (tmp) {
        equityName_ = XMLUtils::getNodeValue(tmp);
    } else {
        tmp = XMLUtils::getChildNode(node, "EquityIdentifier");
        QL_REQUIRE(tmp, "Need either a Name or EquityIdentifier node.");
        identifierType_ = XMLUtils::getChildValue(tmp, "IdentifierType", true);
        identifierName_ = XMLUtils::getChildValue(tmp, "IdentifierName", true);
        currency_ = XMLUtils::getChildValue(tmp, "Currency", false);
        exchange_ = XMLUtils::getChildValue(tmp, "Exchange", false);
    }
    setEquityName();
}

XMLNode* EquityIdentifier::toXML(XMLDocument& doc) {
    XMLNode* node;
    if (identifierName_.empty()) {
        node = doc.allocNode("Name", equityName_);
    } else {
        node = doc.allocNode("EquityIdentifier");
        QL_REQUIRE(node, "Failed to create trade node");
        XMLUtils::addChild(doc, node, "IdentifierType", identifierType_);
        XMLUtils::addChild(doc, node, "IdentifierName", identifierName_);
        if (!currency_.empty())
            XMLUtils::addChild(doc, node, "Currency", currency_);
        if (!exchange_.empty())
            XMLUtils::addChild(doc, node, "Exchange", exchange_);
    }
    return node;
}

} // namespace data
} // namespace ore