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
    add_files("src/memtable/*.cc")
    add_includedirs("include", {public = true})
    add_deps("skiplist")  -- ����skiplist��

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