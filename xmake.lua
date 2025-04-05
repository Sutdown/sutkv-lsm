-- ������Ŀ
set_project("lsmkv")
set_version("0.0.1")
set_languages("c++20")

add_rules("mode.debug", "mode.release")
add_requires("gtest") -- ���gtest����

target("skiplist")
    set_kind("static")  -- ���ɾ�̬��
    add_files("src/skiplist/*.cc")
    add_includedirs("include", {public = true})

target("memtable")
    set_kind("static")  -- ���ɾ�̬��
    add_deps("skiplist")
    add_deps("iterator")
    add_deps("sst")
    add_files("src/memtable/*.cc")
    add_includedirs("include", {public = true})

target("block")
    set_kind("static")  -- ���ɾ�̬��
    add_files("src/block/*.cc")
    add_includedirs("include", {public = true})

target("utils")
    set_kind("static")  -- ���ɾ�̬��
    add_files("src/utils/*.cc")
    add_includedirs("include", {public = true})

target("iterator")
    set_kind("static")  -- ���ɾ�̬��
    add_files("src/iterator/*.cc")
    add_includedirs("include", {public = true})

target("sst")
    set_kind("static")  -- ���ɾ�̬��
    add_deps("block")
    add_deps("utils")
    add_files("src/sst/*.cc")
    add_includedirs("include", {public = true})

target("lsm")
    set_kind("static")  -- ���ɾ�̬��
    add_deps("sst")
    add_files("src/lsm/*.cc")
    add_includedirs("include", {public = true})

-- ���嶯̬���ӿ�Ŀ��
target("lsm_shared")
    set_kind("shared")
    add_files("src/**.cc")
    add_includedirs("include", {public = true})
    set_targetdir("$(buildir)/lib")

    -- ��װͷ�ļ��Ͷ�̬���ӿ�
    on_install(function (target)
        os.cp("include", path.join(target:installdir(), "include/toni-lsm"))
        os.cp(target:targetfile(), path.join(target:installdir(), "lib"))
    end)

-- �������
target("test_skiplist")
    set_kind("binary")  -- ���ɿ�ִ���ļ�
    add_files("test/test_skiplist.cc")
    add_deps("skiplist")  -- ����skiplist��
    add_packages("gtest")  -- ���gtest��

target("test_memtable")
    set_kind("binary")  -- ���ɿ�ִ���ļ�
    add_files("test/test_memtable.cc")
    add_deps("memtable")  -- ����memtable��
    add_packages("gtest")  -- ���gtest��
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
    add_deps("block")  -- ���memtable�Ƕ�����target��������Ҫ��Ӷ�Ӧ������
    add_packages("gtest")
    add_includedirs("include") 

target("test_utils")
    set_kind("binary")
    add_files("test/test_utils.cc")
    add_deps("utils")
    add_packages("gtest")
    add_includedirs("include")

target("test_sst")
    set_kind("binary")
    add_files("test/test_sst.cc")
    add_deps("sst")
    add_packages("gtest")
    add_includedirs("include")

target("test_lsm")
    set_kind("binary")
    add_files("test/test_lsm.cc")
    add_deps("lsm", "memtable", "iterator")  -- Added memtable and iterator dependencies
    add_packages("gtest")
    add_includedirs("include")

target("test_blockcache")
    set_kind("binary")
    add_files("test/test_blockcache.cc")
    add_deps("block")
    add_packages("gtest")
    add_includedirs("include")

-- ���尸��
target("test_main")
    set_kind("binary")
    add_files("src/main.cc")
    add_deps("lsm_shared")
    add_includedirs("include", {public = true})
    set_targetdir("$(buildir)/bin")