#ifndef LTCSTATICWAVETABLES_H
#define LTCSTATICWAVETABLES_H

#include <vector>
#include <type_traits>
#include <array>

#include "intfloatconversions.h"

template <typename Internal, typename Output>
class LTCStaticWavetable
{

public:

    /* 23.98 fps LTC
       48KHz
       Zero bit
       Positive polarity */
    static constexpr const std::array<Internal, 26> _zeroBitPositive2398{
            0.15904326870931959070354367,
            0.51150303643996608560229333,
            0.69801602991355182759036779,
            0.72266224256545108506344377,
            0.66331124615104763986295211,
            0.61378990072996508775560187,
            0.62484825566732982160544907,
            0.68254214620894382292703995,
            0.73400194034094623507513688,
            0.73679736921312599662314824,
            0.69338313038020382705894917,
            0.64610408490494264377446143,
            0.63954533275624736443631946,
            0.68189127283247175093094938,
            0.73694813141512149901046769,
            0.75394656281378458206887672,
            0.71194921570472557981190675,
            0.64282801797337196791914948,
            0.61164902954873434648419561,
            0.66546840273007235033730922,
            0.78724501950417791196201733,
            0.89112530507709952765083017,
            0.86780880151251882370644353,
            0.65303240954171959398166791,
            0.27394134942699793766607286,
            0.00000000000000000000000000
        };

    /* 23.98 fps LTC
       48KHz
       Zero bit
       Negative polarity */
    static constexpr const std::array<Internal, 26> _zeroBitNegative2398{
            -0.15904326870931820292476289,
            -0.51150303643996397617854655,
            -0.69801602991355105043425056,
            -0.72266224256545263937567825,
            -0.66331124615104974928669890,
            -0.61378990072996586491171911,
            -0.62484825566732793422630721,
            -0.68254214620894049225796607,
            -0.73400194034094445871829748,
            -0.73679736921312788400229010,
            -0.69338313038020782386183782,
            -0.64610408490494541933202299,
            -0.63954533275624569910178252,
            -0.68189127283246631083812872,
            -0.73694813141511728016297411,
            -0.75394656281378558126959888,
            -0.71194921570473279626156682,
            -0.64282801797337874027959970,
            -0.61164902954873390239498576,
            -0.66546840273006213628548267,
            -0.78724501950416370110730213,
            -0.89112530507709308835728734,
            -0.86780880151253070309280702,
            -0.65303240954175290067240667,
            -0.27394134942704412294389726,
            0.00000000000000000000000000
        };

    /* 23.98 fps LTC
       48KHz
       One bit
       Positive polarity */
    static constexpr const std::array<Internal, 26> _oneBitPositive2398{
            0.28018013027145860505129349,
            0.63143836456859381289774547,
            0.59024055457269808400155853,
            0.66084106073033643813374738,
            0.73130408599140417358341892,
            0.64641583979785621760782988,
            0.67218283298308634243767301,
            0.76074625161267006578924565,
            0.64929115762830635905089594,
            0.63685616187644444874393912,
            0.91961520411869046576214259,
            0.85041194516994844665447317,
            0.11173189163735385009967871,
            -0.53656004584360894593686453,
            -0.61987576772598318441964693,
            -0.60348969429607624004319177,
            -0.71819687858878000774609518,
            -0.69490905543035719205136047,
            -0.63335000593435386573304413,
            -0.73253579767914800413564080,
            -0.72547045787455333165638649,
            -0.59890652979311898018721649,
            -0.76780964239236648705144717,
            -0.97715190092042036873465349,
            -0.53299970448011546597655297,
            0.00000000000000000000000000
        };

    /* 23.98 fps LTC
       48KHz
       One bit
       Negative polarity */
    static constexpr const std::array<Internal, 26> _oneBitNegative2398{
            -0.28018013027145854954014226,
            -0.63143836456859359085314054,
            -0.59024055457269797297925606,
            -0.66084106073033643813374738,
            -0.73130408599140483971723370,
            -0.64641583979785732783085450,
            -0.67218283298308412199162376,
            -0.76074625161266962170003580,
            -0.64929115762831068892069197,
            -0.63685616187643789842809383,
            -0.91961520411867869739808157,
            -0.85041194516997065111496568,
            -0.11173189163740208929009867,
            0.53656004584358896192242128,
            0.61987576772598762531174543,
            0.60348969429607079995037111,
            0.71819687858877512276478683,
            0.69490905543036229907727375,
            0.63335000593435231142080966,
            0.73253579767914389631044969,
            0.72547045787455788357078745,
            0.59890652979311898018721649,
            0.76780964239236038082481173,
            0.97715190092042181202458551,
            0.53299970448012090606937363,
            0.00000000000000000000000000
        };

