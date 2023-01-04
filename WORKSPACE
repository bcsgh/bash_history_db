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
    commit = "3026483ae575e2de942db5e760cf95e973308dd5",  # current as of 2022/10/25
    remote = "https://github.com/google/googletest.git",
    shallow_since = "1666712359 -0700",
)

#############################################
git_repository(
    name = "com_google_absl",
    commit = "827940038258b35a29279d8c65b4b4ca0a676f8d",  # current as of 2022/10/27
    remote = "https://github.com/abseil/abseil-cpp.git",
    shallow_since = "1666903548 -0700",
)

#############################################
git_repository(
    name = "bazel_rules",
    commit = "8e3e79e7555ab797ceef2918d1fcdaffdacac6dc",  # current as of 2023/01/02
    remote = "https://github.com/bcsgh/bazel_rules.git",
    shallow_since = "1672689339 -0800",
)

#############################################

load("@bazel_rules//cc_embed_data:cc_embed_data_deps.bzl", cc_embed_data_deps = "get_deps")
load("@bazel_rules//repositories:repositories.bzl", "jsoncpp")

jsoncpp()
cc_embed_data_deps()
