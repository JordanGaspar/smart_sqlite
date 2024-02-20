# how to compile on fedora linux 39

```bash
g++ your_main_file.cpp smart_sqlite.cpp -lsqlite3 -std=c++23
```

# compile with cmake
cd smart_sqlite
mkdir build
cmake -S . -B build
cd build
make all

# usage

```cpp
//example
#include "smart_sqlite.hpp"
//... other include libraries ...

int main() {
  sqlite db("file.sqlite");

  db.exec("create table if not exists test (name text, password text, age "
          "integer, balance float);");
  db.exec("insert into test (name, password, age, balance) values ('Jordan', "
          "'123456', 30, "
          "450.30), "
          "('User123', '654321random@', 36, 750.30), ('Opuscopus', "
          "'somerandom', 13, 5.43);");

  auto stmt = db.prepare("select name, password, age, balance from test;");

  std::cout << "Only values: \n" << std::endl;

  auto step = stmt->step();

  for (auto x : step) {
    auto name = x[0].as_text();
    auto password = x[1].as_text();
    auto age = x[2].as_integer();
    auto balance = x[3].as_float();

    printf("Name: %s; Password: %s; Age: %lld; Balance: %f\n", name.c_str(),
           password.c_str(), age, balance);
  }

  std::cout << "\nWith names: \n" << std::endl;

  auto with_name = stmt->step_with_column_name();

  auto name_column = with_name["name"];
  auto password_column = with_name["password"];
  auto age_column = with_name["age"];
  auto balance_column = with_name["balance"];

  auto ziped = std::ranges::views::zip(name_column, password_column, age_column,
                                       balance_column);

  for (auto [name, password, age, balance] : ziped) {
    printf("Name: %s; Password: %s; Age: %lld; Balance: %f\n",
           name.as_text().c_str(), password.as_text().c_str(), age.as_integer(),
           balance.as_float());
  }

  std::cout << std::endl;

  return 0;
}
```
