// ----- LoadPerson.cpp -----
#include<vector>
#include<iostream>
#include <string>
#include<sstream>
#include <cstdint>

using namespace std;

struct DBConnectionParams {
    string_view name_;
    int connection_timeout_;
    bool allow_exceptions_;
    DBLogLevel log_level_;
};

struct SearchFilterParams {
    int min_age_;
    int max_age_;
    string_view name_;
};

struct Person {
    string name_;
    int age_;
};

DBHandler ConnectDB(const DBConnectionParams& connection_params) {
    DBConnector connector(connection_params.allow_exceptions_, connection_params.log_level_);

    if (connection_params.name_.starts_with("tmp."s)) { // cpp 20
        return connector.ConnectTmp(connection_params.name_, connection_params.connection_timeout_);
    }

    return connector.Connect(connection_params.name_, connection_params.connection_timeout_);
}

DBQuery CreateQuery(const SearchFilterParams &params, const DBHandler &db) {
    ostringstream query_str;
    query_str << "from Persons "s
              << "select Name, Age "s
              << "where Age between "s << params.min_age_ << " and "s << params.max_age_ << " "s
              << "and Name like '%"s << db.Quote(params.name_) << "%'"s;
    return DBQuery(query_str.str());
}

vector<Person> LoadPersons(const DBConnectionParams& connection_params, const SearchFilterParams& search_filter_params) {
    DBHandler db = ConnectDB(connection_params);

    if (!connection_params.allow_exceptions_ && !db.IsOK()) {
        return {};
    }

    DBQuery query = CreateQuery(search_filter_params, db);

    vector<Person> persons;
    for (auto [name, age]: db.LoadRows<string, int>(query)) {
        persons.push_back({move(name), age});
    }
    return persons;
}

// ----- CheckDateTimeValidity.cpp -----
#include <array>
#include <string>
#include <stdexcept>

using namespace std;

int CalculateDayMaxCountInMonth(const DateTime &dt) {
    const bool is_leap_year = (dt.year % 4 == 0) && !(dt.year % 100 == 0 && dt.year % 400 != 0);
    const array month_lengths = {31, 28 + is_leap_year, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return month_lengths[dt.month - 1];
}

void ValidityChecker(const string& dt_name, const DateTime &dt, int dt_min, int dt_max) {
    if (dt < dt_min) {
        throw domain_error(dt_name + " is too small"s);
    }

    if (dt_max < dt) {
        throw domain_error(dt_name + " is too big"s);
    }
}

void CheckDateTimeValidity(const DateTime &dt) {
    ValidityChecker("year"s, dt.year, 1, 9999);
    ValidityChecker("month"s, dt.month, 1, 12);
    ValidityChecker("day"s, dt.day, 1, CalculateDayMaxCountInMonth(dt));
    ValidityChecker("hour"s, dt.hour, 0, 23);
    ValidityChecker("minute"s, dt.minute, 0, 59);
    ValidityChecker("second"s, dt.seconds, 0, 59);
}

// ----- ParseCitySubjson.cpp -----
#include <string>
#include <vector>

using namespace std;

struct Country {
    string name_;
    string iso_code_;
    string phone_code_;
    string time_zone_;
    vector<Language> languages_;
};

void ParseCitySubjson(vector<City> &cities, const Json &json, const Country &country) {
    for (const auto &city_json: json.AsList()) {
        const auto &city_obj = city_json.AsObject();
        cities.push_back(
                {
                        city_obj["name_"s].AsString(),
                        city_obj["iso_code_"s].AsString(),
                        country.phone_code_ + city_obj["phone_code_"s].AsString(),
                        country.name_,
                        country.iso_code_,
                        country.time_zone_,
                        country.languages_
                }
        );
    }
}

void ParseCountryJson(vector<Country> &countries, vector<City> &cities, const Json &json) {
    for (const auto &country_json: json.AsList()) {
        const auto &country_obj = country_json.AsObject();
        countries.push_back(
                {
                        country_obj["name_"s].AsString(),
                        country_obj["iso_code_"s].AsString(),
                        country_obj["phone_code_"s].AsString(),
                        country_obj["time_zone_"s].AsString(),
                }
        );
        Country &country = countries.back();
        for (const auto &lang_obj: country_obj["languages"s].AsList()) {
            country.languages_.push_back(FromString<Language>(lang_obj.AsString()));
        }
        ParseCitySubjson(cities, country_obj["cities"s], country);
    }
}
// ----- The End -----