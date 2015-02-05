print('httpget.lua started')
Tstart  = tmr.now()
print(Tstart)

sk = nil
sk=net.createConnection(net.UDP, 0) 
sk:on("sent", function(sck, c) sck:send("salam salam salam\r\n") end )
sk:on("receive", function(sck, c) print(c) end )
--sk:on("connection", function(sck, c) sck:send("start") end )
--sk:on("reconnection", function(sck, c) print(c) end )
--sk:on("disconnection", function(sck, c) print(c) end )

sk:connect(5000,"192.168.1.9")
--sk:send("GET /ap HTTP/1.1\r\nHost: 192.168.1.9\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")
tmr.alarm(0, 1000, 0, function() print("hello world") sk:send("start") end )