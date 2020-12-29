import requests

def func (msgDict):
    helpMsg = '狸的一个访问wttr.in的小玩意'
    req = list(msgDict['raw_message'].split())
    for i in range(0, len(req)):
        req[i] = req[i].strip()
    if i == 0:
        ans = helpMsg
    else:
        ans = '[CQ:image,file=http://wttr.in/' + req[1] + '_tqp0_lang=en.png]'

    return ans
 