    /* 24 fps LTC
       48KHz
       Zero bit
       Positive polarity */
    static constexpr const std::array<Internal, 25> _zeroBitPositive24 {
            0.15904326870931959070354367,
            0.51150303643996608560229333,
            0.69801602991355182759036779,
            0.72266224256545108506344377,
            0.66331124615104763986295211,
            0.61378990072996508775560187,
            0.62484825566732982160544907,
            0.68254214620894382292703995,
            0.73400194034094623507513688,
            0.73679736921312599662314824,
            0.69338313038020382705894917,
            0.64610408490494264377446143,
            0.63954533275624736443631946,
            0.68189127283247175093094938,
            0.73694813141512149901046769,
            0.75394656281378458206887672,
            0.71194921570472557981190675,
            0.64282801797337196791914948,
            0.61164902954873434648419561,
            0.66546840273007235033730922,
            0.78724501950417791196201733,
            0.89112530507709952765083017,
            0.86780880151251882370644353,
            0.65303240954171959398166791,
            0.27394134942699793766607286
        };

    /* 24 fps LTC
       48KHz
       Zero bit
       Negative polarity */
    static constexpr const std::array<Internal, 25> _zeroBitNegative24 {
            -0.15904326870931820292476289,
            -0.51150303643996397617854655,
            -0.69801602991355105043425056,
            -0.72266224256545263937567825,
            -0.66331124615104974928669890,
            -0.61378990072996586491171911,
            -0.62484825566732793422630721,
            -0.68254214620894049225796607,
            -0.73400194034094445871829748,
            -0.73679736921312788400229010,
            -0.69338313038020782386183782,
            -0.64610408490494541933202299,
            -0.63954533275624569910178252,
            -0.68189127283246631083812872,
            -0.73694813141511728016297411,
            -0.75394656281378558126959888,
            -0.71194921570473279626156682,
            -0.64282801797337874027959970,
            -0.61164902954873390239498576,
            -0.66546840273006213628548267,
            -0.78724501950416370110730213,
            -0.89112530507709308835728734,
            -0.86780880151253070309280702,
            -0.65303240954175290067240667,
            -0.27394134942704412294389726
        };

    /* 24 fps LTC
       48KHz
       One bit
       Positive polarity */
    static constexpr const std::array<Internal, 25> _oneBitPositive24 {
            0.28018013027145860505129349,
            0.63143836456859381289774547,
            0.59024055457269808400155853,
            0.66084106073033643813374738,
            0.73130408599140417358341892,
            0.64641583979785621760782988,
            0.67218283298308634243767301,
            0.76074625161267006578924565,
            0.64929115762830635905089594,
            0.63685616187644444874393912,
            0.91961520411869046576214259,
            0.85041194516994844665447317,
            0.11173189163735385009967871,
            -0.53656004584360894593686453,
            -0.61987576772598318441964693,
            -0.60348969429607624004319177,
            -0.71819687858878000774609518,
            -0.69490905543035719205136047,
            -0.63335000593435386573304413,
            -0.73253579767914800413564080,
            -0.72547045787455333165638649,
            -0.59890652979311898018721649,
            -0.76780964239236648705144717,
            -0.97715190092042036873465349,
            -0.53299970448011546597655297
        };

    /* 24 fps LTC
       48KHz
       One bit
       Negative polarity */
    static constexpr const std::array<Internal, 25> _oneBitNegative24 {
            -0.28018013027145854954014226,
            -0.63143836456859359085314054,
            -0.59024055457269797297925606,
            -0.66084106073033643813374738,
            -0.73130408599140483971723370,
            -0.64641583979785732783085450,
            -0.67218283298308412199162376,
            -0.76074625161266962170003580,
            -0.64929115762831068892069197,
            -0.63685616187643789842809383,
            -0.91961520411867869739808157,
            -0.85041194516997065111496568,
            -0.11173189163740208929009867,
            0.53656004584358896192242128,
            0.61987576772598762531174543,
            0.60348969429607079995037111,
            0.71819687858877512276478683,
            0.69490905543036229907727375,
            0.63335000593435231142080966,
            0.73253579767914389631044969,
            0.72547045787455788357078745,
            0.59890652979311898018721649,
            0.76780964239236038082481173,
            0.97715190092042181202458551,
            0.53299970448012090606937363
        };

