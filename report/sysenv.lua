local sysenv = {};

function sysenv.get_version(cmd)
  local f = io.popen(cmd.." --version")
  local s = f:read("l")
  local v = string.gmatch(s, "%d+%.%d+%.%d+")()
  if v ~= nil and v ~= '' then
    tex.print(cmd .. ' ' .. v)
  end
end

function sysenv.print_sysinfo()
  local f = io.popen('lscpu | grep "Model name"')
end

function sysenv.print_sysversion(cmds)
  tex.print("\\begin{itemize}")
  for i,v in ipairs(cmds) do
    tex.print("  \\item ")
    sysenv.get_version(v)
  end
  tex.print("\\end{itemize}")
end

return sysenv;
