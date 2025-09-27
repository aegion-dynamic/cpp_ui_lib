#include "graphtype.h"
#include <vector>

QString graphTypeToString(GraphType type)
{
    switch (type) {
        case GraphType::BDW: return "BDW";
        case GraphType::BRW: return "BRW";
        case GraphType::BTW: return "BTW";
        case GraphType::FDW: return "FDW";
        case GraphType::FTW: return "FTW";
        case GraphType::LTW: return "LTW";
        case GraphType::RTW: return "RTW";
        default: return "UNKNOWN";
    }
}

std::vector<GraphType> getAllGraphTypes()
{
    return {
        GraphType::BDW,
        GraphType::BRW,
        GraphType::BTW,
        GraphType::FDW,
        GraphType::FTW,
        GraphType::LTW,
        GraphType::RTW
    };
}

std::vector<QString> getAllGraphTypeStrings()
{
    return {
        "FDW",
        "BDW",
        "BRW",
        "LTW",
        "BTW",
        "RTW",
        "FTW"
    };
}
