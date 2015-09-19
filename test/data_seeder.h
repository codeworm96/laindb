#ifndef DATA_SEEDER_H_
#define DATA_SEEDER_H_

#include <vector>
#include <sstream>
#include <cstdlib>

std::string itos(int x)
{
  std::ostringstream oss;
    oss << x;
    return oss.str();
}


class DataSeeder
{
    public:
        DataSeeder(int max_size) :size(0)
        {
            for (int i = 0; i < max_size; ++i){
                data.push_back(itos(i));
            }
        }

        std::string key_for_get()
        {
            int k = rand() % size;
            return data[k];
        }

        std::string key_for_del()
        {
            int k = rand() % size;
            std::string res = data[k];
            --size;
            data[k] = data[size];
            data[size] = res;
            return res;
        }

        std::string key_for_put()
        {
            int k = rand() % (data.size() - size) + size;
            std::string tmp = data[size];
            data[size] = data[k];
            data[k] = tmp;
            ++size;
            return data[size - 1];
        }
    private:
        int size;
        std::vector<std::string> data;
};

#endif //DATA_SEEDER_H_
