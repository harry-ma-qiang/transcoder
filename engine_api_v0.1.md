![media_engine](assets/media_engine.svg)

Our goal is to build a media engine,As shown above, we want the media engine to have these features. File transcoding,Live,Video wall, Pub/Sub service, Engine protection。The new media engine needs to have the functions shown at the current stage. The media engine needs to provide a standardized and unified interface, and the user only needs to provide corresponding parameters according to the interface to complete the corresponding function. The following information is a description of the function and interface.

Note: The base address is `http://xxengine.com/api` ，If the request path for adding a signal is `/signal`, the request address is `http://xxengine.com/api/signal` and the interface address is modified according to the actual situation.

## Signal

Signal management is used to manage live signals, such as creating HLS live , updating live configurations, and deleting live task.

| Features | Description |
| ------------ | ------------- |
| Add Signal | Provide video stream address, transcoding parameters, output protocol parameters to the media engine, and create a live stream |
| Update Signal | Update signal configuration information |
| Delete Signal | Delete the signal according to the signal unique identifier |
| State management | Start streaming or stop streaming |



| Supported input | Supported output |
| --------------- | ---------------- |
| SPTS over UDP   |                  |
| MPTS over UDP   | HLS              |
| RTMP            |                  |
| HLS             |                  |

Note:

1. In the future, we hope to support RTMP,HLS,HLS adaptive ,DASH... but Priority support for HLS and HLS adaptive protocol output at this stage

### Add signal

**Description：** 

- The purpose of this feature is to create a live streaming. Users need to provide live broadcast signals, transcoding and output protocol required parameters. Finally generate a live broadcast address.

**Request path：** 

- ` /signal`

**Method：**

- POST 

**body：** 

```
{
    "signalId": "123456789123456789",
    "signalSource": "udp://239.78.78.78:59000",
    "programId": 12,
    "enableTranscode": true,
    "config:": [
        {
            "codec": "avc",
            "bitRate": 2500,
            "rateControlMethod": "vbr",
            "profile": "high",
            "frameRate": 25,
            "resolution": {
                "width": 1080,
                "height": 720
            }
        }
    ],
    "hls": {
        "segTime": 10,
        "segNum": 3
    }
}
```

**parameter：**

| parameter       | Required | Type    | Description                                               |
| :-------------- | :------- | :------ | --------------------------------------------------------- |
| signalId        | YES      | String  | Signal unique identifier                                  |
| signalSource    | YES      | String  | Source address                                            |
| programId       | YES      | int     | If the source is mpts we need program Id                  |
| enableTranscode | YES      | Boolean | Whether to enable transcoding                             |
| config          | YES      | Array   | Transcoding parameters (see table below)                  |
| hls             | YES      | Object  | HLS output protocol parameters                            |
| -segTime        | YES      | int     | Single slice time default 10 seconds                      |
| -segNum         | YES      | int     | The number of slices included in the m3u8 file, default 3 |

 Note:

- Multiple transcoding parameters mean HLS adaptive streaming

  

**transcoding parameters:** 

| parameter         | Required | Type   | Description                                                  |
| :---------------- | :------- | :----- | ------------------------------------------------------------ |
| codec             | YES      | String | video codec                                                  |
| bitRate           | YES      | int    | Video bitrate,  Unit kbps                                    |
| rateControlMethod | YES      | String | RateControlMethod: vbr/cbr/la                                |
| profile           | YES      | String | profile: high/main/baseline<br>>720P:high, <br>>480P:main, <br>other: baseline |
| frameRate         | YES      | int    | FrameRate:   0 means "copy"                                  |
| resolution        | YES      | Object | Video resolution                                             |
| - width           | YES      | int    | width                                                        |
| -height           | YES      | int    | height                                                       |

**response**

```
{
    "code": 200,
    "msg": "create signal success",
    "result":true
}
```

**Response parameter** 

| parameter | Type    | Description                                            |
| :-------- | :------ | ------------------------------------------------------ |
| code      | int     | code:200 The request is successful, otherwise it fails |
| msg       | string  | Response  message                                      |
| result    | Boolean | Request result                                         |

### Update signal

