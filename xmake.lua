set_project("Heng")
set_version("0.0.1")

set_xmakever("2.7.5")

set_warnings("everything")

add_rules("mode.debug", "mode.release")

-- helper
local function _replace_ext(target, ext)
    return path.join(target:targetdir(), target:basename() .. ext)
end

rule("freestanding")
    on_config(function (target)
        local flags =
        {
            "-nostdinc", "-nostdlib", "-ffreestanding",
            "-fno-builtin", "-fno-pic", "-fno-stack-protector",
            "-mno-mmx", "-mno-sse", "-mno-sse2"
        }

        local ldflags = {"-nostdlib", "--omagic"}

        if target:get("arch") == "i386" then
            table.insert(flags, "-m32")

            table.insert(ldflags, "-m")
            table.insert(ldflags, "elf_i386")
        elseif  target:get("arch") == "x64" then
            table.insert(flags, "-m64")
            table.insert(flags, "-mno-red-zone")
            table.insert(flags, "-mcmodel=large")
            table.insert(flags, "-fno-strict-aliasing")

            table.insert(ldflags, "-m")
            table.insert(ldflags, "elf_x86_64")
        end

        target:add("asflags", flags)
        target:add("cflags", flags)
        target:add("ldflags", ldflags, {force = true})
    end)
rule_end()

rule("extra_obj")
    after_link(function (target, opt)
        import("core.project.depend")
        import("lib.detect.find_tool")
        import("utils.progress")
        depend.on_changed(function()
            local objcopy = assert(find_tool("objcopy"), "objcopy not found!")
            local flags =
            {
                "-O", "binary",
                "-j", ".text",
                "-j", ".rodata",
                "-j", ".data",
                "-j", ".bss",
                target:targetfile(),
                _replace_ext(target, ".bin")
            }

            os.iorunv(objcopy.program, flags)
            progress.show(opt.progress, "${color.build.target}extra %s", target:targetfile())
        end, {files = {target:targetfile()}})
    end)

    on_clean(function (target)
        os.tryrm(_replace_ext(target, ".bin"))
    end)
rule_end()

rule("image")
    after_link(function (target, opt)
        import("core.project.project")
        import("core.project.depend")
        import("lib.detect.find_tool")
        import("utils.progress")
        depend.on_changed(function()
            -- prepare
            local mbr_magic = path.join(target:targetdir(), "mbr_magic")
            -- x86 magic number
            local file = assert(io.open(mbr_magic, "wb"), "Cannot open " .. mbr_magic)
            file:write("\x55\xaa")
            file:close()
            -- extra kernel
            local kernel = _replace_ext(target, ".bin")
            local objcopy = assert(find_tool("objcopy"), "objcopy not found!")
            os.iorunv(objcopy.program, {"-S", target:targetfile(), kernel})
            progress.show(opt.progress, "${color.build.target}extra %s", target:targetfile())
            -- get boot loader
            local imagefile = _replace_ext(target, ".img")
            local boot_loader_file = _replace_ext(project.target("boot_loader"), ".bin")
            local setup_file = _replace_ext(project.target("setup"), ".bin")

            local dd = "dd"
            os.iorunv(dd, {"if=/dev/zero", "of=" .. imagefile, "bs=512", "count=2"})
            os.iorunv(dd, {"if=" .. boot_loader_file, "of=" .. imagefile, "bs=446", "count=1", "conv=notrunc"})
            os.iorunv(dd, {"if=" .. mbr_magic, "of=" .. imagefile, "bs=510", "seek=1", "conv=notrunc"})
            os.iorunv(dd, {"if=" .. setup_file, "of=" .. imagefile, "bs=512", "seek=1", "conv=notrunc"})
            os.iorunv(dd, {"if=" .. kernel, "of=" .. imagefile, "bs=512", "seek=2", "conv=notrunc"})
            progress.show(opt.progress, "${color.build.target}create %s", imagefile)
        end, {files = {target:targetfile()}})
    end)

    on_clean(function (target)
        import("core.project.project")
        local mbr_magic = path.join(target:targetdir(), "mbr_magic")
        local boot_loader_file = _replace_ext(project.target("boot_loader"), ".bin")
        local setup_file = _replace_ext(project.target("setup"), ".bin")
        local kernel = _replace_ext(target, ".bin")
        local imagefile = _replace_ext(target, ".img")
        os.tryrm(mbr_magic)
        os.tryrm(boot_loader_file)
        os.tryrm(setup_file)
        os.tryrm(kernel)
        os.tryrm(imagefile)
    end)
rule_end()

add_includedirs("include")

target("boot_loader")
    set_arch("i386")
    set_kind("binary")
    set_optimize("smallest")
    set_toolset("ld", "ld")
    add_rules("freestanding", "extra_obj")

    add_files("src/boot_loader/boot.S")
    add_files("src/boot_loader/loader.c")

    add_ldflags("-e", "real_mode", "-Ttext", "0x7c00", {force = true})

    on_run(function () end)
target_end()

target("setup")
    set_arch("x64")
    set_kind("binary")
    set_optimize("smallest")
    set_toolset("ld", "ld")
    add_rules("freestanding", "extra_obj")

    add_files("src/boot_loader/entry_64.S")

    add_ldflags("-e", "entry_64", "-Ttext", "0x0700", {force = true})
    on_run(function () end)
target_end()

target("kernel")
    set_arch("x64")
    set_kind("binary")
    set_toolset("ld", "ld")
    add_rules("freestanding", "image")

    add_files("src/*.c")
    add_files("src/*.S")

    add_ldflags("-T", "script/kernel.ld", {force = true})

    add_deps("boot_loader", "setup")

    on_run(function (target)
        import("lib.detect.find_tool")
        local qemu = assert(find_tool("qemu-system-x86_64"), "qemu not found!")
        local image = _replace_ext(target, ".img")

        local flags =
        {
            "-no-shutdown", "-no-reboot",
            "-nographic",
            "-m", "32M"
        }

        if is_mode("debug") then
            -- listen 1234 port
            table.join2(flags, {"-s", "-S"})
        end

        table.join2(flags, {"-hda", image})

        os.execv(qemu.program, flags)
        -- gdb -q -x script/.gdbinit
    end)
target_end()
