workspace(name = "bash_history_db")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Note:
# local_repository(name = "...", path = "/home/...")

#############################################
http_archive(
    name = "sqlite",
    build_file = "@//:BUILD.sqlite",
    sha256 = "f3c79bc9f4162d0b06fa9fe09ee6ccd23bb99ce310b792c5145f87fbcc30efca",
    strip_prefix = "sqlite-amalgamation-3310100",
    urls = ["https://www.sqlite.org/2020/sqlite-amalgamation-3310100.zip"],  # 3.31.1, current as of 2020/1/28
)

#############################################
git_repository(
    name = "com_google_googletest",
    commit = "b10fad38c4026a29ea6561ab15fc4818170d1c10",  # current as of 2023/11/12
    remote = "https://github.com/google/googletest.git",
    shallow_since = "1698701593 -0700",
)

#############################################
git_repository(
    name = "com_google_absl",
    commit = "483a2d59e649179ea9d9bc4d808f6c9d16646f9d",  # current as of 2023/11/12
    remote = "https://github.com/abseil/abseil-cpp.git",
    shallow_since = "1699496241 -0800",
)

#############################################
git_repository(
    name = "bazel_rules",
    commit = "be9e3fa50c41cf9a1e93d2288ce02c67047d71c3",  # current as of 2023/11/16
    remote = "https://github.com/bcsgh/bazel_rules.git",
    shallow_since = "1700184387 -0800",
)

#############################################

load("@bazel_rules//cc_embed_data:cc_embed_data_deps.bzl", cc_embed_data_deps = "get_deps")
load("@bazel_rules//repositories:repositories.bzl", "jsoncpp")

jsoncpp()
cc_embed_data_deps()