**Description：** 

- Update the configuration of the live broadcast

**Request path：** 

- ` /signal/{signalId}`

**Method：**

- POST 

**parameter：** 

| parameter | Required | Type   | Description              |
| :-------- | :------- | :----- | ------------------------ |
| signalId  | YES      | String | Signal unique identifier |

**body：** 

```
{
    "signalSource": "udp://239.78.78.78:59000",
    "programId": 12,
    "enableTranscode": true,
    "config": [
        {
            "codec": "avc",
            "bitRate": 2500,
            "rateControlMethod": "vbr",
            "profile": "high",
            "frameRate": 25,
            "resolution": {
                "width": 1080,
                "height": 720
            }
        }
    ],
    "hls": {
        "segTime": 10,
        "segNum": 3
    }
}
```

**parameter：**

| parameter       | Required | Type    | Description                                               |
| :-------------- | :------- | :------ | --------------------------------------------------------- |
| signalSource    | YES      | String  | Source address                                            |
| programId       | YES      | int     | If the source is mpts we need program Id                  |
| enableTranscode | YES      | Boolean | Whether to enable transcoding                             |
| config          | YES      | Array   | Transcoding parameters (see table below)                  |
| hls             | YES      | Object  | HLS output protocol parameters                            |
| -segTime        | YES      | int     | Single slice time default 10 seconds                      |
| -segNum         | YES      | int     | The number of slices included in the m3u8 file, default 3 |

**transcoding parameters:** 

| parameter         | Required | Type   | Description                                                  |
| :---------------- | :------- | :----- | ------------------------------------------------------------ |
| codec             | YES      | String | video codec                                                  |
| bitRate           | YES      | int    | Bitrate,  Unit kbps                                          |
| rateControlMethod | YES      | String | RateControlMethod: vbr/cbr/la                                |
| profile           | YES      | String | profile: high/main/baseline<br>>720P:high, <br>>480P:main, <br>other: baseline |
| frameRate         | YES      | int    | FrameRate:   0 means "copy"                                  |
| resolution        | YES      | Object | Video resolution                                             |
| - width           | YES      | int    | width                                                        |
| -height           | YES      | int    | height                                                       |

**response**

```
{
    "code": 200,
    "msg": "Update signal config success",
    "result":true
}
```

**Response parameter** 

| parameter | Type    | Description                                            |
| :-------- | :------ | ------------------------------------------------------ |
| code      | int     | code:200 The request is successful, otherwise it fails |
| msg       | string  | Single slice time default 10 seconds                   |
| result    | Boolean | Request result                                         |

### Delete signal

**Description：** 

- If the live signal is not needed, delete it by the unique ID of the signal.

**Request path：** 

- ` /signal/{signalId}`

**Method：**

- DELETE

**parameter**

| parameter | Required | Type   | Description              |
| :-------- | :------- | :----- | ------------------------ |
| signalId  | YES      | String | Signal unique identifier |

**Response**

```
  {
    "code": 200,
    "msg":"Delete signal successfully",
    "result":true
  }
```

**Response parameter** 

| parameter | Type    | Description                                            |
| :-------- | :------ | ------------------------------------------------------ |
| code      | int     | code:200 The request is successful, otherwise it fails |
| msg       | string  | Response  message                                      |
| result    | Boolean | Delete signal result                                   |

### State management

**Description：** 

- Start streaming or stop streaming

**Request path：** 

- ` /signal/action`

**Method：**

- GET

**parameter：** 

| parameter | Description              |
| --------- | ------------------------ |
| signalId  | Signal unique identifier |
| op        | start/stop               |

**Response**

```
{
    "code": 200,
    "msg": "start success",
    "result":true
}
```

**Response parameter** 

| parameter | Type    | Description                                            |
| :-------- | :------ | ------------------------------------------------------ |
| code      | int     | code:200 The request is successful, otherwise it fails |
| msg       | string  | Response  message                                      |
| result    | Boolean | Operation result                                       |

## Video wall

Combine multiple different video frames into the same one,   Combine multiple different video images into the same video stream and output them in HLS protocol.  The default resolution is 720x480

