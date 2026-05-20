cmake -S backend -B backend/build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_STANDARD=20
cmake --build E:/NotSelfDAO/app/pic_gallery/backend/build -j 16