cd './bash'
find ../src  \( -name  "*.h" -o -name "*.cpp"  -o -name "*.inl" -o -name "*.hpp" \)  -print > tmp2.txt
sed  's#../src/##g' tmp2.txt >tmp.txt
sed  '/set(SRC_CPP_FILES/ r tmp.txt' SRC_CMakeLists_Template.txt > ../src/CMakeLists.txt
rm tmp.txt tmp2.txt
