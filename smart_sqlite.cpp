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

#include <smart_sqlite.hpp>
// #include <iostream>
#include <memory>
#include <new>
// #include <ranges>
#include <sqlite3.h>
#include <stdexcept>
#include <vector>

struct sqlite::stmt::stmt_deleter {
  void operator()(sqlite3_stmt *stmt) { sqlite3_finalize(stmt); }
};

struct sqlite::impl {
  std::shared_ptr<sqlite3> db;
};

struct sqlite::stmt::impl {
  std::shared_ptr<sqlite3> db;
  std::unique_ptr<sqlite3_stmt, sqlite::stmt::stmt_deleter> stmt;
};

sqlite::stmt::stmt(sqlite *ptr, std::string_view query)
    : pimpl(new sqlite::stmt::impl) {

  pimpl->db = ptr->pimpl->db;

  auto db = pimpl->db.get();

  sqlite3_stmt *sqlite_stmt;

  if (sqlite3_prepare(db, query.data(), query.size(), &sqlite_stmt, NULL) !=
      SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db));
  }

  pimpl->stmt.reset(sqlite_stmt);
}

void sqlite::stmt::bind(int index, int var) {
  int rc;

  if ((rc = sqlite3_bind_int(pimpl->stmt.get(), index, var)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, long long var) {
  int rc;

  if ((rc = sqlite3_bind_int64(pimpl->stmt.get(), index, var)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, float var) {
  int rc;

  if ((rc = sqlite3_bind_double(pimpl->stmt.get(), index, var)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, double var) {
  int rc;

  if ((rc = sqlite3_bind_double(pimpl->stmt.get(), index, var)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, std::span<char> var) {
  int rc;

  if ((rc = sqlite3_bind_blob64(pimpl->stmt.get(), index, var.data(),
                                var.size(), SQLITE_TRANSIENT)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, std::span<unsigned char> var) {
  int rc;

  if ((rc = sqlite3_bind_blob64(pimpl->stmt.get(), index, var.data(),
                                var.size(), SQLITE_TRANSIENT)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, const std::vector<char> &var) {
  int rc;

  if ((rc = sqlite3_bind_blob64(pimpl->stmt.get(), index, var.data(),
                                var.size(), SQLITE_TRANSIENT)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, const std::vector<unsigned char> &var) {
  int rc;

  if ((rc = sqlite3_bind_blob64(pimpl->stmt.get(), index, var.data(),
                                var.size(), SQLITE_TRANSIENT)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, std::string_view var) {
  int rc;

  if ((rc = sqlite3_bind_text(pimpl->stmt.get(), index, var.data(), var.size(),
                              SQLITE_TRANSIENT)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, const std::string &var) {
  int rc;

  if ((rc = sqlite3_bind_text(pimpl->stmt.get(), index, var.data(), var.size(),
                              SQLITE_TRANSIENT)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, std::wstring_view var) {
  int rc;

  if ((rc = sqlite3_bind_text16(pimpl->stmt.get(), index, var.data(),
                                var.size(), SQLITE_TRANSIENT)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, const std::wstring &var) {
  int rc;

  if ((rc = sqlite3_bind_text16(pimpl->stmt.get(), index, var.data(),
                                var.size(), SQLITE_TRANSIENT)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, const char *var) {
  int rc;

  if ((rc = sqlite3_bind_text(pimpl->stmt.get(), index, var, -1,
                              SQLITE_TRANSIENT)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

void sqlite::stmt::bind(int index, const wchar_t *var) {
  int rc;

  if ((rc = sqlite3_bind_text16(pimpl->stmt.get(), index, var, -1,
                                SQLITE_TRANSIENT)) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errstr(rc));
  }
}

sqlite::sqlite(const char *file_name) : pimpl(new impl) {

  auto db = pimpl->db.get();

  if (sqlite3_open(file_name, &db) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db));
  }

  pimpl->db.reset(db, [=](sqlite3 *db) { sqlite3_close_v2(db); });
}

sqlite::~sqlite() {}

sqlite::stmt_t sqlite::prepare(std::string_view query) {
  return sqlite::stmt_t(new stmt(this, query));
}

void sqlite::exec(const char *query,
                  int (*callback)(void *, int, char **, char **), void *arg) {

  struct errmsg_deleter {
    void operator()(void *ptr) { sqlite3_free(ptr); }
  };

  char *errmsg = nullptr;

  int rc = sqlite3_exec(pimpl->db.get(), query, callback, arg, &errmsg);

  std::unique_ptr<char, errmsg_deleter> ptr(errmsg);

  if (rc != SQLITE_OK) {
    throw std::runtime_error(ptr.get());
  }
}

sqlite::stmt::table_t sqlite::stmt::step() {

  struct stmt_reset {
    void operator()(void *ptr) {
      sqlite3_reset((sqlite3_stmt *)ptr);
      sqlite3_clear_bindings((sqlite3_stmt *)ptr);
    };
  };

  std::unique_ptr<sqlite3_stmt, stmt_reset> stmt(pimpl->stmt.get());

  table_t table;

  int rc;

  while ((rc = sqlite3_step(pimpl->stmt.get())) != SQLITE_DONE) {
    if (rc == SQLITE_ROW) {
      int num_cols = sqlite3_column_count(stmt.get());
      row_t row;

      for (int i = 0; i < num_cols; i++) {
        switch (sqlite3_column_type(stmt.get(), i)) {
        case (SQLITE3_TEXT): {
          auto ptr = sqlite3_column_text(stmt.get(), i);
          auto size = sqlite3_column_bytes(stmt.get(), i);
          row.emplace_back(cell_t{std::string(ptr, ptr + size)});
          break;
        }
        case (SQLITE_INTEGER):
          row.emplace_back(cell_t{sqlite3_column_int(stmt.get(), i)});
          break;
        case (SQLITE_FLOAT):
          row.emplace_back(cell_t{sqlite3_column_double(stmt.get(), i)});
          break;
        case (SQLITE_BLOB): {
          auto ptr = reinterpret_cast<const char *>(
              sqlite3_column_blob(stmt.get(), i));
          auto size = sqlite3_column_bytes(stmt.get(), i);
          row.emplace_back(cell_t{std::vector<char>(ptr, ptr + size)});
          break;
        }
        case (SQLITE_NULL):
          row.emplace_back(cell_t{nullptr});
          break;
        default:
          break;
        }
      }

      table.emplace_back(row);

    } else {
      throw std::runtime_error(sqlite3_errstr(rc));
    }
  }

  return table;
}

sqlite::stmt::column_with_name_t sqlite::stmt::step_with_column_name() {

  struct stmt_reset {
    void operator()(void *ptr) {
      sqlite3_reset((sqlite3_stmt *)ptr);
      sqlite3_clear_bindings((sqlite3_stmt *)ptr);
    };
  };

  std::unique_ptr<sqlite3_stmt, stmt_reset> stmt(pimpl->stmt.get());

  column_with_name_t column_with_name;

  int rc;

  while ((rc = sqlite3_step(pimpl->stmt.get())) != SQLITE_DONE) {
    if (rc == SQLITE_ROW) {
      int num_cols = sqlite3_column_count(stmt.get());

      for (int i = 0; i < num_cols; i++) {
        const char *col_name = sqlite3_column_name(stmt.get(), i);

        if (col_name == nullptr) {
          throw std::bad_alloc();
        }

        switch (sqlite3_column_type(stmt.get(), i)) {
        case (SQLITE3_TEXT): {
          auto ptr = sqlite3_column_text(stmt.get(), i);
          auto size = sqlite3_column_bytes(stmt.get(), i);
          column_with_name[col_name].emplace_back(
              cell_t{std::string(ptr, ptr + size)});
          break;
        }
        case (SQLITE_INTEGER):
          column_with_name[col_name].emplace_back(
              cell_t{sqlite3_column_int(stmt.get(), i)});
          break;
        case (SQLITE_FLOAT):
          column_with_name[col_name].emplace_back(
              cell_t{sqlite3_column_double(stmt.get(), i)});
          break;
        case (SQLITE_BLOB): {
          auto ptr = reinterpret_cast<const char *>(
              sqlite3_column_blob(stmt.get(), i));
          auto size = sqlite3_column_bytes(stmt.get(), i);
          column_with_name[col_name].emplace_back(
              cell_t{std::vector<char>(ptr, ptr + size)});
          break;
        }
        case (SQLITE_NULL):
          column_with_name[col_name].emplace_back(cell_t{nullptr});
          break;
        default:
          break;
        }
      }
    } else {
      throw std::runtime_error(sqlite3_errstr(rc));
    }
  }

  return column_with_name;
}

bool sqlite::stmt::cell_t::is_integer() {
  if (this->index() == INTEGER) {
    return true;
  } else {
    return false;
  }
}

bool sqlite::stmt::cell_t::is_float() {
  if (this->index() == FLOAT) {
    return true;
  } else {
    return false;
  }
}

bool sqlite::stmt::cell_t::is_text() {
  if (this->index() == TEXT) {
    return true;
  } else {
    return false;
  }
}

bool sqlite::stmt::cell_t::is_blob() {
  if (this->index() == BLOB) {
    return true;
  } else {
    return false;
  }
}

bool sqlite::stmt::cell_t::is_null() {
  if (this->index() == NULL_VALUE) {
    return true;
  } else {
    return false;
  }
}

long long sqlite::stmt::cell_t::as_integer() {
  return std::get<long long>(*this);
}

double sqlite::stmt::cell_t::as_float() { return std::get<double>(*this); }

std::string sqlite::stmt::cell_t::as_text() {
  return std::get<std::string>(*this);
}

std::vector<char> sqlite::stmt::cell_t::as_blob() {
  return std::get<std::vector<char>>(*this);
}

std::nullptr_t sqlite::stmt::cell_t::as_null() { return nullptr; }
