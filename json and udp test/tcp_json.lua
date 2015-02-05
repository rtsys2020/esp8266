
print('httpget.lua started')
t=tmr.now()
tmr.delay(1000)
tt=tmr.now()
print(tt-t)

t1=0
t2=0

sk = nil
sk=net.createConnection(net.TCP, 0) 
sk:on("sent", function(sck, c) print("send\r\n")  end )
sk:on("receive", function(sck, c) print(c) print("recive\r\n") t2=tmr.now() print("elapsed:") print(t2-t1) end )
sk:on("connection", function(sck, c) print(c) t1=tmr.now() end )
sk:on("reconnection", function(sck, c) print(c) end )
sk:on("disconnection", function(sck, c) print(c) end )

sk:connect(5000,"192.168.1.9")
--sk:send("GET /ap HTTP/1.1\r\nHost: 192.168.1.9\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n")
tmr.alarm(0, 1000, 0, function() print("hello world") sk:send("GET /ap HTTP/1.1\r\nHost: 192.168.1.9\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n") end )