// Copyright (c) 2018, Benjamin Shropshire,
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

#include <unistd.h>

#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/strings/str_join.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "cli/db.h"
#include "cli/sqlite_handle.h"
#include "glog/logging.h"

namespace {

std::string DbPath() {
  const char* file_env = getenv("HISTORYDB");
  if (!file_env) file_env = "~/.history.sqlite3";

  std::string file;
  // SQLite doesn't resolve ~/ paths.
  if (file_env[0] == '~') {
    file = absl::StrCat(getenv("HOME"), file_env + 1);
  } else {
    file = file_env;
  }

  static char ret[PATH_MAX];
  return realpath(file.c_str(), ret);
}

void MainPath(std::vector<absl::string_view> argv) {
  bash_history_db::Row row;

  // PPID (PID of the session)
  row.pid = getppid();  // Grab this befor it changes.

  // Fork off and die (or not if we can't fork at all).
  // Do this twice to avoid zombies.
  if (fork() > 0) exit(0);
  setsid();  // "release" from the parent
  if (fork() > 0) exit(0);

  // Host
  char buff[256];
  if (0 == gethostname(buff, sizeof(buff))) row.host = buff;

  // User
  row.user = cuserid(nullptr);

  // Timestamp
  row.time = absl::ToUnixMicros(absl::Now());

  // PWD
  std::shared_ptr<char> pwd{get_current_dir_name(), &free};
  row.pwd = pwd.get();

  // Resolved_PWD
  std::shared_ptr<char> rpwd{realpath(pwd.get(), nullptr), &free};
  row.rpwd = rpwd.get();

  // Command
  row.cmd = absl::StrJoin(argv, " ");

  // Open the DB
  bash_history_db::SqlietHandle db{DbPath().c_str(),
                                   SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX};
  CHECK(db.ok()) << sqlite3_errstr(db.status());

  // Don't worry to much about integrety.
  sqlite3_exec(db.db(), "PRAGMA synchronous=0;", nullptr, nullptr, nullptr);

  // Do insert.
  bash_history_db::Insert(db.db(), row);
}

constexpr char kMsg[] = R"(Usage:
CMD={this command}

Place in .bashrc
  trap '$CMD -- $(history 1)' DEBUG

Setup:
  $CMD --create)";

}  // namespace

ABSL_FLAG(bool, create, false, "Create a new DB if it doesn't exist");

int main(int argc, char** argv) {
  // The normal use is via:
  // $ trap `sql_log_history -- $(history 1)`
  // history returns '#### $ARGV` so we get `sql_log_history -- #### $ARGV`
  // We only care about the case with 4+ args and then only care about [3:]
  if (argc > 3 && argv[1][0] == '-' &&  //
      argv[1][1] == '-' && argv[1][2] == '\0') {
    MainPath({argv + 3, argv + argc});
    return 0;
  }
  absl::SetProgramUsageMessage(kMsg);

  auto args = absl::ParseCommandLine(argc, argv);
  google::InitGoogleLogging(args[0]);

  if (absl::GetFlag(FLAGS_create)) {
    auto path = DbPath();

    bash_history_db::SqlietHandle db{
        path, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE};
    CHECK(db.ok());
    CHECK(bash_history_db::CreateDb(db.db())) << path;
  }

  return 0;
}