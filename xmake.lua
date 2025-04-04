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

target("block")
    set_kind("static")  -- ���ɾ�̬��
    add_files("src/block/*.cc")
    add_includedirs("include", {public = true})

target("utils")
    set_kind("static")  -- ���ɾ�̬��
    add_files("src/utils/*.cc")
    add_includedirs("include", {public = true})

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