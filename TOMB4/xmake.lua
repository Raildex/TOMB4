add_requires("zlib","xaudio2redist")
target("trng2")
add_packages("zlib")
	set_kind("binary")
	add_files("game/*.cpp")
	add_files("specific/*.cpp")
	add_files("tomb4/*.cpp")
	set_pcxxheader("tomb4/pch.h")
	if(is_plat("windows")) then
		add_packages("xaudio2redist")
		add_syslinks("Msacm32","d3d9","ddraw","dsound","Winmm","Gdi32","Advapi32","user32","dxgi", "dxguid","dinput8","xaudio2_9redist","Ole32")
		add_files("tomb4/tomb4.rc")
		add_defines("DIRECT3D_VERSION=0x700")
	end
	if(is_mode("debug")) then 
		add_defines("DO_LOG")
	end
target_end()