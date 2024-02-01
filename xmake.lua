set_project("lqp")
set_version("0.1.0")

add_requires("glpk")

option("examples", { description = "Build examples", default = true })

add_rules("mode.debug", "mode.releasedbg", "mode.release")

if is_mode("sanitizers") then
    set_symbols("debug")
    set_optimize("none")
    set_policy("build.sanitizer.address", true)
    set_policy("build.sanitizer.undefined", true)
end

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
    add_defines("LQP_BUILD")
    add_files("library/*.cc")
    add_headerfiles("include/lqp/*.h")
    add_includedirs("include", { public = true })
    add_packages("glpk")

if has_config("examples") then

    target("glpk_example")
      set_kind("binary")
      add_files("examples/glpk_example.cc")
      add_deps("lqp")

end
