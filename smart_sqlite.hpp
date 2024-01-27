/*
MIT License

Copyright (c) 2024 Jordan Gaspar Alves Silva

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <memory>
#include <span>
#include <string_view>
#include <variant>
#include <vector>

#include <map>

class sqlite {
  struct impl;
  std::unique_ptr<impl> pimpl;

  class stmt {
    struct stmt_deleter;
    struct impl;
    std::unique_ptr<impl> pimpl;

    void bind_many_(size_t i) {}

    template <typename Head, typename... Tail>
    void bind_many_(size_t i, Head head, Tail... tail) {
      bind(i, head);
      bind_many_(i + 1, tail...);
    }

    enum type { INTEGER, FLOAT, TEXT, BLOB, NULL_VALUE };

  protected:
    stmt(sqlite *ptr, std::string_view query);

  public:
    class cell_t : public std::variant<long long, double, std::string,
                                       std::vector<char>, std::nullptr_t> {
    public:
      bool is_integer();
      bool is_float();
      bool is_text();
      bool is_blob();
      bool is_null();

      long long as_integer();
      double as_float();
      std::string as_text();
      std::vector<char> as_blob();
      std::nullptr_t as_null();
    };

    typedef std::vector<cell_t> row_t;
    typedef std::vector<row_t> table_t;

    typedef std::string column_name_t;
    typedef std::vector<cell_t> column_t;
    typedef std::map<column_name_t, column_t> column_with_name_t;

    table_t step();
    column_with_name_t step_with_column_name();

    void bind(int index, int var);
    void bind(int index, long long var);
    void bind(int index, float var);
    void bind(int index, double var);
    void bind(int index, std::string_view var);
    void bind(int index, std::wstring_view var);
    void bind(int index, std::span<char> var);
    void bind(int index, std::span<unsigned char> var);
    void bind(int index, const std::string &var);
    void bind(int index, const std::wstring &var);
    void bind(int index, const std::vector<char> &var);
    void bind(int index, const std::vector<unsigned char> &var);
    void bind(int index, const char *var);
    void bind(int index, const wchar_t *var);

    template <typename... Args, size_t i = 1> void bind_many(Args... args) {
      bind_many_(i, args...);
    }

    friend class sqlite;
  };

public:
  typedef std::shared_ptr<stmt> stmt_t;

  sqlite(const char *file_name);
  stmt_t prepare(std::string_view query);
  void exec(const char *query,
            int (*callback)(void *, int, char **, char **) = nullptr,
            void *arg = nullptr);
};