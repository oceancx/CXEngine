
if [ $1 = "test" ]; then
    cd test;
    start ./Debug/luadbgtest.exe --cwd='E:/Github/luadebugger/test/'
fi

if [ $1 = "vscode" ]; then
    cd vscode;
    start ./Debug/vsdbgadapter.exe --cwd='E:/Github/luadebugger/vscode/'
fi
