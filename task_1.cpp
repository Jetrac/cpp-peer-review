#include <vector>
#include <iomanip>
#include <iostream>

using namespace std;

class ReadingManager {
private:
    static const int MAX_USERS_COUNT_ = 100000;
    static const int MAX_PAGE_COUNT_ = 1000;

    vector<int> users_to_page_count_ = vector<int>(MAX_USERS_COUNT_ + 1, 0);
    vector<int> page_to_users_count_ = vector<int>(MAX_PAGE_COUNT_ + 1, 0);
    int readers_count_ = 0;
public:
    void PrintQueries(istream& in, ostream& out) {
        int query_count;
        in >> query_count;

        for (int query_number = 0; query_number < query_count; ++query_number) {
            string query_type;
            in >> query_type;

            int user_id;
            in >> user_id;

            if (query_type == "READ") {
                int page_count;
                in >> page_count;

                ReadRequestHandler(user_id, page_count);
            } else if (query_type == "CHEER") {
                out << setprecision(6) << CheerRequestHandler(user_id) << endl;
            }
        }
    }

    void ReadRequestHandler(int user_id, int page_count) {
        int user_page = users_to_page_count_[user_id];
        if(user_page == 0) {
            users_to_page_count_[user_id] = page_count;
            ++page_to_users_count_[page_count];
            ++readers_count_;
        } else {
            --page_to_users_count_[user_page];
            users_to_page_count_[user_id] = page_count;
            ++page_to_users_count_[page_count];
        }
    }


    double CheerRequestHandler(int user_id) const {
        if (users_to_page_count_[user_id] == 0) {
            return 0;
        }

        if (readers_count_ == 1) {
            return 1;
        }

        int readers_count_up_to_the_page = 0;
        for (int i = 1; i < users_to_page_count_[user_id]; ++i) {
            readers_count_up_to_the_page += page_to_users_count_[i];
        }

        return readers_count_up_to_the_page * 1.0 / (readers_count_ - 1);
    }
};


int main() {
    ReadingManager reading_manager_;
    reading_manager_.PrintQueries(std::cin, std::cout);
}