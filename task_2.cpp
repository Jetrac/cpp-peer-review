#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <string_view>

using namespace std;

class Domain {
private:
    string domain_;
public:
    explicit Domain(string str) : domain_(std::move(str)) {}

    bool operator==(const Domain& rhs) const {
        return lexicographical_compare(
                domain_.begin(),
                domain_.end(),
                rhs.domain_.begin(),
                rhs.domain_.end()
        );
    }

    bool IsSubDomain(const Domain &rhs) const {
        if (domain_.size() >= rhs.domain_.size()) {
            return equal(rhs.domain_.begin(), rhs.domain_.end(), domain_.begin());
        }

        return false;
    }

    string GetValue() const {
        return domain_;
    }
};

class DomainChecker {
private:
    vector<Domain> forbidden_domains_;
public:
    template<typename It>
    DomainChecker(It begin, It end): forbidden_domains_(begin, end) {
        SortUnique();
    }

    bool IsForbidden(const Domain &domains) {
        if (forbidden_domains_.empty()) {
            return false;
        }

        auto it = upper_bound(forbidden_domains_.begin(), forbidden_domains_.end(), domains,[](const Domain &lhs, const Domain &rhs) {
            return lhs.GetValue() < rhs.GetValue();
        });

        if (it != forbidden_domains_.begin()) {
            return domains.IsSubDomain(forbidden_domains_[it - forbidden_domains_.begin() - 1]);
        } else {
            return domains.IsSubDomain(forbidden_domains_[it - forbidden_domains_.begin()]);
        }
    }

private:
    void SortUnique() {
        sort(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain &lhs, const Domain &rhs) {
            return lhs.GetValue() < rhs.GetValue();
        });

        auto last = unique(forbidden_domains_.begin(), forbidden_domains_.end(),[](const Domain &lhs, const Domain &rhs) {
            return rhs.IsSubDomain(lhs);
        });

        forbidden_domains_.erase(last, forbidden_domains_.end());
    }
};

template<typename Number>
vector<Domain> ReadDomains(istream &input, Number domains_number) {
    vector<Domain> domains;

    for (Number i = 0; i < domains_number; ++i) {
        string line;
        getline(input, line);

        line.insert(0, 1, '.');
        reverse(line.begin(), line.end());

        domains.emplace_back(std::move(line));
    }

    return domains;
}

template<typename Number>
Number ReadNumberOnLine(istream &input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

int main() {
    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain &domain: test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}