workspace(name = "bash_history_db")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Note:
# local_repository(name = "...", path = "/home/...")

#############################################
# needed by com_github_glog_glog
git_repository(
    name = "com_github_gflags_gflags",
    remote = "git://github.com/gflags/gflags.git",
    tag = "v2.2.2",  # current as of 2020/1/25
)

#############################################
git_repository(
    name = "com_github_glog_glog",
    remote = "git://github.com/google/glog.git",
    tag = "v0.4.0",
)

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
    remote = "git://github.com/google/googletest.git",
    tag = "release-1.10.0",  # current as of 2020/1/25
)

#############################################
git_repository(
    name = "com_google_absl",
    commit = "44427702614d7b86b064ba06a390f5eb2f85dbf6",  # current as of 2020/1/25
    remote = "git://github.com/abseil/abseil-cpp.git",
)

#############################################
git_repository(
    name = "bazel_rules",
    commit = "83f66ea56b852ac7108f901ba311a54bf3249e05",  # current as of 2020/1/29
    remote = "git://github.com/bcsgh/bazel_rules.git",
)
