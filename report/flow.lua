local flow = {};

function flow.draw(body)
  local f = io.popen("./autoflow/a.out <<EOF\n"..body.."EOF\n");
  for line in f:lines() do
    tex.print(line)
    print(line);
    print(#line)
  end
  --local s = f:read("*a");
  --tex.sprint(s);
end

return flow;

