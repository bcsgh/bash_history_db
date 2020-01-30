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

#include <cstdlib>
#include <string>

#include "cli/db.h"
#include "cli/sqlite_handle.h"
#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "sqlite3.h"

namespace bash_history_db {
namespace {

int LogSteps(void *p, int count, char **text, char **names) {
  auto *m = static_cast<std::map<std::string, std::string> *>(p);

  if (!m->empty()) {
    LOG(ERROR) << "Expected 1 row, found more than 1";
    return 1;
  }

  for (int i = 0; i < count; i++) {
    if (text[i]) {
      LOG(INFO) << names[i] << ": '" << text[i] << "'";
      m->emplace(names[i], text[i]);
    } else {
      LOG(INFO) << names[i] << ": NULL";
      m->emplace(names[i], "NULL");
    }
  }

  return 0;
}

TEST(SQLiteBasic, Test) {
  std::string file = "test.sqlite3";

  LOG(INFO) << file;
  SqlietHandle db{
      file, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX};
  ASSERT_TRUE(db.ok()) << sqlite3_errstr(db.status());

  ASSERT_TRUE(CreateDb(db.db()));

  Row row;
  row.pid = 0;
  row.time = 0;
  row.host = "host";
  row.user = "user";
  row.rpwd = "/path";
  row.pwd = "path";
  row.cmd = "true";

  ASSERT_TRUE(Insert(db.db(), row));

  char *err;
  std::map<std::string, std::string> result;
  auto check_status =
      sqlite3_exec(db.db(), "SELECT * FROM history", &LogSteps, &result, &err);
  std::shared_ptr<char> cleanup(err, &sqlite3_free);

  ASSERT_EQ(SQLITE_OK, check_status) << err;

  // Check the values.
  using ::testing::Contains;
  using ::testing::Pair;
  EXPECT_THAT(result, Contains(Pair("Host", "host")));
  EXPECT_THAT(result, Contains(Pair("PID", "0")));
  EXPECT_THAT(result, Contains(Pair("User", "user")));
  EXPECT_THAT(result, Contains(Pair("Timestamp", "0")));
  EXPECT_THAT(result, Contains(Pair("PWD", "path")));
  EXPECT_THAT(result, Contains(Pair("Resolved_PWD", "/path")));
  EXPECT_THAT(result, Contains(Pair("Command", "true")));
}

}  // namespace
}  // namespace bash_history_db