| Features          | Description                                                  |
| ----------------- | ------------------------------------------------------------ |
| Add video wall    | Select a set of video source streams and combine them into one |
| Update video wall | Update the configuration of the video wall, such as the number of video source streams |
| Delete video wall | Delete video wall                                            |

### Add video wall

**Description：** 

- Select a set of video source streams and combine them into one stream and output them in HLS protocol

**Request path：** 

- ` /wall`

**Method：**

- POST 

**body：** 

```
{
	"wallId","1234567123456"
	"wallSource":[123456123456]
}
```

**parameter：**

| parameter  | Required | Type   | Description           |
| :--------- | :------- | :----- | --------------------- |
| wallId     | YES      | String | Video wall unique ID  |
| wallSource | YES      | Array  | Signal unique ID list |

**Response**

```
{
    "code": 200,
    "msg": "create TVWall success",
    "result":true
}
```

**Response parameter** 

| parameter | Type    | Description                                            |
| :-------- | :------ | ------------------------------------------------------ |
| code      | int     | code:200 The request is successful, otherwise it fails |
| msg       | string  | Response  message                                      |
| result    | Boolean | Request result                                         |

### Update video wall

**Description：** 

- Update video wall config

**Request path：** 

- ` /wall/{wallId}`

**Method：**

- POST

**body：** 

```
{
	"wallSource":[1234234,2123233]
}
```

**parameter：** 

| parameter  | Required | Type   | Description           |
| :--------- | :------- | :----- | --------------------- |
| wallId     | YES      | string | Video wall unique ID  |
| wallSource | YES      | Array  | Signal unique ID list |

**Response**

```
  {
    "code": 200,
    "msg":"update TVWall successfully",
    "result":true
  }
```

**Response parameter** 

| parameter | Type    | Description                                            |
| :-------- | :------ | ------------------------------------------------------ |
| code      | int     | code:200 The request is successful, otherwise it fails |
| msg       | string  | Response  message                                      |
| result    | Boolean | Request result                                         |

### Delete video wall

**Description：** 

- Delete video wall

**Request path：** 

- ` /wall/{wallId}`

**Method：**

- DELETE

**parameter：** 

| parameter | Required | Type   | Description          |
| :-------- | :------- | :----- | -------------------- |
| wallId    | YES      | String | Video wall unique ID |

**Response**

```
  {
    "code": 200,
    "msg":"Delete TVWall successfully",
    "result":true
  }
```

 **Response parameter** 

| parameter | Type    | Description                                            |
| :-------- | :------ | ------------------------------------------------------ |
| code      | int     | code:200 The request is successful, otherwise it fails |
| msg       | string  | Response  message                                      |
| result    | Boolean | Request result                                         |

## File transcoding

The media engine transcodes based on the video file address and transcoding configuration and outputs it in the HLS protocol. The engine needs to provide the necessary management operations, such as canceling the transcoding task, and can query the transcoding details and the progress of the transcoding.

| Features          | Description                      |
| ----------------- | -------------------------------- |
| Add task          | Create a video transcoding task  |
| Cancel task       | Cancel  a video transcoding task |
| Query task detail | Query task details and progress  |

### Add task

**Description：** 

- Create a video transcoding task,Output HLS protocol

**Request path：** 

- ` /task`

**Method：**

- POST 

**body：** 

```
{
    "taskId": "123456789123456789",
    "videoSource": "http://file.com/file.mp4",
    "config": [
        {
            "video": {
                "codec": "avc",
                "bitRate": 2500,
                "rateControlMethod": "vbr",
                "profile": "high",
                "frameRate": 25,
                "resolution": {
                    "width": 1080,
                    "height": 720
                }
            },
            "audio": {
                "codec": "aac",
                "sampleRate": 48000,
                "bitRate": 128
            }
        }
    ]
}
```

**parameter：**

| parameter   | Required | Type   | Description                    |
| :---------- | :------- | :----- | ------------------------------ |
| taskId      | YES      | String | Transcoding task unique ID     |
| videoSource | YES      | String | Video file address             |
| config      | YES      | Array  | File transcoding configuration |
| -video      | YES      | Object | Video stream parameters        |
| -audio      | YES      | Object | Audio stream parameters        |

