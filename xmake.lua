-- 定义项目
set_project("lsmkv")
set_version("0.0.1")
set_languages("c++20")

add_rules("mode.debug", "mode.release")
add_requires("gtest") -- 添加gtest依赖

target("skiplist")
    set_kind("static")  -- 生成静态库
    add_files("src/skiplist/*.cc")
    add_includedirs("include", {public = true})

target("memtable")
    set_kind("static")  -- 生成静态库
    add_files("src/memtable/*.cc")
    add_includedirs("include", {public = true})
    add_deps("skiplist")  -- 依赖skiplist库

-- 定义测试
target("test_skiplist")
    set_kind("binary")  -- 生成可执行文件
    add_files("test/test_skiplist.cc")
    add_deps("skiplist")  -- 依赖skiplist库
    add_packages("gtest")  -- 添加gtest包

target("test_memtable")
    set_kind("binary")  -- 生成可执行文件
    add_files("test/test_memtable.cc")
    add_deps("memtable")  -- 依赖memtable库
    add_packages("gtest")  -- 添加gtest包
    add_includedirs("include")