    /* 25 fps LTC
       48KHz
       Zero bit
       Positive polarity */
    static constexpr const std::array<Internal, 24> _zeroBitPositive25 {
            0.16507733897060011818425096,
            0.52512505376583384908428798,
            0.70335943077285323354885804,
            0.71329479422418007317219235,
            0.64860000475395274133205703,
            0.60980730658035375846282022,
            0.63817090683812360829563204,
            0.70243724266768825525986131,
            0.74134095912383601856276982,
            0.72248796578792695566306747,
            0.66916218569830387963293106,
            0.63599843349520757129766935,
            0.65756039263978771636232068,
            0.71566819093771794069169800,
            0.75460152005053193757788677,
            0.73212002393544195477659287,
            0.66251870364324971784952822,
            0.61219204042897090722874509,
            0.64605195388465919137388482,
            0.76481113471537298664770788,
            0.88472294735737067039593740,
            0.88195159698391545344264841,
            0.67469670327640829388116117,
            0.28513329725166497441435354
        };

    /* 25 fps LTC
       48KHz
       Zero bit
       Negative polarity */
    static constexpr const std::array<Internal, 24> _zeroBitNegative25 {
            -0.16507733897060009042867534,
            -0.52512505376583540339652245,
            -0.70335943077285367763806789,
            -0.71329479422417851885995788,
            -0.64860000475395085395291517,
            -0.60980730658035320335130791,
            -0.63817090683812627283089114,
            -0.70243724266769147490663272,
            -0.74134095912383646265197967,
            -0.72248796578792462419471576,
            -0.66916218569829999385234487,
            -0.63599843349520723823076196,
            -0.65756039263979171316520933,
            -0.71566819093772238158379650,
            -0.75460152005053360291242370,
            -0.73212002393543629263916728,
            -0.66251870364324205731065831,
            -0.61219204042896924189420815,
            -0.64605195388466685191275474,
            -0.76481113471538664239091077,
            -0.88472294735737833093480731,
            -0.88195159698390590552463664,
            -0.67469670327637831785949629,
            -0.28513329725162184224984685
        };

    /* 25 fps LTC
       48KHz
       One bit
       Positive polarity */
    static constexpr const std::array<Internal, 24> _oneBitPositive25 {
            0.28766722692857665810706180,
            0.62603920267963830692536931,
            0.58568176090340517969679013,
            0.68057005520296642853139701,
            0.72041276495999462259334223,
            0.63299744600516250070398883,
            0.70740753182428184331342891,
            0.74514029646427537567632271,
            0.60442502140561105150595722,
            0.73865150215176211823120411,
            0.98481681767571926933158011,
            0.55347204631856494749797548,
            -0.28766722692857027432467021,
            -0.62603920267963819590306684,
            -0.58568176090340517969679013,
            -0.68057005520296509626376746,
            -0.72041276495999462259334223,
            -0.63299744600516238968168636,
            -0.70740753182428284251415107,
            -0.74514029646427382136408823,
            -0.60442502140560971923832767,
            -0.73865150215177188819382081,
            -0.98481681767572026853230227,
            -0.55347204631853208489644658
        };

    /* 25 fps LTC
       48KHz
       One bit
       Negative polarity */
    static constexpr const std::array<Internal, 24> _oneBitNegative25 {
            -0.28766722692857615850670072,
            -0.62603920267963841794767177,
            -0.58568176090340529071909259,
            -0.68057005520296631750909455,
            -0.72041276495999440054873730,
            -0.63299744600516216763708144,
            -0.70740753182428406375947816,
            -0.74514029646427304420797100,
            -0.60442502140560860901530305,
            -0.73865150215177366455066021,
            -0.98481681767572026853230227,
            -0.55347204631852708889283576,
            0.28766722692860680066218038,
            0.62603920267963963919299886,
            0.58568176090340529071909259,
            0.68057005520297231271342753,
            0.72041276495999151396887328,
            0.63299744600516127945866174,
            0.70740753182428750545085450,
            0.74514029646427104580652667,
            0.60442502140560783185918581,
            0.73865150215177644010822178,
            0.98481681767571949137618503,
            0.55347204631852942036118748
        };

