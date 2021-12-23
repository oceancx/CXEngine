rm -rf dist

mkdir -p dist/.vscode
cp launch.json dist/.vscode

mkdir -p dist/bin
cp bin/Release/CXServer.exe dist/bin/
cp bin/Release/CXEngine.exe dist/bin/
cp bin/Release/cxluax.exe dist/bin/

mkdir -p dist/res
cp -r res/font dist/res/
cp -r res/icon dist/res/
cp -r res/shader dist/res/
cp -r res/storage dist/res/
cp -r res/tables dist/res/
cp -r res/fbx dist/res/

mkdir -p dist/data
cp -r data/scene   dist/data
cp data/addon.wdf   dist/data
cp data/atom.wdf    dist/data
cp data/chat.wdf    dist/data
cp data/firework.wdf    dist/data
cp data/goods.wdf   dist/data
cp data/item.wd1    dist/data
cp data/item.wdf    dist/data
cp data/magic.wd1   dist/data
cp data/magic.wdf   dist/data
cp data/mapani.wdf  dist/data
cp data/mhimage.wd1 dist/data
cp data/mhimage.wdf dist/data
cp data/mhxiu.wdf   dist/data
cp data/misc.wdf    dist/data
cp data/music.wdf   dist/data
cp data/mypal.wdf   dist/data
cp data/scene.wdf   dist/data
cp data/shape.wd1   dist/data
cp data/shape.wd2   dist/data
cp data/shape.wd3   dist/data
cp data/shape.wd4   dist/data
cp data/shape.wd5   dist/data
cp data/shape.wd6   dist/data
cp data/shape.wd7   dist/data
cp data/shape.wd8   dist/data
cp data/shape.wd9   dist/data
cp data/shape.wda   dist/data
cp data/shape.wdf   dist/data
cp data/smap.wdf    dist/data
cp data/sound.wdf   dist/data
cp data/stock.wdf   dist/data
cp data/waddon.wdf  dist/data
cp data/wzife.wd1   dist/data
cp data/wzife.wd2   dist/data
cp data/wzife.wd3   dist/data
cp data/wzife.wdf   dist/data
cp data/wzimage.wdf dist/data

mkdir -p dist/scripts
cp -r scripts/client dist/scripts/
cp -r scripts/combat dist/scripts/
cp -r scripts/server dist/scripts/
cp -r scripts/share dist/scripts/
cp -r scripts/generator dist/scripts/
cp -r scripts/launcher dist/scripts/
cp -r scripts/examples dist/scripts/

cp imgui.ini dist/

touch dist/launcher.bat
echo 'copy /y bin\cxluax.exe  bin\launcher.exe&start bin/launcher.exe  --cwd=%cd%/ --script_path=scripts/launcher/' > dist/launcher.bat



# mv data/* dist/data