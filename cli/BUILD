# Copyright (c) 2020, Benjamin Shropshire,
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

load("@bazel_rules//cc_embed_data:cc_embed_data.bzl", "cc_embed_data")

cc_library(
    name = "sqlite_handle",
    srcs = ["sqlite_handle.cc"],
    hdrs = ["sqlite_handle.h"],
    deps = [
        "@com_google_absl//absl/log:log",
        "@sqlite//:sqlite_lib",
    ],
)

cc_library(
    name = "test_main",
    srcs = ["test_main.cc"],
    deps = [
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/log:initialize",
        "@com_google_absl//absl/log:log",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest",
    ],
)

cc_embed_data(
    name = "sql",
    srcs = glob(["*.sql"]),
    namespace = "sql_scripts",
)

cc_library(
    name = "db",
    srcs = ["db.cc"],
    hdrs = ["db.h"],
    deps = [
        ":sql",
        ":sqlite_handle",
        "@com_google_absl//absl/log:log",
        "@com_google_absl//absl/time",
        "@sqlite//:sqlite_lib",
    ],
)

cc_test(
    name = "basic_test",
    timeout = "short",
    srcs = ["basic_test.cc"],
    deps = [
        ":db",
        ":sqlite_handle",
        ":test_main",
        "@com_google_absl//absl/log:log",
        "@com_google_googletest//:gtest",
    ],
)

cc_binary(
    name = "sql_log_history",
    srcs = ["log_main.cc"],
    deps = [
        ":db",
        ":sqlite_handle",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/log:initialize",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
    ],
)