    /* 29.97 fps LTC
       60KHz
       Zero bit
       Positive polarity */
    static constexpr const std::array<Internal, 26> _zeroBitPositive2997 {
            0.15904326870931959070354367,
            0.51150303643996608560229333,
            0.69801602991355182759036779,
            0.72266224256545108506344377,
            0.66331124615104763986295211,
            0.61378990072996508775560187,
            0.62484825566732982160544907,
            0.68254214620894382292703995,
            0.73400194034094623507513688,
            0.73679736921312599662314824,
            0.69338313038020382705894917,
            0.64610408490494264377446143,
            0.63954533275624736443631946,
            0.68189127283247175093094938,
            0.73694813141512149901046769,
            0.75394656281378458206887672,
            0.71194921570472557981190675,
            0.64282801797337196791914948,
            0.61164902954873434648419561,
            0.66546840273007235033730922,
            0.78724501950417791196201733,
            0.89112530507709952765083017,
            0.86780880151251882370644353,
            0.65303240954171959398166791,
            0.27394134942699793766607286,
            0.00000000000000000000000000
        };

    /* 29.97 fps LTC
       60KHz
       Zero bit
       Negative polarity */
    static constexpr const std::array<Internal, 26> _zeroBitNegative2997 {
            -0.15904326870931820292476289,
            -0.51150303643996397617854655,
            -0.69801602991355105043425056,
            -0.72266224256545263937567825,
            -0.66331124615104974928669890,
            -0.61378990072996586491171911,
            -0.62484825566732793422630721,
            -0.68254214620894049225796607,
            -0.73400194034094445871829748,
            -0.73679736921312788400229010,
            -0.69338313038020782386183782,
            -0.64610408490494541933202299,
            -0.63954533275624569910178252,
            -0.68189127283246631083812872,
            -0.73694813141511728016297411,
            -0.75394656281378558126959888,
            -0.71194921570473279626156682,
            -0.64282801797337874027959970,
            -0.61164902954873390239498576,
            -0.66546840273006213628548267,
            -0.78724501950416370110730213,
            -0.89112530507709308835728734,
            -0.86780880151253070309280702,
            -0.65303240954175290067240667,
            -0.27394134942704412294389726,
            0.00000000000000000000000000
        };

    /* 29.97 fps LTC
       60KHz
       One bit
       Positive polarity */
    static constexpr const std::array<Internal, 26> _oneBitPositive2997 {
            0.28018013027145860505129349,
            0.63143836456859381289774547,
            0.59024055457269808400155853,
            0.66084106073033643813374738,
            0.73130408599140417358341892,
            0.64641583979785621760782988,
            0.67218283298308634243767301,
            0.76074625161267006578924565,
            0.64929115762830635905089594,
            0.63685616187644444874393912,
            0.91961520411869046576214259,
            0.85041194516994844665447317,
            0.11173189163735385009967871,
            -0.53656004584360894593686453,
            -0.61987576772598318441964693,
            -0.60348969429607624004319177,
            -0.71819687858878000774609518,
            -0.69490905543035719205136047,
            -0.63335000593435386573304413,
            -0.73253579767914800413564080,
            -0.72547045787455333165638649,
            -0.59890652979311898018721649,
            -0.76780964239236648705144717,
            -0.97715190092042036873465349,
            -0.53299970448011546597655297,
            0.00000000000000000000000000
        };

    /* 29.97 fps LTC
       60KHz
       One bit
       Negative polarity */
    static constexpr const std::array<Internal, 26> _oneBitNegative2997 {
            -0.28018013027145854954014226,
            -0.63143836456859359085314054,
            -0.59024055457269797297925606,
            -0.66084106073033643813374738,
            -0.73130408599140483971723370,
            -0.64641583979785732783085450,
            -0.67218283298308412199162376,
            -0.76074625161266962170003580,
            -0.64929115762831068892069197,
            -0.63685616187643789842809383,
            -0.91961520411867869739808157,
            -0.85041194516997065111496568,
            -0.11173189163740208929009867,
            0.53656004584358896192242128,
            0.61987576772598762531174543,
            0.60348969429607079995037111,
            0.71819687858877512276478683,
            0.69490905543036229907727375,
            0.63335000593435231142080966,
            0.73253579767914389631044969,
            0.72547045787455788357078745,
            0.59890652979311898018721649,
            0.76780964239236038082481173,
            0.97715190092042181202458551,
            0.53299970448012090606937363,
            0.00000000000000000000000000
        };

