// Copyright (c) 2020, Benjamin Shropshire,
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "cli/db.h"

#include <memory>
#include <string>

#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "cli/sql_emebed_data.h"
#include "glog/logging.h"
#include "sqlite3.h"

namespace bash_history_db {

bool CreateDb(sqlite3* db) {
  std::string create{sql_scripts::cli_create_sql()};

  char* err;
  auto status = sqlite3_exec(
      db, create.c_str(),
      +[](void*, int, char**, char**) -> int { LOG(FATAL); }, nullptr, &err);
  std::shared_ptr<char> cleanup(err, &sqlite3_free);

  LOG_IF(ERROR, status != SQLITE_OK)
      << "Create failed: " << sqlite3_errstr(status) << " '"
      << (err ? err : "n/a") << "'";

  return status == SQLITE_OK;
}

namespace {
// Consume whitespace at the front of a null-terminated string.
// TODO also make this consume SQL comments.
const char* Consume(const char* tail) {
  while (true) {
    switch (*tail) {
      case '\n':
      case '\t':
      case ' ':
        tail++;
        continue;

      case '\0':
      default:
        return tail;
    }
  }
}

enum InsertParams {
  kInsert_Host = 1,
  kInsert_PID = 2,
  kInsert_User = 3,
  kInsert_Timestamp = 4,
  kInsert_PWD = 5,
  kInsert_Resolved_PWD = 6,
  kInsert_Command = 7,
};

int Insert(sqlite3_stmt* statment, int p, int64_t i) {
  return sqlite3_bind_int64(statment, p, i);
}
int Insert(sqlite3_stmt* statment, int p, std::string s) {
  return sqlite3_bind_text(statment, p, s.c_str(), s.size(), SQLITE_TRANSIENT);
}

}  // namespace

bool Insert(sqlite3* db, const Row row) {
  auto deadline = absl::Now() + absl::Milliseconds(100);

  std::string insert{sql_scripts::cli_insert_sql()};

  sqlite3_stmt* statment;
  const char* tail;
  std::shared_ptr<sqlite3_stmt> cleanup;
  int status;
  bool ret = true;

again:
  do {
    status = sqlite3_prepare_v2(db, insert.c_str(), insert.size() + 1,
                                &statment, &tail);
    cleanup = {statment, &sqlite3_finalize};
  } while (status == SQLITE_BUSY && absl::Now() > deadline);

  if (status != SQLITE_OK) {
    LOG(ERROR) << "prepare of Insert: " << sqlite3_errstr(status);
    return false;
  }

  // Check for trailing content.
  tail = Consume(tail);
  if (*tail) {
    LOG(ERROR) << "Unexpected text in statement: '" << tail << "'";
    ret = false;
  }

  // Use a macro to get line number in the logging.
#define LOG_SQLITE(L, status)  \
  if ((status) == SQLITE_OK) { \
  } else                       \
    LOG_IF(L, !(ret = false))  \
        << sqlite3_errstr(status) << " -- " << sqlite3_errmsg(db) << ": "

  LOG_SQLITE(ERROR, Insert(statment, kInsert_Host, row.host));
  LOG_SQLITE(ERROR, Insert(statment, kInsert_PID, row.pid));
  LOG_SQLITE(ERROR, Insert(statment, kInsert_User, row.user));
  LOG_SQLITE(ERROR, Insert(statment, kInsert_Timestamp, row.time));
  LOG_SQLITE(ERROR, Insert(statment, kInsert_PWD, row.pwd));
  LOG_SQLITE(ERROR, Insert(statment, kInsert_Resolved_PWD, row.rpwd));
  LOG_SQLITE(ERROR, Insert(statment, kInsert_Command, row.cmd));

#undef LOG_SQLITE

  switch (auto step_result = sqlite3_step(statment)) {
    case SQLITE_DONE:
      break;
    case SQLITE_BUSY:
      if (absl::Now() > deadline) return false;
      goto again;  // TODO figure out a better way to do this?

    default:
      LOG(ERROR) << "Unexpeted status: " << sqlite3_errstr(step_result);
      ret = false;
      break;
  }

  return ret;
}

}  // namespace bash_history_db
