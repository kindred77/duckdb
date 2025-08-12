we borrow the framework of gporca, not the logical and physical operators


cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_EXTENSIONS="icu;parquet;json"
-DXERCES_INCLUDE_DIR=c:/mywork/projects/cpp/gp-xerces/kindred/gp-xerces-archive/build_install/include -DXERCES_LIBRARY=c:/mywork/projects/cpp/gp-xerces/kindred/gp-xerces-archive/build_install/lib/libxerces-c.a ..