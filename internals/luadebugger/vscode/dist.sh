rm -rf dist
mkdir dist
cp main.lua dist/
cp package.json dist/
cp Release/vsdbgadapter.exe dist/
cp README.md dist/

cd dist 
vsce package
vsce publish
