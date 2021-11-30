function dirname(fullpath)
    fullpath = fullpath:gsub('[\n\r]*$','')

	local s = string.match(fullpath, "[^/]*[/][^/]*$");
    return string.sub(s, 1, string.find(s, "/")-1)
end


print("\n");
Master_Folder_Name = dirname(debug.getinfo(1,"S").source:sub(2));
print(Master_Folder_Name)


function get_files_paths(Folder_Name)
    return { 
        gkit_dir .. "/" .. Master_Folder_Name .. "/" .. Folder_Name .. "/*.c*", 
        gkit_dir .. "/" .. Master_Folder_Name .. "/" .. Folder_Name .. "/*.h*", 
        gkit_dir .. "/" .. Master_Folder_Name .. "/" .. Folder_Name .. "/**/*.c*", 
        gkit_dir .. "/" .. Master_Folder_Name .. "/" .. Folder_Name .. "/**/*.h*", 
    }
end

function create_gkit_project(Folder_Name)
    print(" - Project:", Folder_Name);
    
    project(Folder_Name)
        language "C++"
        kind "ConsoleApp"
        targetdir ( gkit_dir .. "/bin/")
        files ( gkit_files )
        files ( get_files_paths(Folder_Name) )
        files { gkit_dir .. "/" .. Master_Folder_Name .. "/" .. Folder_Name .. "/main.cpp" }
end



--create_gkit_project( "template" );
create_gkit_project( "mmv_tp" );


print("\n");