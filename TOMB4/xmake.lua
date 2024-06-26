add_requires("zlib","xaudio2redist")
target("trng2")
add_packages("zlib")
	set_kind("binary")
	add_languages("c17","c++11")
	add_files("game/*.c")
	add_files("specific/*.c")
	add_includedirs("./")
	add_headerfiles("game/*.h")
	add_headerfiles("global/*.h")
	add_headerfiles("specific/*.h")
	set_warnings("all","error")
	add_cflags("Wnopointer-to-int-cast")
	if(is_plat("windows")) then
		add_packages("xaudio2redist")
		add_syslinks("Msacm32","d3d9","ddraw","dsound","Winmm","Gdi32","Advapi32","user32","dxgi", "dxguid","dinput8","xaudio2_9redist","Ole32")
		add_files("tomb4/tomb4.rc")
		add_defines("XAUDIO2_HELPER_FUNCTIONS","WIN32_LEAN_AND_MEAN","NOMINMAX", "_CRT_SECURE_NO_WARNINGS")
	end
	if(is_mode("debug")) then 
		add_defines("DO_LOG")
	end
	add_defines("_USE_MATH_DEFINES")
target_end()