    /* 30 fps LTC
       60KHz
       Zero bit
       Positive polarity */
    static constexpr const std::array<Internal, 25> _zeroBitPositive30 {
            0.15904326870931959070354367,
            0.51150303643996608560229333,
            0.69801602991355182759036779,
            0.72266224256545108506344377,
            0.66331124615104763986295211,
            0.61378990072996508775560187,
            0.62484825566732982160544907,
            0.68254214620894382292703995,
            0.73400194034094623507513688,
            0.73679736921312599662314824,
            0.69338313038020382705894917,
            0.64610408490494264377446143,
            0.63954533275624736443631946,
            0.68189127283247175093094938,
            0.73694813141512149901046769,
            0.75394656281378458206887672,
            0.71194921570472557981190675,
            0.64282801797337196791914948,
            0.61164902954873434648419561,
            0.66546840273007235033730922,
            0.78724501950417791196201733,
            0.89112530507709952765083017,
            0.86780880151251882370644353,
            0.65303240954171959398166791,
            0.27394134942699793766607286
        };

    /* 30 fps LTC
       60KHz
       Zero bit
       Negative polarity */
    static constexpr const std::array<Internal, 25> _zeroBitNegative30 {
            -0.15904326870931820292476289,
            -0.51150303643996397617854655,
            -0.69801602991355105043425056,
            -0.72266224256545263937567825,
            -0.66331124615104974928669890,
            -0.61378990072996586491171911,
            -0.62484825566732793422630721,
            -0.68254214620894049225796607,
            -0.73400194034094445871829748,
            -0.73679736921312788400229010,
            -0.69338313038020782386183782,
            -0.64610408490494541933202299,
            -0.63954533275624569910178252,
            -0.68189127283246631083812872,
            -0.73694813141511728016297411,
            -0.75394656281378558126959888,
            -0.71194921570473279626156682,
            -0.64282801797337874027959970,
            -0.61164902954873390239498576,
            -0.66546840273006213628548267,
            -0.78724501950416370110730213,
            -0.89112530507709308835728734,
            -0.86780880151253070309280702,
            -0.65303240954175290067240667,
            -0.27394134942704412294389726
        };

    /* 30 fps LTC
       60KHz
       One bit
       Positive polarity */
    static constexpr const std::array<Internal, 25> _oneBitPositive30 {
            0.28018013027145860505129349,
            0.63143836456859381289774547,
            0.59024055457269808400155853,
            0.66084106073033643813374738,
            0.73130408599140417358341892,
            0.64641583979785621760782988,
            0.67218283298308634243767301,
            0.76074625161267006578924565,
            0.64929115762830635905089594,
            0.63685616187644444874393912,
            0.91961520411869046576214259,
            0.85041194516994844665447317,
            0.11173189163735385009967871,
            -0.53656004584360894593686453,
            -0.61987576772598318441964693,
            -0.60348969429607624004319177,
            -0.71819687858878000774609518,
            -0.69490905543035719205136047,
            -0.63335000593435386573304413,
            -0.73253579767914800413564080,
            -0.72547045787455333165638649,
            -0.59890652979311898018721649,
            -0.76780964239236648705144717,
            -0.97715190092042036873465349,
            -0.53299970448011546597655297
        };

    /* 30 fps LTC
       60KHz
       One bit
       Negative polarity */
    static constexpr const std::array<Internal, 25> _oneBitNegative30 {
            -0.28018013027145854954014226,
            -0.63143836456859359085314054,
            -0.59024055457269797297925606,
            -0.66084106073033643813374738,
            -0.73130408599140483971723370,
            -0.64641583979785732783085450,
            -0.67218283298308412199162376,
            -0.76074625161266962170003580,
            -0.64929115762831068892069197,
            -0.63685616187643789842809383,
            -0.91961520411867869739808157,
            -0.85041194516997065111496568,
            -0.11173189163740208929009867,
            0.53656004584358896192242128,
            0.61987576772598762531174543,
            0.60348969429607079995037111,
            0.71819687858877512276478683,
            0.69490905543036229907727375,
            0.63335000593435231142080966,
            0.73253579767914389631044969,
            0.72547045787455788357078745,
            0.59890652979311898018721649,
            0.76780964239236038082481173,
            0.97715190092042181202458551,
            0.53299970448012090606937363
        };

protected:

