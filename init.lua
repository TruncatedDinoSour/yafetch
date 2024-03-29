--[[
    Example user config: https://ari-web.xyz/dotfile/gentoo/dotfiles/config/yafetch/init.lua
--]]

local red = "\27[31m"
local grn = "\27[32m"
local yel = "\27[33m"
local blu = "\27[34m"
local wht = "\27[37m"
local bld = "\27[1m"
local res = "\27[0m"

yafetch.sep = " ~ "
yafetch.sep_color = bld

--[[
    if set to false, yafetch.shell() will return
    the full path of the current shell
--]]

yafetch.shell_base = true
local shell = yafetch.shell()
local shell_icon = " "

local kernel = yafetch.kernel()
local kernel_icon = " "

local pkgs = yafetch.pkgs()
local pkgs_icon = " "

local distro = yafetch.distro()

-- https://www.nerdfonts.com/cheat-sheet (search for 'linux')
local distro_icons = {
    ["Alpine Linux"] = "\u{f313}",
    ["Arch Linux"] = "\u{f303}",
    ["CentOS"] = "\u{f304}",
    ["Container_Linux"] = "\u{f305}",
    ["Debian"] = "\u{f306}",
    ["Devuan"] = "\u{f307}",
    ["Elementary"] = "\u{f309}",
    ["Fedora"] = "\u{f30a}",
    ["FreeBSD"] = "\u{f30c}",
    ["Gentoo"] = "\u{f30d}",
    ["Linux Mint"] = "\u{f30f}",
    ["Mageia"] = "\u{f310}",
    ["Mandriva"] = "\u{f311}",
    ["Manjaro"] = "\u{f312}",
    ["NixOS"] = "\u{f313}",
    ["OpenSUSE"] = "\u{f314}",
    ["Raspbian"] = "\u{f315}",
    ["Redhat"] = "\u{f316}",
    ["Sabayon"] = "\u{f317}",
    ["Slackware"] = "\u{f318}",
    ["Ubuntu"] = "\u{f31b}",
    ["Linux"] = "\u{f31a}",
}

function running_as_root()
    local fd = io.open"/root"
    if fd == nil then return false else io.close(fd) return true end
end



if running_as_root() then
    --[
    -- Dear future me,
    -- I am very sorry for the bad formatting,
    -- I cannot make it any prettier :(
    -- I hope you're not too mad about it...
    --]

    ascii1 = grn .. "    \\^V//       "  .. red .. bld
    ascii2 = res .. "    |. .|       "   .. res
    ascii3 = res .. "  - \\ - / _     "  .. res
    ascii4 = res .. "   \\_| |_/      "  .. res
    ascii5 = res .. "     \\ \\        " .. res
    ascii6 = res .. "   "                .. red .. "__" .. res .. "/_/" .. red .. "__      "   .. res
    ascii7 = red .. "  |_______|     "   .. res
    ascii8 = red .. "   \\     /      "  .. res .. "I am (g)root, don't break me"              .. res
    ascii9 = red .. "    \\___/       "  .. res

    root_mode = true
    root_extras = {
        ascii6,
        ascii7,
        ascii8,
        ascii9
    }
else
    ascii1 = wht .. " ,d88b.d88b,    " .. res
    ascii2 = red .. " 88888888888    " .. res
    ascii3 = grn .. " `Y8888888Y´    " .. res
    ascii4 = yel .. "   `Y888Y´      " .. res
    ascii5 = blu .. "     `Y´        " .. res

    root_mode = false
    root_extras = {}
end

yafetch.header_sep = string.format("%s@%s", blu, res)
yafetch.header_sep_color = wht
yafetch.header_format = ascii1  -- could be ascii1, an icon, etc.

function yafetch.init()
    yafetch.header()
    yafetch.format(ascii2 .. res .. red, (distro_icons[distro] or distro_icons["Linux"]) .. " ", wht, distro)
    yafetch.format(ascii3 .. res .. grn, shell_icon, wht, shell)
    yafetch.format(ascii4 .. res .. yel, kernel_icon, wht, kernel)
    yafetch.format(ascii5 .. res .. blu, pkgs_icon, wht, pkgs)

    if root_mode then
        for extra_art in pairs(root_extras) do
                print(root_extras[extra_art])
        end
    end
end
