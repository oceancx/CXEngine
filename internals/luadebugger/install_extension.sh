vscodedir=$1internals/luadebugger/vscode
cd ${vscodedir};
vsce package;
code --install-extension luadbgadapter-0.1.0.vsix
