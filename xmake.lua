add_rules("mode.debug", "mode.release")


add_requires("libsdl")
add_requires("libsdl_image")



target("Program")
    set_kind("binary")
    add_files("src/*.c")
    add_packages("libsdl")
    add_packages("libsdl_image")