**Video stream parameters：** 

| parameter         | Required | Type   | Description                                                  |
| :---------------- | :------- | :----- | ------------------------------------------------------------ |
| codec             | YES      | String | Video codec                                                  |
| bitRate           | YES      | int    | Bitrate,  Unit kbps                                          |
| rateControlMethod | YES      | String | RateControlMethod: vbr/cbr/la                                |
| profile           | YES      | String | profile: high/main/baseline<br/>>720P:high, <br/>>480P:main, <br/>other: baseline |
| frameRate         | YES      | int    | FrameRate:   0 means "copy"                                  |
| resolution        | YES      | Object | Video resolution                                             |
| - width           | YES      | int    | width                                                        |
| -height           | YES      | int    | height                                                       |

**Audio stream parameters：** 

| parameter  | Required | Type   | Description      |
| :--------- | :------- | :----- | ---------------- |
| codec      | YES      | String | Audio codec      |
| sampleRate | YES      | int    | Audio SampleRate |
| bitRate    | YES      | int    | BitRate          |


**Response**

```
  {
    "code": 200,
    "msg":"created file transcoding task Successfully",
    "result":true
  }
```

 **Response parameter** 

| parameter | Type    | Description                                            |
| :-------- | :------ | ------------------------------------------------------ |
| code      | int     | code:200 The request is successful, otherwise it fails |
| msg       | string  | Response message                                       |
| result    | Boolean | Create file transcoding task results                   |

### Cancel task

**Description：** 

- Cancel transcoding task

**Request path：** 

- ` /task/{taskId}`

**Method：**

- DELETE

**parameter：** 

| parameter | Required | Type   | Description                        |
| :-------- | :------- | :----- | ---------------------------------- |
| taskId    | YES      | String | Transcoding task unique identifier |

**Response**

```
  {
    "code": 200,
    "msg":"Delete file transcoding task Successfully",
    "result":true
  }
```

 **Response parameter** 

| parameter | Type    | Description                                            |
| :-------- | :------ | ------------------------------------------------------ |
| code      | int     | code:200 The request is successful, otherwise it fails |
| msg       | string  | Response message                                       |
| result    | boolean | Cancel the transcoding task result                     |

### Query task detail

**Description：** 

- Query transcoding task details

**Request path：** 

- ` /task/{taskId}`

**Method：**

- GET

**body：** 

```
{
    "taskId": "123456789123456789",
    "videoSource": "http://file.com/file.mp4",
    "progress": 0.96,
    "config": [
        {
            "video": {
                "codec": "avc",
                "bitRate": 2500,
                "rateControlMethod": "vbr",
                "profile": "high",
                "frameRate": 25,
                "resolution": {
                    "width": 1080,
                    "height": 720
                }
            },
            "audio": {
                "codec": "aac",
                "sampleRate": 48000,
                "bitRate": 128
            }
        }
    ]
}
```

**parameter：**

| parameter   | Required | Type   | Description                |
| :---------- | :------- | :----- | -------------------------- |
| taskId      | YES      | String | Transcoding task unique ID |
| videoSource | YES      | String | Video file address         |
| progress    | YES      | float  | Video transcoding progress |
| config      | YES      | Array  | File transcoding parameter |
| -video      | YES      | Object | Video stream parameters    |
| -audio      | YES      | Object | Audio stream parameters    |

Note: For file transcoding parameters, see "Add Task" above.

## Pub/Sub

The user submits the push address to the media engine as a subscription, The engine pushes relevant information to the address, The following fields are push information fields. For the time being, only the real-time data of the signal is pushed. If there is other information, the field will be added and the meaning of the field will be rich.

| Field     | Description                                                  |
| --------- | ------------------------------------------------------------ |
| tag       | "signal  " Indicates that the data is real-time information of the signal |
| type      | 1. "input"  Signal input stream information  <br>2. "output "  Signal output stream information |
| status    | "resume":    Live start or recovery <br>"suspend":  Live interruption<br>"abnormal":  Video stream is abnormal <br>"normal": Video stream is normal |
| msg       | message                                                      |
| timestamp | Time of occurrence                                           |

