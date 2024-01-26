set_project("lqp")
set_version("0.1.0")

add_repositories("sched-repo https://github.com/SchedulingLab/xmake-repo")

add_requires("glpk")

add_rules("mode.debug", "mode.releasedbg", "mode.release")

set_policy("build.warning", true)
set_warnings("allextra")
set_languages("cxx17")
set_encodings("utf-8")

if is_plat("windows") then
  add_cxflags("/wd4251") -- Disable warning: class needs to have dll-interface to be used by clients of class blah blah blah
  add_defines("NOMINMAX", "_CRT_SECURE_NO_WARNINGS")
end

target("lqp")
    set_kind("shared")
    add_files("library/*.cc")
    add_files("library/solvers/*.cc")
    add_packages("glpk")
