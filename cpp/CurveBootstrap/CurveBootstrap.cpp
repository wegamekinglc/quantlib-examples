#include <ql/quantlib.hpp>

using namespace QuantLib;

int main(int, char* []) {
    
    /*********************
    **   RFR CURVE    **
    *********************/

    auto today = Date(25, October, 2021);
    auto settlementDays = 2;
    Settings::instance().evaluationDate() = today;
    std::cout << "Today: " << today << std::endl;

    std::vector<ext::shared_ptr<RateHelper>> instruments;

    std::vector<std::tuple<Month, int, Frequency, ext::shared_ptr<Quote>>> futuresData  = {
        std::make_tuple(November, 2021, Monthly, ext::make_shared<SimpleQuote>(99.934)),
        std::make_tuple(December, 2021, Monthly, ext::make_shared<SimpleQuote>(99.922)),
        std::make_tuple(January, 2022, Monthly, ext::make_shared<SimpleQuote>(99.914)),
        std::make_tuple(February, 2022, Monthly, ext::make_shared<SimpleQuote>(99.919)),
        std::make_tuple(March, 2022, Quarterly, ext::make_shared<SimpleQuote>(99.876)),
        std::make_tuple(June, 2022, Quarterly, ext::make_shared<SimpleQuote>(99.799)),
        std::make_tuple(September, 2022, Quarterly, ext::make_shared<SimpleQuote>(99.626)),
        std::make_tuple(December, 2022, Quarterly, ext::make_shared<SimpleQuote>(99.443)),
    };

    for (const auto& q : futuresData) {
        auto month = std::get<0>(q);
        auto year = std::get<1>(q);
        auto frequency = std::get<2>(q);
        auto quote = std::get<3>(q);
        auto helper = ext::make_shared<SofrFutureRateHelper>(Handle<Quote>(quote), month, year, frequency);
        instruments.push_back(helper);
    }

    std::map<Period, ext::shared_ptr<Quote>> oisData = {
        {1 * Years, ext::make_shared<SimpleQuote>(0.00137)},
        {2 * Years, ext::make_shared<SimpleQuote>(0.00409)},
        {3 * Years, ext::make_shared<SimpleQuote>(0.00674)},
        {5 * Years, ext::make_shared<SimpleQuote>(0.01004)},
        {8 * Years, ext::make_shared<SimpleQuote>(0.01258)},
        {10 * Years, ext::make_shared<SimpleQuote>(0.01359)},
        {12 * Years, ext::make_shared<SimpleQuote>(0.01420)},
        {15 * Years, ext::make_shared<SimpleQuote>(0.01509)},
        {20 * Years, ext::make_shared<SimpleQuote>(0.01574)},
        {25 * Years, ext::make_shared<SimpleQuote>(0.01586)},
        {30 * Years, ext::make_shared<SimpleQuote>(0.01579)},
        {35 * Years, ext::make_shared<SimpleQuote>(0.01559)},
        {40 * Years, ext::make_shared<SimpleQuote>(0.01514)},
        {45 * Years, ext::make_shared<SimpleQuote>(0.01446)},
        {50 * Years, ext::make_shared<SimpleQuote>(0.01425)}
    };

    auto sofr = ext::make_shared<Sofr>();
    for (const auto& q : oisData) {
        auto tenor = q.first;
        auto quote = q.second;
        auto helper = ext::make_shared<OISRateHelper>(settlementDays, tenor, Handle<Quote>(quote), sofr);
        instruments.push_back(helper);
    }

    Calendar termStructureCalendar = UnitedStates(UnitedStates::GovernmentBond);
    DayCounter termStructureDayCounter = Actual360();
    auto rfrTermStructure = ext::make_shared<PiecewiseYieldCurve<Discount, Cubic>>(
        0,
        termStructureCalendar,
        instruments,
        termStructureDayCounter
    );

    rfrTermStructure->enableExtrapolation();

    /*********************
    **   RE-PRICE    **
    *********************/

    RelinkableHandle<YieldTermStructure> discountingTermStructure;
    discountingTermStructure.linkTo(rfrTermStructure);

    auto sofrIndex = ext::make_shared<Sofr>(discountingTermStructure);
    auto startDate = sofrIndex->fixingCalendar().advance(today, settlementDays * Days);
    auto maturityDate = sofrIndex->fixingCalendar().advance(startDate, 20 * Years, sofrIndex->businessDayConvention(), sofrIndex->endOfMonth());
    std::cout << "SOFR Index swap start date: " << startDate << std::endl;
    std::cout << "SOFR Index swap maturity date: " << maturityDate << std::endl;

    Schedule schedule(startDate,
                      maturityDate,
                      Period(Annual),
                      sofrIndex->fixingCalendar(),
                      sofrIndex->businessDayConvention(),
                      sofrIndex->businessDayConvention(),
                      DateGeneration::Forward,
                      sofrIndex->endOfMonth());

    

    Real nominal = 1000000.0;

    OvernightIndexedSwap swap(
        OvernightIndexedSwap::Payer,
        nominal,
        schedule,
        0.0,
        sofrIndex->dayCounter(),
        sofrIndex
    );

    swap.setPricingEngine(ext::make_shared<DiscountingSwapEngine>(discountingTermStructure));
    auto npv = swap.NPV();
    auto fairSpread = swap.fairSpread();
    auto fairRate = swap.fairRate();
    std::cout << "SOFR Index swap NPV: " << npv << std::endl;
    std::cout << "SOFR Index swap fair spread: " << io::rate(fairSpread) << std::endl;
    std::cout << "SOFR Index swap fair fixed rate: " << io::rate(fairRate) << std::endl;

    return 0;
}