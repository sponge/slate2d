require('vstudio')

premake.override(premake.vstudio.dotnetbase, "projectProperties", function(base, prj)
  if string.find(prj.filename, 'csgame') then
    premake.w('  <PropertyGroup>')
    premake.w('    <ImplicitUsings>enable</ImplicitUsings>')
    premake.w('    <Nullable>enable</Nullable>')
    premake.w('    <PublishTrimmed>true</PublishTrimmed>')
    premake.w('    <PublishRelease>true</PublishRelease>')
    premake.w('    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>')
    premake.w('    <Platforms>x86;x64</Platforms>')
    -- premake.w('    <PublishAot>true</PublishAot>')
    premake.w('  </PropertyGroup>')

    premake.w('  <ItemGroup>')
    premake.w('    <ContentWithTargetPath Include="../build/bin/x86_64_$(Configuration)/*.dll">')
    premake.w('    <CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>')
    premake.w('    <TargetPath>%%(Filename)%%(Extension)</TargetPath>')
    premake.w('    </ContentWithTargetPath>')
    premake.w('  </ItemGroup>')
  end
  base(prj)
end)

function csgame()
  language "c#"
  kind "WindowedApp"
  dotnetframework "net7.0"
  files { "csgame/**.cs" }
  disablewarnings { "CS8625", "CS8600", "CS8604" } -- disable some warnings in imgui.net
end

function dump(o)
  if type(o) == 'table' then
     local s = '{ '
     for k,v in pairs(o) do
        if type(k) ~= 'number' then k = '"'..k..'"' end
        s = s .. '['..k..'] = ' .. tostring(v) .. ','
     end
     return s .. '} '
  else
     return tostring(o)
  end
end

if included ~= true then
  workspace "csgame"
    configurations { "Debug", "Release" }
    location "build"
    project "csgame_standalone"
    csgame()
end