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

target("block")
    set_kind("static")  -- 生成静态库
    add_files("src/block/*.cc")
    add_includedirs("include", {public = true})

target("utils")
    set_kind("static")  -- 生成静态库
    add_files("src/utils/*.cc")
    add_includedirs("include", {public = true})

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

target("test_block")
    set_kind("binary")
    add_files("test/test_block.cc")
    add_deps("block")  
    add_packages("gtest")
    add_includedirs("include")

target("test_blockmeta")
    set_kind("binary")
    add_files("test/test_blockmeta.cc")
    add_deps("block")  -- 如果memtable是独立的target，这里需要添加对应的依赖
    add_packages("gtest")
    add_includedirs("include") 

target("test_utils")
    set_kind("binary")
    add_files("test/test_utils.cc")
    add_deps("utils")
    add_packages("gtest")
    add_includedirs("include")