add_requires("zlib","xaudio2redist")
target("trng2")
add_packages("zlib")
	set_kind("binary")
	add_languages("c23")
	add_files("game/*.c")
	add_files("global/*.c")
	add_includedirs("./")
	add_headerfiles("game/*.h")
	add_headerfiles("global/*.h")
	add_headerfiles("specific/*.h")
	set_warnings("all","error")
	set_warnings("extra","error")
	set_warnings("pedantic", "error")
	if(is_plat("windows")) then
		add_files("specific/windows/*.c")
		add_packages("xaudio2redist")
		add_syslinks("Msacm32","d3d9","ddraw","dsound","Winmm","Gdi32","Advapi32","user32","dxgi", "dxguid","dinput8","xaudio2_9redist","Ole32")
		add_files("tomb4/tomb4.rc")
		add_defines("XAUDIO2_HELPER_FUNCTIONS","WIN32_LEAN_AND_MEAN","NOMINMAX", "_CRT_SECURE_NO_WARNINGS", "COBJMACROS", "CINTERFACE")
		set_runtimes(is_mode("debug") and "MTd" or "MT")
	end
	if(is_mode("debug")) then 
		add_defines("DO_LOG")
		add_defines("_DEBUG")
	end

	add_defines("_USE_MATH_DEFINES")
target_end()