### subscription

**Description：** 

- The user submits the push address to the media engine, after which the engine will push the message to the address.

**Method：**

- POST

**body：**

```
{
  "pushUrl":"http://127.0.0.1:8800/push"
}
```

**parameter：**

| parameter | Required | Type   | Description        |
| :-------- | :------- | :----- | ------------------ |
| pushUrl   | YES      | String | Subscriber address |

**Response**

```
{
    "code": 200,
    "msg": "Update push address",
    "result":true
}
```

**Response parameter** 

| parameter | Type    | Desc                                                   |
| :-------- | :------ | ------------------------------------------------------ |
| code      | int     | code:200 The request is successful, otherwise it fails |
| msg       | string  | Response  message                                      |
| result    | Boolean | Request result                                         |

### Push input information

**Description：** 

- The media engine push live input stream information to subscribers

**Method：**

- POST

**body：** 

```
{
    "tag": "signal",
    "signalId": "123456123456",
    "type": "input",
    "status": "",
    "msg": "",
    "timestamp":1566195376,
    "data": {
        "vLeft": 0,
        "vReft": 0,
        "frameRate": 30,
        "BitRate": 2500,
        "video": [
            {
                "enabled": true,
                "codec": "avc",
                "frameRate": 30,
                "index": 0,
                "resolution": {
                    "width": 1920,
                    "height": 1080
                }
            }
        ],
        "audio": [
            {
                "enabled": true,
                "codec": "mp2",
                "channels": 2,
                "index": 0,
                "sampleRate": 48000
            }
        ]
    }
}
```
**live input**

| parameter   | Required | Type    | Description                         |
| :---------- | :------- | :------ | ----------------------------------- |
| vLeft       | YES      | int     | Left channel volume                 |
| vReft       | YES      | int     | Right channel volume                |
| frameRate   | YES      | int     | FrameRate                           |
| bitRate     | YES      | int     | bitRate                             |
| video       | YES      | Array   | Video stream information            |
| -enabled    | YES      | Boolean | Whether the video stream is enabled |
| -codec      | YES      | String  | Codec                               |
| -frameRate  | YES      | int     | FrameRate                           |
| -index      | YES      | int     | Index                               |
| -resolution | YES      | Object  | Resolution                          |
| --width     | YES      | int     | Width                               |
| --height    | YES      | int     | Height                              |
| audio       | YES      | Array   | Audio stream information            |
| -enabled    | YES      | Boolean | Whether the audio stream is enabled |
| -codec      | YES      | String  | Codec                               |
| -channels   | YES      | int     | Number of channels                  |
| -index      | YES      | int     | Index                               |
| -sampleRate | YES      | int     | SampleRate                          |

### Push output information

**Description：** 

- The media engine push live stream output information to subscribers

**Method：**

- POST

**body：** 

```
{
    "tag": "signal",
    "signalId": "123456123456",
    "type": "output",
    "status": "",
    "msg": "",
    "timestamp":1566195376,
    "data": {
        "vLeft": 0,
        "vReft": 0,
        "frameRate": 30,
        "BitRate": 2500,
        "video": [
            {
                "enabled": true,
                "codec": "avc",
                "frameRate": 30,
                "index": 0,
                "resolution": {
                    "width": 1920,
                    "height": 1080
                }
            }
        ],
        "audio": [
            {
                "enabled": true,
                "codec": "mp2",
                "channels": 2,
                "index": 0,
                "sampleRate": 48000
            }
        ]
    }
}
```

**Note:** For the meaning of the fields, see "Push input information"

## License

The authorization method of the new media engine has not been determined yet. How to authorize and how to securely save the authorization information needs to be defined by the media engine. If the media engine has protection enabled, the user needs to provide authorization information to unlock the features of the media engine, otherwise the engine will stop working. The following functions are required, but the parameters need to be determined according to the protection mechanism of the media engine.

| Features       | Description                                                  |
| -------------- | ------------------------------------------------------------ |
| Query License  | Query the authorization information of the media engine      |
| Update License | Update the license information to the media engine to extend the usage period. |

