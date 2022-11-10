# ESP32_UIPEthernet_thingspeak

<img src="./ESP32_UIPEthernet_thingspeak.png" alt="" width="500" height="344" title="">


 ![post-online](https://reqbin.com/post-online) 
 
  ![thingspeak](https://thingspeak.com/channels/1623046) 



## POST - Add a TalkBack Command
```js
POST /talkbacks/47146/commands.json?api_key=OSZC0RZD8PZFWLZX HTTP/1.1
Host: api.thingspeak.com
Accept: application/json
Content-Type: text/html
Content-Length: 21

command_string=3_OFF
```

## GET - List All TalkBack Commands
```js
GET /talkbacks/47146/commands.json?api_key=OSZC0RZD8PZFWLZX HTTP/1.1
Host: api.thingspeak.com
Accept: application/json
```

## DELETE - Delete All TalkBack Commands
```js
DELETE /talkbacks/47146/commands.json?api_key=OSZC0RZD8PZFWLZX HTTP/1.1
Host: api.thingspeak.com
Accept: application/json
```
