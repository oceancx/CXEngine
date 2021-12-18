--[[
CXEngine.exe --cwd=[工作目录] --script_path=[lua文件夹相对工作目录]/  --luapath=[相对script_path下的lua文件路径]
示例：
./bin/Debug/CXEngine.exe --cwd=[CXEngine目录的路径]/  --script_path=scripts/examples/ --luapath=example0.lua

example1 显示WDF文件里的动画

ADDONWDF        =  0;    //addon.wdf
ATOMWDF         =  1;    //atom.wdf
CHATWDF         =  2;    //chat.wdf
FIREWORKWDF     =  3;    //firework.wdf
GOODSWDF        =  4;    //goods.wdf
ITEMWDF         =  5;    //item.wdf
MAGICWDF        =  6;    //magic.wdf
MAPANIWDF       =  7;    //mapani.wdf
MHIMAGEWDF      =  8;    //mhimage.wdf
MISCWDF         =  9;    //misc.wdf
MUSICWDF        =  10;   //music.wdf
SCENEWDF        =  11;   //scene.wdf
SHAPEWDF        =  12;   //shape.wdf
SHAPEWD1        =  13;   //shape.wd1
SHAPEWD2        =  14;   //shape.wd2
SHAPEWD3        =  15;   //shape.wd3
SHAPEWD4        =  16;   //shape.wd4
SHAPEWD5        =  17;   //shape.wd5
SHAPEWD6        =  18;   //shape.wd6
SHAPEWD7        =  19;   //shape.wd7
SMAPWDF         =  20;   //smap.wdf
SOUNDWDF        =  21;   //sound.wdf
STOCKWDF        =  22;   //stock.wdf
WADDONWDF       =  23;   //waddon.wdf
WZIFEWDF        =  24;   //wzife.wdf
WZIFEWD1        =  25;   //wzife.wd1
]]

local anim = nil
function init()
    anim = animation_create(WADDONWDF,0xCA8FDEAD)         --0xCA8FDEAD为was在wdf里的hash值,CA8FDEAD可以在res\tables\ini\waddon.wdf.ini找到对应的动画名字
    anim:SetPos(200,100)                --设置动画在屏幕上的位置,屏幕的原点(0,0)在左上角
    anim:SetLoop(0)                     --设置动画的循环次数,0表示无限循环
    anim:Play()                         --动画创建后默认状态是STOP,STOP状态下调用动画Update,动画是不会更新的.调用Play会让动画进入PLAY状态,PLAY状态下调用动画Update,动画是会更新
end

function update()
    if anim then
        anim:Update()                   --在游戏主循环里调用anim的Update函数, Update里面会根据这一帧的delta time还有自己的默认换帧时间去更新当前的帧数
        anim:Draw()                     --绘制动画里的当前帧
    end
end

--程序运行从这里开始
do
    iw_init(800,600)            --创建一个窗口（800x600）
    init()                      --初始化函数
    iw_render(update)           --update函数注册到窗口更新回调
    iw_deinit()                 --程序结束
end