### Query

**Description：** 

- Obtain authorization information

**Request path：** 

- ` /license`

**Method：**

- GET

**Response**

```
{
    "code": 200,
    "msg": "Query license",
    "result": {
        "expired":1566195376,
        "signer":"XIESHIMEI",
        "issueDate":1566395376 
    }
}
```

**Response parameter** 

| parameter  | Type   | Description                                            |
| :--------- | :----- | ------------------------------------------------------ |
| code       | int    | code:200 The request is successful, otherwise it fails |
| msg        | String | Response message                                       |
| result     | Object |                                                        |
| -expired   | int    | Expiration timestamp                                   |
| -signer    | String | signer                                                 |
| -issueDate | int    | Signing timestamp                                      |

### Update

**Description：** 

- Update authorization information

**Request path：** 

- ` /license`

**Method：**

- POST

**body：** 

```
{
	"license":"gcbPiR3/4UzYZfG0fzy7+czaqft/tZmR6Itkhkqv2L+NmeVChMwSTS2jBC0Fizcmc+CCvyTLWtzPEJT0Bjzz4lcqCORgOcegaR2NFeZXbH6YligKORyDZOc5mmTGxVySWjIhUFRTZUrxkfjtZAHvuZCRkDOHYnX6UFvqL7cv50yqxZFRE40j6B0KwVYPHnqcfOv7o9cRJ58F9PbcnRQGh1sEWT9w9rdo/l2RoH+rusx3QyPJAsI7R5Yri+zvfSBmjSxYL+dAMMaW5WHo8BSgo7+nfxZolhWm9QOqCyCB0BfMB8bONguCqvGvblCcZJzZhLXYcODqZdBSgFE4nAoq/A/LAV+A32muRMiC/3DD55z1okC6Q4dPYEAdSZ/CbAcMUCVw4fRc9wCJgJReg/J6IHmxsYCU1T+qUTHQ3Xmj2Zu56DUyGHAy3fnwdk7ztKQmn3GNQp9clwozxWWYsGaM3S2wMJdI5R/QS0WJVniCAD/hdSpsmPYXRChUNKZsDnE0v0Yf2jxfRDAQhsoH7tyMzJxbKeIGnzWZV8K/bG1z4gh1KcDr0oTv4KmCH1xONFoxSrZoradHQg9FZd9AEjitG1JP5oBEYB9QWF+YcNEJuDrkyTtNkJgwqhcmq7KS/Oy91Ve0TzE55nfBJag6hYbzQFxb2f0R/inTNZ94XKqEFoc=@MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAi8a9gmo4ZQwcv1KUyTSubpAJZP7Mwhm0j/0prIVquFJzH1kIBYhp+IzxgWSC6QJ3mfOdlkf0WBLDKfnPQWQuJEMkz0+GFzr+n+nu3HsbNJRFgY5AidSchJ72j/tQnPY+h0wgXGMfZDYEyrkmRYDezItWjrhBKVy7q3M4x2r0J0jrWPUJ/x+sIcQ8PKNIsrF/LvBaeYpkCk8hyG7JXQe0unn3HoC9ZBJlHpqRs7wHQiRbCCDu8DfRKnVYBbeqEEb8z9xLAPCe1SApnar5unSAw50/IaZeliKOgYLsy1D+8HVF3fwGlZcRuBGBGxVjQrgmw33V2m45VooucI5d37soGwIDAQAB"
}
```

**parameter** 

| parameter | Type   | Description         |
| :-------- | :----- | ------------------- |
| license   | String | License information |

**Response**

```
{
    "code": 200,
    "msg": "Update license",
    "result":true
}
```

**Response parameter** 

| parameter | Type    | Description                                            |
| :-------- | :------ | ------------------------------------------------------ |
| code      | int     | code:200 The request is successful, otherwise it fails |
| msg       | string  | Response message                                       |
| result    | Boolean | Request result                                         |



## Change log

| Version | Time       | Creator | Detail |
| ------- | ---------- | ------- | ------ |
| v0.1    | 2019-09-04 | Lucas   | create |
|         |            |         |        |
|         |            |         |        |