    template <int I>
    inline constexpr static std::array<Output, I> _convert(const std::array<Internal, I> values)
    {
        std::array<Output, I> converted;
        for (size_t i(0); i < I; ++i)
        {
            converted[i] = ((
                    std::is_floating_point<Output>::value
                    ? static_cast<Output>(values[i])
                    : float_to_int<Internal, Output>(values[i])
                ));
        }
        return converted;
    }

public:

    inline constexpr static const std::array<Output, 24>
        zeroBitPositive25 = _convert<24>(_zeroBitPositive25),
        zeroBitNegative25 = _convert<24>(_zeroBitNegative25),
        oneBitPositive25 = _convert<24>(_oneBitPositive25),
        oneBitNegative25 = _convert<24>(_oneBitNegative25);
    
    inline constexpr static const std::array<Output, 25>
        zeroBitPositive24 = _convert<25>(_zeroBitPositive24),
        zeroBitNegative24 = _convert<25>(_zeroBitNegative24),
        oneBitPositive24 = _convert<25>(_oneBitPositive24),
        oneBitNegative24 = _convert<25>(_oneBitNegative24),
        zeroBitPositive30 = _convert<25>(_zeroBitPositive30),
        zeroBitNegative30 = _convert<25>(_zeroBitNegative30),
        oneBitPositive30 = _convert<25>(_oneBitPositive30),
        oneBitNegative30 = _convert<25>(_oneBitNegative30);

    inline constexpr static const std::array<Output, 26>
        zeroBitPositive2398 = _convert<26>(_zeroBitPositive2398),
        zeroBitNegative2398 = _convert<26>(_zeroBitNegative2398),
        oneBitPositive2398 = _convert<26>(_oneBitPositive2398),
        oneBitNegative2398 = _convert<26>(_oneBitNegative2398),
        zeroBitPositive2997 = _convert<26>(_zeroBitPositive2997),
        zeroBitNegative2997 = _convert<26>(_zeroBitNegative2997),
        oneBitPositive2997 = _convert<26>(_oneBitPositive2997),
        oneBitNegative2997 = _convert<26>(_oneBitNegative2997);

    constexpr LTCStaticWavetable() {};
    constexpr ~LTCStaticWavetable() {};

    constexpr static int get_sample_rate(int fps)
    {
        /* Returns the sample rate used for specified frame rate */
        return (fps == 30) ? 60000 : 48000;
    }

    constexpr static int get_bit_length(int fps, bool slowdown)
    {
        /* Get length of bit in samples */
        return 24 + (fps != 25) + slowdown;
    }

    constexpr static std::vector<Output> get(int fps, bool slowdown, bool bit, bool polarity)
    {
        /* Returns a vector of samples for specified bit */
        if (fps == 25)
        {
            auto begin = (
                    bit
                    ? (polarity ? oneBitPositive25.begin() : oneBitNegative25.begin())
                    : (polarity ? zeroBitPositive25.begin() : zeroBitNegative25.begin())
                );
            auto end = (
                    bit
                    ? (polarity ? oneBitPositive25.end() : oneBitNegative25.end())
                    : (polarity ? zeroBitPositive25.end() : zeroBitNegative25.end())
                );
            return std::vector<Output>(begin, end);
        }
        else if (fps == 24)
        {
            auto begin = (
                    bit
                    ? (polarity ? oneBitPositive2398.begin() : oneBitNegative2398.begin())
                    : (polarity ? zeroBitPositive2398.begin() : zeroBitNegative2398.begin())
                );
            auto end = (
                    bit
                    ? (polarity ? oneBitPositive2398.end() : oneBitNegative2398.end())
                    : (polarity ? zeroBitPositive2398.end() : zeroBitNegative2398.end())
                );
            return std::vector<Output>(begin, slowdown ? end : --end);
        }
        auto begin = (
                bit
                ? (polarity ? oneBitPositive2997.begin() : oneBitNegative2997.begin())
                : (polarity ? zeroBitPositive2997.begin() : zeroBitNegative2997.begin())
            );
        auto end = (
                bit
                ? (polarity ? oneBitPositive2997.end() : oneBitNegative2997.end())
                : (polarity ? zeroBitPositive2997.end() : zeroBitNegative2997.end())
            );
        return std::vector<Output>(begin, slowdown ? end : --end);
    }

};

#endif